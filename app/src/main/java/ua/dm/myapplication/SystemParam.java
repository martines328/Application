package ua.dm.myapplication;

/**
 * Created by destr on 23.03.2020.
 */
class SystemParam {
    /*-------------------------------*/
    private long[] S;
    private long[] r;
    private long[] key;
    private long[] iv;
    private int keySize;

    public long[] getGamma() {
        return gamma;
    }

    public void setGamma(long[] gamma) {
        this.gamma = gamma;
    }

    private long[] gamma;

    public SystemParam(long[] key, long[] iv, int keySize) {
        this.S = new long[]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        this.r = new long[]{0,0};
        this.gamma = new long[]{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        this.key = key;
        this.iv = iv;
        this.keySize = keySize;
    }

    public long[] getS() {
        return S;
    }

    public void setS(long []s) {
        this.S = s;

    }

    public long[] getR() {
        return r;
    }

    public void setR(long[] r) {
        this.r = r;
    }

    public long[] getKey() {
        return key;
    }

    public void setKey(long[] key) {
        this.key = key;
    }

    public long[] getIv() {
        return iv;
    }

    public void setIv(long[] iv) {
        this.iv = iv;
    }

    public int getKeySize() {
        return keySize;
    }

    public void setKeySize(int keySize) {
        this.keySize = keySize;
    }
}

