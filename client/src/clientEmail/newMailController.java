package clientEmail;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

public class newMailController{

    @FXML private TextField toTextField;
    @FXML private TextArea messageTextArea;

    private socketInfo sock;
    private final Stage stage;

    public newMailController(socketInfo socket) throws IOException {
        this.sock = socket;

        stage = new Stage();
        System.out.println("pierwszy");
        //Parent root = FXMLLoader.load(getClass().getResource("/clientEmail/newMailWindow.fxml"));
        FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("newMailWindow.fxml"));
        Parent root1 = (Parent) fxmlLoader.load();
        System.out.println("drugi");
        stage.setTitle("Send New E-Mail");
        stage.setScene(new Scene(root1, 600, 600));
        stage.setMinWidth(400);
        stage.setMinHeight(400);
        //primaryStage.setResizable(false);
        stage.show();
    }

    @FXML
    void sendNewMessage() {
        /*
        String to = toTextField.getText();
        String mess = messageTextArea.getText();
        to = to.trim();
        mess = mess.trim();

        String m = "send " + to.substring(0, to.indexOf('@'));
        m += " " + to.substring(to.indexOf('@')+1) + " " + mess + "\0";

        System.out.println(m);

        stage.close();
        */
    }


    //@Override
    //public void initialize(URL location, ResourceBundle resources) {
        //TODO
    //}
}
