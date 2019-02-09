package application;
	
import javafx.application.Application;
import javafx.scene.*;
import model.*;
import javafx.geometry.*;
import javafx.stage.*;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;

/**
 * SR -> BF :  Documentation a ecrire !
 */	
public class JavaFXMain extends Application {
	
		
	@Override
	public void start(Stage windows) throws Exception {
				
		windows.setTitle("Themis");
		windows.setX(0);
		windows.setY(0);
		windows.setResizable(false);

		if (Main.SIMULATOR) windows.setScene(createSimulator());
		else windows.setScene(createRaspberryTouchscreen());        
		
				
		//TODO gestion message erreur communication bus SPI (fenetre ou pop up)	    
		
		windows.show();
		
	}
	
	
	// real themis
	private Scene createRaspberryTouchscreen(){

		BorderPane touchScreenLayout = new BorderPane();
		touchScreenLayout.setStyle("-fx-background-color: #000;");
		touchScreenLayout.setPadding(new Insets(10));
	    //touchScreenLayout.getChildren().add(createSimulatorEncoders());
		return new Scene(touchScreenLayout,800,480);
	}
	
	// simulator
	private Scene createSimulator(){

		BorderPane simulatorLayout = new BorderPane();
		simulatorLayout.setStyle("-fx-background-color: #222;");
		simulatorLayout.setPadding(new Insets(10));
		/*simulatorStage.setScene(simulator);
		simulatorStage.setTitle("Themis Simulator");
		simulatorStage.setResizable(true);*/
	    simulatorLayout.setBottom(createSimulatorEncoders());
	    simulatorLayout.setLeft(createSimulatorTouchscreen());
	    simulatorLayout.setRight(createSimulatorPads());
		return new Scene(simulatorLayout,1600,910);
		
	}
	
	private AnchorPane createSimulatorTouchscreen(){
        
        AnchorPane screen = new AnchorPane();
        //AnchorPane.setLeftAnchor(screen, 410.0);
        //AnchorPane.setLeftAnchor(screen, 230.0);
        
        // 	TODO : il faut pas encoder les resources comme ça, il faut utiliser getResource()
        ImageView iv1 = new ImageView(new Image("file:src/resources/img/logo.png")); 
        iv1.setStyle("-fx-border-color : grey");
        screen.getChildren().add(iv1);
        
        return screen;
	}
	
	private GridPane createSimulatorEncoders(){
		
        GridPane encoders = new GridPane();
        //encoders.setStyle("-fx-background-color: black;");
        encoders.setHgap(10);
        encoders.setVgap(10);
        int x=0,y=0;
        for (AbstractModel m : Main.createModels()){
        	encoders.add(m.createSimulatorJavaFXControlGroup(), x%3, y%3);
        	x++;
        	if (x==3) {y++; x=0;}
        	
        }
		return encoders;
	}
	
	private GridPane createSimulatorPads(){
		
        GridPane pads = new GridPane();
        pads.setPadding(new Insets(80));
        pads.setHgap(10);
        pads.setVgap(10);

        // pads:
		int i=0;
	    for(int x = 0; x <8; x++){
	    	for(int y = 0; y <4; y++){
	           	i=i+1;
	           	Button butt = new Button();
	           	butt.setMinSize(70.0,70.0);
	           	butt.setStyle("-fx-background-color : white;");
	           	pads.add(butt,x,y);
	        }
	    }
	    
	    return pads;
	}



}