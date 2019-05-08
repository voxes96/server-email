package clientEmail;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.TextField;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

import java.io.*;
import java.net.Socket;
import java.net.URL;
import java.util.ResourceBundle;

public class loginController implements Initializable {

    private String str;
    private char[] buff = new char[5];
    private int le;

    @FXML public TextField usernameTextField;
    @FXML public PasswordField passwordPasswordField;
    @FXML public TextField ipAddressTextField;
    @FXML public TextField portTextField;
    @FXML public Button loginButton;
    @FXML public Label errorLabel;

    /**
     * funkcja pozwalajaca sie zalogowac
     */
    @FXML
    void loginButtonClicked(ActionEvent event) {

        try {
            Socket clientSocket = new Socket( ipAddressTextField.getText(), Integer.parseInt(portTextField.getText()) );

            //System.out.println("Connect to sign");

            BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);

            String msg = "sign " + usernameTextField.getCharacters() + " " + passwordPasswordField.getCharacters() + "\0";

            writer.print(msg);
            writer.flush();

            le = reader.read(buff);
            str = String.valueOf(buff);
            str = str.trim();
            buff = new char[5];
            System.out.println("mes login: " + str + " length: " + le);

            if (str.equals("1")) {
                try {
                    socketInfo sock = new socketInfo(clientSocket, reader, writer, usernameTextField.getCharacters().toString());

                    FXMLLoader loader = new FXMLLoader();
                    loader.setLocation(getClass().getResource("clientWindow.fxml"));
                    Parent clientParent = loader.load();

                    Scene clientScene = new Scene(clientParent, 650, 700);

                    clientController controller = loader.getController();
                    controller.initData(sock);

                    Stage stage = (Stage) ((Node)event.getSource()).getScene().getWindow();
                    stage.setScene(clientScene);
                    stage.setMinWidth(650);
                    stage.setMinHeight(650);
                    stage.show();

                } catch (Exception e) {
                    errorLabel.setTextFill(Color.RED);
                    errorLabel.setText("Undefined error.");
                    System.out.println("Cannot load fxml file");
                    e.printStackTrace();
                }
            } else if (str.equals("-11") || str.equals("-20")) {
                errorLabel.setTextFill(Color.RED);
                errorLabel.setText("Wrong username or password.");
                System.out.println("Wrong username or password.");
            } else {
                System.out.println("Undefined error.");
            }
        } catch (IOException e) {
            errorLabel.setTextFill(Color.RED);
            errorLabel.setText("Cannot connect to the server.");
            System.out.println("Cannot connect to the server.");
            //e.printStackTrace();
        }
    }

    /**
     * funkcja tworzaca nowego uzytkownika
     */
    @FXML
    void createButtonClicked() {
        try {
            Socket clientSocket = new Socket( ipAddressTextField.getText(), Integer.parseInt(portTextField.getText()) );
            System.out.println("Connect to create");

            BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);

            String msg = "create " + usernameTextField.getCharacters() + " " + passwordPasswordField.getCharacters() + "\0";
            writer.print(msg);
            writer.flush();

            le = reader.read(buff);
            str = String.valueOf(buff);
            str = str.trim();
            buff = new char[5];
            System.out.println("mes create: " + str + " length: " + le);
            if (str.equals("1")) {
                errorLabel.setTextFill(Color.GREEN);
                errorLabel.setText("Create user successfully.");
            } else if (str.equals("-10")) {
                errorLabel.setTextFill(Color.RED);
                errorLabel.setText("That user already exists.");
            }
        } catch (IOException e) {
            errorLabel.setTextFill(Color.RED);
            errorLabel.setText("Cannot connect to the server.");
            System.out.println("Cannot connect to the server.");
            //e.printStackTrace();
        }
    }


    @Override
    public void initialize(URL location, ResourceBundle resources) {
        //TODO
    }
}
