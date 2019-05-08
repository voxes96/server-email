package clientEmail;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;


public class Main extends Application {

    @Override
    public void start(Stage primaryStage) throws IOException{

        Parent root = FXMLLoader.load(getClass().getResource("loginWindow.fxml"));
        primaryStage.setTitle("E-Mail");
        primaryStage.setScene(new Scene(root, 600, 400));
        primaryStage.setMinWidth(440);
        primaryStage.setMinHeight(320);
        //primaryStage.setResizable(false);
        primaryStage.show();

    }


    public static void main(String[] args) {
        Application.launch(args);
    }

}
