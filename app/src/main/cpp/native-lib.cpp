#include <jni.h>
#include <string>
#include <syslog.h>
#include "android/log.h"
#include "mul.h"


extern "C"
#define byte(n,w)   (((w)>>(n*8)) & 0xff)
#define ainv_mul(w) (((w)>>8)^(strumok_alphainv_mul[w&0xff]))
#define a_mul(w)    (((w)<<8)^(strumok_alpha_mul[w>>56]))
#define T(w)        ((T0[byte(0,(w))])^(T1[byte(1,(w))])^(T2[byte(2,(w))])^(T3[byte(3,(w))])^(T4[byte(4,(w))])^(T5[byte(5,(w))])^(T6[byte(6,(w))])^(T7[byte(7,(w))]))


JNIEXPORT jobject JNICALL
Java_ua_dm_myapplication_Strumok_init(JNIEnv *env, jobject obj, jobject ctx) {
    // TODO: implement init()


    jclass systemParamClass = env -> FindClass("ua/dm/myapplication/SystemParam");

    ///init geters and seters method

    jmethodID getKeySize = env -> GetMethodID(systemParamClass,"getKeySize","()I");
    jmethodID setKeySize = env -> GetMethodID(systemParamClass,"setKeySize","(I)V");

    jmethodID getKey = env -> GetMethodID(systemParamClass, "getKey","()[J");
    jmethodID setKey = env -> GetMethodID(systemParamClass, "setKey","([J)V");//JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ

    jmethodID getR = env -> GetMethodID(systemParamClass, "getR","()[J");
    jmethodID setR = env -> GetMethodID(systemParamClass, "setR","([J)V");

    jmethodID getS = env -> GetMethodID(systemParamClass,"getS", "()[J");
    jmethodID setS = env -> GetMethodID(systemParamClass,"setS", "([J)V");

    jmethodID getIv = env -> GetMethodID(systemParamClass, "getIv","()[J");
    jmethodID setIv = env -> GetMethodID(systemParamClass, "setIv","([J)V");

    ///init jni variables

    jint jKeySize =  env-> CallIntMethod(ctx,getKeySize);
    jobject jObjectIV = env ->  CallObjectMethod(ctx,getIv);
    jobject jObjectKey = env ->CallObjectMethod(ctx,getKey);
    jobject jObjectS =  env -> CallObjectMethod(ctx,getS);
    jobject jObjectR =  env -> CallObjectMethod(ctx,getR);

    jlongArray tmpLongArray = static_cast<jlongArray>(jObjectKey);
    jlong *jKey = env -> GetLongArrayElements(tmpLongArray,0);

    tmpLongArray = static_cast<jlongArray>(jObjectIV);
    jlong *jIV = env -> GetLongArrayElements(tmpLongArray,0);



    tmpLongArray = static_cast<jlongArray>(jObjectR);
    jlong *jR = env -> GetLongArrayElements(tmpLongArray,0);

    tmpLongArray = static_cast<jlongArray>(jObjectS);
    jlong *jS = env -> GetLongArrayElements(tmpLongArray,0);

    ///init C variables
    uint64_t  S[16];
    uint64_t  r[2];
    uint64_t  key[8];
    uint64_t  iv[4];
    uint8_t   keySize; // Key size in bytes

    if(jKeySize == 32) {

        keySize = jKeySize;

        memcpy(iv, jIV, 32);
        memcpy(key, jKey, 32);

        S[0] = key[3] ^ iv[0];
        S[1] = key[2];
        S[2] = key[1] ^ iv[1];
        S[3] = key[0] ^ iv[2];
        S[4] = key[3];
        S[5] = key[2] ^ iv[3];
        S[6] = ~key[1];
        S[7] = ~key[0];
        S[8] = key[3];
        S[9] = key[2];
        S[10] = ~key[1];
        S[11] = key[0];
        S[12] = key[3];
        S[13] = ~key[2];
        S[14] = key[1];
        S[15] = ~key[0];
    }else if(jKeySize == 64){
        keySize = jKeySize;

        memcpy(iv, jIV, 32);
        memcpy(key, jKey, 64);

        S[0] = key[7] ^ iv[0];
        S[1] = key[6];
        S[2] = key[5];
        S[3] = key[4] ^ iv[1];
        S[4] = key[3];
        S[5] = key[2] ^ iv[2];
        S[6] = key[1];
        S[7] = ~key[0];
        S[8] = key[4] ^ iv[3];
        S[9] = ~key[6];
        S[10] = key[5];
        S[11] = ~key[7];
        S[12] = key[3];
        S[13] = key[2];
        S[14] = ~key[1];
        S[15] =key[0];
    }else{
        return NULL;
    }

    r[0] = 0;
    r[1] = 0;

    static uint64_t outfrom_fsm, fsmtmp;

    outfrom_fsm = (r[0] + S[15]) ^ r[1];
    S[0] = a_mul(S[0]) ^ S[13] ^ ainv_mul(S[11]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[13];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[0]) ^ r[1];
    S[1] = a_mul(S[1]) ^ S[14] ^ ainv_mul(S[12]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[14];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[1]) ^ r[1];
    S[2] = a_mul(S[2]) ^ S[15] ^ ainv_mul(S[13]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[15];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[2]) ^ r[1];
    S[3] = a_mul(S[3]) ^ S[0] ^ ainv_mul(S[14]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[0];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[3]) ^ r[1];
    S[4] = a_mul(S[4]) ^ S[1] ^ ainv_mul(S[15]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[1];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[4]) ^ r[1];
    S[5] = a_mul(S[5]) ^ S[2] ^ ainv_mul(S[0]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[2];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[5]) ^ r[1];
    S[6] = a_mul(S[6]) ^ S[3] ^ ainv_mul(S[1]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[3];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[6]) ^ r[1];
    S[7] = a_mul(S[7]) ^ S[4] ^ ainv_mul(S[2]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[4];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[7]) ^ r[1];
    S[8] = a_mul(S[8]) ^ S[5] ^ ainv_mul(S[3]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[5];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[8]) ^ r[1];
    S[9] = a_mul(S[9]) ^ S[6] ^ ainv_mul(S[4]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[6];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[9]) ^ r[1];
    S[10] = a_mul(S[10]) ^ S[7] ^ ainv_mul(S[5]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[7];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[10]) ^ r[1];
    S[11] = a_mul(S[11]) ^ S[8] ^ ainv_mul(S[6]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[8];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[11]) ^ r[1];
    S[12] = a_mul(S[12]) ^ S[9] ^ ainv_mul(S[7]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[9];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[12]) ^ r[1];
    S[13] = a_mul(S[13]) ^ S[10] ^ ainv_mul(S[8]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[10];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[13]) ^ r[1];
    S[14] = a_mul(S[14]) ^ S[11] ^ ainv_mul(S[9]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[11];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[14]) ^ r[1];
    S[15] = a_mul(S[15]) ^ S[12] ^ ainv_mul(S[10]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[12];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[15]) ^ r[1];
    S[0] = a_mul(S[0]) ^ S[13] ^ ainv_mul(S[11]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[13];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[0]) ^ r[1];
    S[1] = a_mul(S[1]) ^ S[14] ^ ainv_mul(S[12]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[14];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[1]) ^ r[1];
    S[2] = a_mul(S[2]) ^ S[15] ^ ainv_mul(S[13]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[15];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[2]) ^ r[1];
    S[3] = a_mul(S[3]) ^ S[0] ^ ainv_mul(S[14]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[0];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[3]) ^ r[1];
    S[4] = a_mul(S[4]) ^ S[1] ^ ainv_mul(S[15]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[1];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[4]) ^ r[1];
    S[5] = a_mul(S[5]) ^ S[2] ^ ainv_mul(S[0]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[2];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[5]) ^ r[1];
    S[6] = a_mul(S[6]) ^ S[3] ^ ainv_mul(S[1]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[3];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[6]) ^ r[1];
    S[7] = a_mul(S[7]) ^ S[4] ^ ainv_mul(S[2]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[4];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[7]) ^ r[1];
    S[8] = a_mul(S[8]) ^ S[5] ^ ainv_mul(S[3]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[5];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[8]) ^ r[1];
    S[9] = a_mul(S[9]) ^ S[6] ^ ainv_mul(S[4]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[6];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[9]) ^ r[1];
    S[10] = a_mul(S[10]) ^ S[7] ^ ainv_mul(S[5]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[7];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[10]) ^ r[1];
    S[11] = a_mul(S[11]) ^ S[8] ^ ainv_mul(S[6]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[8];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[11]) ^ r[1];
    S[12] = a_mul(S[12]) ^ S[9] ^ ainv_mul(S[7]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[9];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[12]) ^ r[1];
    S[13] = a_mul(S[13]) ^ S[10] ^ ainv_mul(S[8]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[10];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[13]) ^ r[1];
    S[14] = a_mul(S[14]) ^ S[11] ^ ainv_mul(S[9]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[11];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[14]) ^ r[1];
    S[15] = a_mul(S[15]) ^ S[12] ^ ainv_mul(S[10]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[12];
    r[1] = T(r[0]);
    r[0] = fsmtmp;

    outfrom_fsm = (r[0] + S[15]) ^ r[1];
    S[0] = a_mul(S[0]) ^ S[13] ^ ainv_mul(S[11]) ^ outfrom_fsm;
    fsmtmp = r[1] + S[13];
    r[1] = T(r[0]);
    r[0] = fsmtmp;


    jKeySize = keySize;
    memcpy(jIV, iv, 32);

    if(keySize == 64)
        memcpy(jKey, key, 64);
    else
        memcpy(jKey, key, 32);

    memcpy(jR, r, 16);
    memcpy(jS, S, 128);

    ///set keySize
    jvalue *value = new jvalue[1];
    value[0].i=jKeySize;
    env-> CallVoidMethodA(ctx, setKeySize, value);

    ///set key
    if(keySize == 32) {
        jlongArray arr = env->NewLongArray(32);
        env-> SetLongArrayRegion(arr,0,32, const_cast<const jlong*>(jKey));
        env-> CallVoidMethod(ctx,setKey,arr);
    }
    if(keySize == 64) {
        jlongArray arr = env->NewLongArray(64);
        env-> SetLongArrayRegion(arr,0,64, const_cast<const jlong*>(jKey));
        env-> CallVoidMethod(ctx,setKey,arr);
    }

    ///set R
    jlongArray arr = env->NewLongArray(2);
    env-> SetLongArrayRegion(arr,0,2, const_cast<const jlong*>(jR));
    env-> CallVoidMethod(ctx,setR,arr);

    ///set S
    arr = env->NewLongArray(16);
    env-> SetLongArrayRegion(arr,0,16, const_cast<const jlong*>(jS));
    env-> CallVoidMethod(ctx,setS,arr);

    ///set IV
    arr = env->NewLongArray(4);
    env-> SetLongArrayRegion(arr,0,4, const_cast<const jlong*>(jIV));
    env-> CallVoidMethod(ctx,setIv,arr);

    return ctx;
}extern "C"


JNIEXPORT jobject JNICALL
Java_ua_dm_myapplication_Strumok_get128Gamma(JNIEnv *env, jobject obj, jobject  ctx) {

    ///init class
    jclass systemParamClass = env -> FindClass("ua/dm/myapplication/SystemParam");

    ///init getters and setters
    jmethodID getR = env -> GetMethodID(systemParamClass, "getR","()[J");
    jmethodID setR = env -> GetMethodID(systemParamClass, "setR","([J)V");

    jmethodID getS = env -> GetMethodID(systemParamClass,"getS", "()[J");
    jmethodID setS = env -> GetMethodID(systemParamClass,"setS", "([J)V");

    jmethodID getGamma = env -> GetMethodID(systemParamClass,"getGamma", "()[J");
    jmethodID setGamma = env -> GetMethodID(systemParamClass,"setGamma", "([J)V");

    ///init jni variables
    jobject jObjectS =  env -> CallObjectMethod(ctx,getS);
    jobject jObjectR =  env -> CallObjectMethod(ctx,getR);
    jobject jObjectGamma =  env -> CallObjectMethod(ctx,getGamma);

    jlongArray tmpLongArray = static_cast<jlongArray>(jObjectR);
    jlong *jR = env -> GetLongArrayElements(tmpLongArray,0);

    tmpLongArray = static_cast<jlongArray>(jObjectS);
    jlong *jS = env -> GetLongArrayElements(tmpLongArray,0);

    tmpLongArray = static_cast<jlongArray>(jObjectGamma);
    jlong *jGamma = env -> GetLongArrayElements(tmpLongArray,0);


    ///init C variables
    uint64_t  S[16];
    uint64_t  r[2];
    uint64_t  out_stream[16];

    memcpy(r, jR, 16);
    memcpy(S, jS, 128);
    memcpy(out_stream, jGamma, 128);

    uint64_t fsmtmp;

    S[1] = a_mul(S[1]) ^ S[14] ^ ainv_mul(S[12]);
    fsmtmp = r[1] + S[14];
    r[1] = T(r[0]);
    r[0] = fsmtmp;
    out_stream[0] = ( r[0] +  S[1]) ^  r[1] ^  S[2];

    S[2] = a_mul( S[2]) ^  S[15] ^ ainv_mul( S[13]);
    fsmtmp =  r[1] +  S[15];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[1] = ( r[0] +  S[2]) ^  r[1] ^  S[3];

    S[3] = a_mul( S[3]) ^  S[0] ^ ainv_mul( S[14]);
    fsmtmp =  r[1] +  S[0];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[2] = ( r[0] +  S[3]) ^  r[1] ^  S[4];

    S[4] = a_mul( S[4]) ^  S[1] ^ ainv_mul( S[15]);
    fsmtmp =  r[1] +  S[1];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[3] = ( r[0] +  S[4]) ^  r[1] ^  S[5];

    S[5] = a_mul( S[5]) ^  S[2] ^ ainv_mul( S[0]);
    fsmtmp =  r[1] +  S[2];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[4] = ( r[0] +  S[5]) ^  r[1] ^  S[6];

    S[6] = a_mul( S[6]) ^  S[3] ^ ainv_mul( S[1]);
    fsmtmp =  r[1] +  S[3];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[5] = ( r[0] +  S[6]) ^  r[1] ^  S[7];

    S[7] = a_mul( S[7]) ^  S[4] ^ ainv_mul( S[2]);
    fsmtmp =  r[1] +  S[4];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[6] = ( r[0] +  S[7]) ^  r[1] ^  S[8];

    S[8] = a_mul( S[8]) ^  S[5] ^ ainv_mul( S[3]);
    fsmtmp =  r[1] +  S[5];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[7] = ( r[0] +  S[8]) ^  r[1] ^  S[9];

    S[9] = a_mul( S[9]) ^  S[6] ^ ainv_mul( S[4]);
    fsmtmp =  r[1] +  S[6];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[8] = ( r[0] +  S[9]) ^  r[1] ^  S[10];

    S[10] = a_mul( S[10]) ^  S[7] ^ ainv_mul( S[5]);
    fsmtmp =  r[1] +  S[7];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[9] = ( r[0] +  S[10]) ^  r[1] ^  S[11];

    S[11] = a_mul( S[11]) ^  S[8] ^ ainv_mul( S[6]);
    fsmtmp =  r[1] +  S[8];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[10] = ( r[0] +  S[11]) ^  r[1] ^  S[12];

    S[12] = a_mul( S[12]) ^  S[9] ^ ainv_mul( S[7]);
    fsmtmp =  r[1] +  S[9];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[11] = ( r[0] +  S[12]) ^  r[1] ^  S[13];

    S[13] = a_mul( S[13]) ^  S[10] ^ ainv_mul( S[8]);
    fsmtmp =  r[1] +  S[10];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[12] = ( r[0] +  S[13]) ^  r[1] ^  S[14];

    S[14] = a_mul( S[14]) ^  S[11] ^ ainv_mul( S[9]);
    fsmtmp =  r[1] +  S[11];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[13] = ( r[0] +  S[14]) ^  r[1] ^  S[15];

    S[15] = a_mul( S[15]) ^  S[12] ^ ainv_mul( S[10]);
    fsmtmp =  r[1] +  S[12];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[14] = ( r[0] +  S[15]) ^  r[1] ^  S[0];

    S[0] = a_mul( S[0]) ^  S[13] ^ ainv_mul( S[11]);
    fsmtmp =  r[1] +  S[13];
    r[1] = T( r[0]);
    r[0] = fsmtmp;
    out_stream[15] = ( r[0] +  S[0]) ^  r[1] ^  S[1];

    memcpy(jR,r,16);
    memcpy(jS,S,128);
    memcpy(jGamma,out_stream,128);

    jlongArray arr = env->NewLongArray(2);
    env-> SetLongArrayRegion(arr,0,2, const_cast<const jlong*>(jR));
    env-> CallVoidMethod(ctx,setR,arr);

    arr = env->NewLongArray(16);
    env-> SetLongArrayRegion(arr,0,16, const_cast<const jlong*>(jS));
    env-> CallVoidMethod(ctx,setS,arr);

    arr = env->NewLongArray(16);
    env-> SetLongArrayRegion(arr,0,16, const_cast<const jlong*>(jGamma));
    env-> CallVoidMethod(ctx,setGamma,arr);

    return ctx;

}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_ua_dm_myapplication_Strumok_cript(JNIEnv *env, jobject thiz, jlongArray buffer,
                              jint curent_size_of_buffer, jint buffer_size, jbyteArray text,
                              jint length_of_text, jint start_index) {

    uint64_t* ct = new uint64_t[length_of_text];
    memcpy(ct,text,length_of_text* sizeof(uint64_t));

    uint64_t* uBuffer = reinterpret_cast<uint64_t *>(buffer);
    memcpy(uBuffer,buffer,buffer_size * sizeof(uint64_t));

    if(curent_size_of_buffer> length_of_text){
        for (int i = 0; i < length_of_text - start_index; ++i) {
            ct[start_index + i] =ct[start_index + i]^ uBuffer[buffer_size - curent_size_of_buffer + i];
        }
    }else {
        for (int i = 0; i < curent_size_of_buffer; ++i) {
            ct[start_index + i] ^= uBuffer[buffer_size - curent_size_of_buffer + i];
        }
    }

    char *res = new char[length_of_text*4];

    for (int i = 0; i < length_of_text*4 ; i+=4) {
        res[i] =   ((char) ((ct[i] & 0xFF000000) >> 24));
        res[i+1] = ((char) ((ct[i] & 0x00FF0000) >> 16));
        res[i+2] = ((char) ((ct[i] & 0x0000FF00) >> 8));
        res[i+3] = ((char) ((ct[i] & 0X000000FF)));
    }

    jbyteArray arr = env->NewByteArray(length_of_text*4);
    env-> SetByteArrayRegion(arr, 0,length_of_text*4, reinterpret_cast<const jbyte *>(res));

    return arr;
}

