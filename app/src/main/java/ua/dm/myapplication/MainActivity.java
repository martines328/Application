package ua.dm.myapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private static long[] key;
    private static long[] iv;

    private static void init(){
        key = new long[8];
        key[7] = 0x8000000000000000L;
        key[6] = 0x0000000000000000L;
        key[5] = 0x0000000000000000L;
        key[4] = 0x0000000000000000L;
        key[3] = 0x0000000000000000L;
        key[2] = 0x0000000000000000L;
        key[1] = 0x0000000000000000L;
        key[0] = 0x0000000000000000L;

        iv = new long[4];

        iv[0]=1;
        iv[1]=2;
        iv[2]=3;
        iv[3]=4;
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        init();
        SystemParam sp = new SystemParam(key,iv,64);
        Strumok s = new Strumok(sp);

        String ct = s.encript("test");
        SystemParam sp1 = new SystemParam(key,iv,64);
        Strumok s1 = new Strumok(sp1);
        String t = s1.decript(ct);


        TextView tv = findViewById(R.id.sample_text);
        tv.setText(t);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
}
