package clientEmail;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;

public class clientController implements Initializable {

    private socketInfo sock;
    private String str;
    private char[] largebuff = new char[40960];
    private char[] buff = new char[5];
    private int le;
    private EmailMessages emMess;

    @FXML private Label usernameLabel;
    @FXML private TextArea emailMessageTextArea;
    @FXML private ListView<String> messagesListView;


    void initData(socketInfo s) {
        this.sock = s;
        emMess = new EmailMessages();

        usernameLabel.setText("Witaj " + sock.user + "     ");

        getMessagesButtonClicked();
    }

    private void downloadMessages() {
        emMess = new EmailMessages();

        try {
            //overwrite();
            sock.writer.print("getmess\0");
            sock.writer.flush();

            /*largebuff = new char[40960];
            while(true) {
                for (int i=0 ; i<40960 ; i++) {

                }
            }*/

            /*
            //String alles = "";
            int dl = 0;
            while ((le = sock.reader.read(largebuff)) > 1000) {        //////////////zmienic sposob reada na read(buff,0,40960)192.168.100.100
                str = String.valueOf(largebuff);
                //str = str.trim();
                largebuff = new char[40960];

                dl += le;
                while (dl < 40960) {
                    le = sock.reader.read(largebuff);
                    str += String.valueOf(largebuff);
                    largebuff = new char[40960];
                    dl += le;
                }
                //dl = 0;
                str = str.trim();

                System.out.println("mes getmess(): " + str.replaceAll("\n"," ").substring(0,10) + " length: " + dl);
                dl=0;
                emMess.add(str);

                //alles = alles + str  + "\n\n";
            }*/
            //System.out.println("calosc\n" + alles);
            while (true) {
                int c;
                char cc;
                String sss = "";
                c = sock.reader.read();
                cc = (char)c;
                sss += cc;
                //System.out.println(cc);
                if (String.valueOf(sss).equals("i")) {
                    for (int i = 1; i < 40960; i++) {
                        c = sock.reader.read();
                        cc = (char)c;
                        //System.out.println(cc);
                        sss =  sss + cc;
                    }
                    sss = sss.trim();
                    emMess.add(sss);
                } else {
                    for (int i = 1; i < 5; i++) {
                        c = sock.reader.read();
                        cc = (char)c;
                        sss = sss + cc;
                    }
                    break;
                }
            }


            //le = sock.reader.read(buff);
            //str = String.valueOf(largebuff);
            //str = str.trim();
            //largebuff = new char[40960];
            //System.out.println("mes getmess: " + str + " length: " + le);


        } catch (IOException e) {
            System.out.println("Error with read from server");
            e.printStackTrace();
        }
    }

    private void showMessagesShortFormat() {
        messagesListView.getItems().clear();
        String s;
        for(int i = 0 ; i<emMess.size ; i++) {
            s = emMess.messages.get(i);
            s = s.replaceAll("\n", " ");
            if(s.length() > 60) { s = s.substring(0, 60) + "..."; }
            messagesListView.getItems().add(emMess.persons.get(i) + "      " + emMess.dates.get(i) + "\n" + s);
        }
    }

    @FXML
    void logoutButtonClicked(ActionEvent event) throws IOException {
        //overwrite();
        sock.writer.print("logout\0");
        sock.writer.flush();

        try {
            le = sock.reader.read(buff);
            str = String.valueOf(buff);
            str = str.trim();
            buff = new char[5];
            System.out.println("mes logout: " + str + " length: " + le);
            sock.socket.close();
        } catch (IOException e) {
            System.out.println("Problem with user logout");
            e.printStackTrace();
        }


        Stage stage = (Stage) ((Node)event.getSource()).getScene().getWindow();
        stage.setMinWidth(440);
        stage.setMinHeight(320);
        Parent loginParent = FXMLLoader.load(getClass().getResource("loginWindow.fxml"));
        Scene loginScene = new Scene(loginParent, 600, 400);
        stage.setScene(loginScene);
        stage.show();
    }

    @FXML
    void newMessageButtonClicked() {
        Stage windowNewMail = new Stage();
        windowNewMail.initModality(Modality.APPLICATION_MODAL);
        windowNewMail.setTitle("Write new message");
        windowNewMail.setMinWidth(500);
        windowNewMail.setMinHeight(600);

        BorderPane newMessBorderPane = new BorderPane();

        Label label1 = new Label("To:   ");
        TextField toTextField = new TextField();
        toTextField.setPrefWidth(400);
        HBox hbox = new HBox();
        hbox.getChildren().addAll(label1, toTextField);

        TextArea messageTextArea = new TextArea();

        Button send = new Button("Send");
        send.setOnAction(event -> {
            String to = toTextField.getText();
            String mess = messageTextArea.getText();
            to = to.trim();
            mess = mess.trim();

            String m = "";
            if (to.indexOf('@') > 0) { m = "send " + to.substring(0, to.indexOf('@')); }
            else { m = "send " + to; }

            m = m + " " + to.substring(to.indexOf('@')+1) + " " + mess + "\0";
            System.out.println(m);

            //overwrite();
            sock.writer.print(m);
            sock.writer.flush();

            try {
                le = sock.reader.read(buff);
                str = String.valueOf(buff);
                str = str.trim();
                buff = new char[5];
                System.out.println("mes send: " + str + " length: " + le);
            } catch(IOException e) {
                e.printStackTrace();
            }

            BorderPane info = new BorderPane();
            Label inf = new Label();
            if (str.equals("1") || str.equals("-999")) { inf.setText("Message has been sent."); }
            else { inf.setText("Cannot send the message."); }
            //Button but = new Button("Close");
            info.setCenter(inf);
            windowNewMail.setScene(new Scene(info, 400, 200));
            windowNewMail.setMinWidth(400);
            windowNewMail.setMinHeight(200);
            windowNewMail.setMaxWidth(400);
            windowNewMail.setMaxHeight(200);
            windowNewMail.setResizable(false);

            //System.out.println("Wyslano.");
            //windowNewMail.close();
        });

        newMessBorderPane.setTop(hbox);
        newMessBorderPane.setCenter(messageTextArea);
        newMessBorderPane.setBottom(send);

        newMessBorderPane.setPadding(new Insets(25, 15, 15, 15));
        BorderPane.setMargin(newMessBorderPane, new Insets(10));
        BorderPane.setMargin(hbox, new Insets(0, 10, 15, 0));
        BorderPane.setMargin(send, new Insets(15, 0, 0, 0));
        BorderPane.setAlignment(hbox, Pos.CENTER_LEFT);
        BorderPane.setAlignment(send, Pos.CENTER);

        Scene sceneNewMail = new Scene(newMessBorderPane);
        windowNewMail.setScene(sceneNewMail);
        windowNewMail.showAndWait();
    }

    @FXML
    void getMessagesButtonClicked() {
        downloadMessages();
        showMessagesShortFormat();
    }

    @FXML
    void messagesListViewSelected() {
        int idx = messagesListView.getSelectionModel().getSelectedIndex();
        //System.out.println(idx);
        if (idx >= 0) {
            emailMessageTextArea.setText("User: " + emMess.persons.get(idx) + "\nDate: " + emMess.dates.get(idx) + "\n\nMessage:\n" + emMess.messages.get(idx));
        }
    }

    @FXML
    void deleteMessageButtonClicked() {
        int idx = messagesListView.getSelectionModel().getSelectedIndex();
        if (idx >= 0) {
            idx = emMess.ids.get(idx);
            System.out.println(idx);

            String s = "remove " + idx + "\0";

            //overwrite();
            sock.writer.print(s);
            sock.writer.flush();

            try {
                le = sock.reader.read(buff);
                str = String.valueOf(buff);
                str = str.trim();
                buff = new char[5];
            } catch (IOException e) {
                System.out.println("Cannot remove message.");
                e.printStackTrace();
            }

            System.out.println("mes remove: " + str + " length: " + le);

            getMessagesButtonClicked();
        }
    }

    String czytaj() throws IOException{
        int c;
        String str = "";
        c = sock.reader.read();
        for(int i=0 ; i<40960 ; i++) {
            c = sock.reader.read();
            str = str + String.valueOf(c);
        }
        return str.trim();
    }

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        //TODO
    }


    private class EmailMessages {
        Integer size;
        List<Integer> ids;
        List<String> persons;
        List<String> dates;
        //List<String> titles;
        List<String> messages;

        EmailMessages() {
            size = 0;
            ids = new ArrayList<>();
            persons = new ArrayList<>();
            dates = new ArrayList<>();
            //titles = new ArrayList<>();
            messages = new ArrayList<>();
        }

        private void add(String str) {
            if ( str.length() > 20) {
                String temp;

                str = str.substring(4);
                //String s = str.substring(0,str.indexOf("\n"));
                ids.add(Integer.parseInt(str.substring(0, str.indexOf("\n"))));
                str = str.substring(str.indexOf("\n") + 1);

                str = str.substring(7);
                temp = str.substring(0, str.indexOf("\n"));
                str = str.substring(str.indexOf("\n") + 1);
                str = str.substring(8);
                //l = l + "@" + str.substring(0,str.indexOf("\n"));
                persons.add(temp + "@" + str.substring(0, str.indexOf("\n")));
                str = str.substring(str.indexOf("\n") + 1);

                str = str.substring(6);
                temp = str.substring(0, str.indexOf("\n"));
                temp = temp.replaceFirst(" ", "-");
                temp = temp.replaceFirst(" ", "-");
                dates.add(temp);
                str = str.substring(str.indexOf("\n") + 1);

                //String m = str.substring(str.indexOf("\n")+1);
                messages.add(str.substring(str.indexOf("\n") + 1));

                size++;
            }
        }
        /*id: 0
        login: lukasz
        domain: wp.pl
        data: 28 12 2018 20:56
        message:
        wiadomosc do niego*/
    }
}
