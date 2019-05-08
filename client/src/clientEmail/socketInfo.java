package clientEmail;

import java.io.BufferedReader;
import java.io.PrintWriter;
import java.net.Socket;

class socketInfo {

    Socket socket;
    BufferedReader reader;
    PrintWriter writer;
    String user;
    //String host;
    //int port;

    /*socketInfo(Socket sock, BufferedReader r, PrintWriter w, String h, int p) {
        this.socket = sock;
        this.reader = r;
        this.writer = w;
        this.host = h;
        this.port = p;
    }*/

    socketInfo(Socket sock, BufferedReader r, PrintWriter w, String u) {
        this.socket = sock;
        this.reader = r;
        this.writer = w;
        this.user = u;
    }

    Socket getSocket() {return this.socket;}

    BufferedReader getReader() {return this.reader;}

    PrintWriter getWriter() {return this.writer;}

    String getUser() {return this.user;}

    //String getHost() {return this.host;}

    //int getPort() {return this.port;}
}
