package ua.dm.myapplication;

import android.util.Log;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

/**
 * Created by destr on 23.03.2020.
 */
public class Strumok {
    private int currentSize;
    private int bufferSize;
    private long[] buffer;
    private SystemParam ctx;

    static {
        System.loadLibrary("native-lib");
    }

    public Strumok(SystemParam ctx){
        currentSize = 0;
        bufferSize = 16;
        buffer = new long[bufferSize];
        this.ctx = init(ctx);
    }

    private native SystemParam init(SystemParam ctx);

    public SystemParam getCtx(){
        return ctx;
    }

    private native SystemParam get128Gamma(SystemParam ctx);

    private native byte[] cript(long[] buffer,int curentSizeOfBuffer,int bufferSize, byte[] text,int lengthOfText,int startIndex);

    private long[] getAllGamma(int size) {

        for (int i = 0; i < size; i += 16) {
            long[] msTmp = get128Gamma(ctx).getGamma();
            System.arraycopy(msTmp, 0, buffer, i, 16);
        }

        return buffer;
    }

    public String encript(String text){

        return chipher(text);
    }

    public String decript(String text){
        return chipher(text);
    }

   private String chipher(String text){
        String criptedText ="";

        if(text.length() % 4 !=0 )
            for (int i =0; i< text.length()%4; i++)
                text+=" ";
        byte[] bt = text.getBytes();

        int textLength = bt.length/4;

        if(textLength > currentSize) {
            if (currentSize == 0) {
                currentSize = bufferSize;
                buffer = getAllGamma(bufferSize);
            }

            if (textLength > currentSize){
                criptedText= new String(cript(buffer,currentSize,bufferSize,bt,textLength,0),StandardCharsets.UTF_8);
                int skip = currentSize;
                if(textLength / 16
                        != 0 ) {
                    bufferSize = (textLength / 16 + 1) * 16;
                }

                buffer = getAllGamma(bufferSize);
                currentSize = bufferSize;
                criptedText = new String(cript(buffer,currentSize,bufferSize,bt,textLength,skip),StandardCharsets.UTF_8);
            }else if(textLength< currentSize){

                criptedText =new String( cript(buffer,currentSize,bufferSize,bt,textLength,0),StandardCharsets.UTF_8);
                currentSize = currentSize - textLength;
            }else {
                criptedText = new String(cript(buffer,currentSize,bufferSize,bt,textLength,0),StandardCharsets.UTF_8);
                currentSize =0;
            }
            if (currentSize == 0){
                buffer = getAllGamma(bufferSize);
                currentSize = bufferSize;
            }



        }



        return criptedText;
    }

}
