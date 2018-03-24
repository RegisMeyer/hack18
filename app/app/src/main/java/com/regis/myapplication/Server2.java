import java.net.ServerSocket;
import java.net.Socket;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;

import java.util.Hashtable;

public class Server2 {

    @SuppressWarnings("resource")
    public static void main (String[] args) throws IOException {

        ServerSocket server = null;
        try {
            server = new ServerSocket(4000);
        } catch (IOException e) {
            System.err.println("Could not start up on: " + "4000" + "Maby server is already open? Or a portforwording messup?");
            System.err.println(e);
            System.exit(1);
        }

        Socket client = null;
        while(true) {
            try {
                client = server.accept();
                System.out.print("connected");
            } catch (IOException e) {
                System.err.println("Accept failed.");
                System.err.println(e);
            }

            Thread t = new Thread(new ClientConn(client));
            t.start();
        }
    }
}


class ClientConn implements Runnable {
    private Socket client;
    ObjectInputStream ois;
    ObjectOutputStream oos;

    ClientConn(Socket client) {
        this.client = client;
        try {
            ois = new ObjectInputStream(client.getInputStream());
            String Recv;
            Recv = ois.readLine();

            oos = new ObjectOutputStream(client.getOutputStream());
            oos.writeChars("Welcome");
            oos.flush();

            oos.close();

        } catch (IOException e) {
            System.err.println(e);
            return;
        }
    }

    public void run() {
        String msg, response;
        try {
            System.out.print("checking");
            while ((msg = ois.readLine()) != null) {
                System.out.print(msg);
            }
        } catch (IOException e) {
            System.err.println(e);
        }
    }

    public void sendMsg(String msg) throws IOException {
        oos.writeChars(msg);
    }
}