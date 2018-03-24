package com.regis.myapplication;

import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.view.MenuItem;
import android.widget.TextView;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;

import android.os.Bundle;
import android.app.Activity;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;




public class MainActivity extends AppCompatActivity {

    Thread m_objThreadClient;
    Socket clientSocket;
    TextView serverMessage;
    EditText clientMessage;
    String sIn, sOut;
    ObjectOutputStream oos;
    ObjectInputStream ois;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        serverMessage = (TextView) findViewById(R.id.textView1);
        clientMessage = (EditText) findViewById(R.id.editText1);

        try {
            StartTh1();
        }catch(IOException ex) {
            // Hello
        }

        Server2 client = new Server2();


        /*try {
            client.main();
        }catch(IOException ex) {
            // Hello
        }*/
    }


    public void StartTh1() throws IOException{
        final String Text;
        String send = "  ";
        Text = clientMessage.getText().toString();
        m_objThreadClient = new Thread( new Runnable(){
            public void run()
            {
                try {
                    clientSocket = new Socket("10.0.2.2", 4000);
                    oos = new ObjectOutputStream (clientSocket.getOutputStream());
                    oos.writeObject(Text);
                    oos.flush();

                } catch (IOException e) {
                    serverMessage.setText(e.getMessage());
                }

            }
        });
        serverMessage.setText(send);

        m_objThreadClient.start();

    }

}

