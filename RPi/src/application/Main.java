package application;
	
import javafx.application.Application;
import javafx.event.*;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
//import javafx.application.Preloader;
import javafx.stage.Stage;
import javafx.stage.Window;
import javafx.scene.*;
import javafx.scene.control.*;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
//import javafx.scene.shape.*;
import javafx.scene.text.*;
import javafx.scene.image.*;
import javafx.scene.input.*;
import view.*;
import model.*;
import java.util.*;

import controller.*;
import controller.component.Control;

/*
 SPIDevice pour faire sp�cif mat�riel (module de communication)
 Permet de communiquer entre Code Java et ce qui arrive/part sur le bus SPI
 */
	
public class Main extends Application {
	
	Stage window;
	
	public static void main(String[] args) {
		Application.launch(Main.class,args);
		
	}

	@Override
	public void start(Stage window) throws Exception {
	
        GridPane layout = new GridPane();
        layout.setHgap(10);
        layout.setVgap(10);
        layout.setPadding(new Insets(0, 10, 0, 10));

        // VCO -> parameters 
		VcoCEM3340 vco1 = new VcoCEM3340();
		VcoLM13700 vco2 = new VcoLM13700();
		MixerV2140D mixer = new MixerV2140D();
		List<SynthParameter<?>> paramsVCO1 = vco1.getParameters();
		List<SynthParameter<?>> paramsVCO2 = vco2.getParameters();
		List<SynthParameter<?>> paramsMixer = mixer.getParameters();
		
		/*int[] index = {0,1,2};
		for (int i : index)*/
		/*
		String lab = params.get(0).getLabel();
		System.out.println(lab);
		Control c = params.get(0).getControl();
		System.out.print(c);
		
		String lab1 = params.get(1).getLabel();
		System.out.println(lab1);
		Control c1 = params.get(1).getControl();
		System.out.print(c1);
		
		String lab2 = params.get(2).getLabel();
		System.out.println(lab2);
		Control c2 = params.get(2).getControl();
		System.out.print(c2);*/
		int i=0;
		for (SynthParameter<?> p : paramsVCO1) {
			i=i+1;
			//String lbl = p.getLabel();
			Label label = new Label(p.getLabel());
			Control c = p.createControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			layout.add(n,i,0);
			//layout.add(lbl,i,1);
			layout.add(label,i,1);
		}
		i=0;
		for (SynthParameter<?> p : paramsVCO2) {
			i=i+1;
			//String lbl = p.getLabel();
			Label label = new Label(p.getLabel());
			Control c = p.createControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			layout.add(n,i,2);
			//layout.add(lbl,i,1);
			layout.add(label,i,3);
		}
		i=0;
		for (SynthParameter<?> p : paramsMixer) {
			i=i+1;
			//String lbl = p.getLabel();
			Label label = new Label(p.getLabel());
			Control c = p.createControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			layout.add(n,4+i,0);
			//layout.add(lbl,i,1);
			layout.add(label,4+i,0);
		}
	
		Scene s = new Scene(layout, 1280,720);
		window.setScene(s);
		window.setTitle("Pulsar");
		window.setResizable(false);
		window.show();
	
	}
	/*
	//@Override
	public void startTmp(Stage window) throws Exception {
		
		//Node n = PerformancePad.getView();
		//Node n1 = PushButton();
		int widthPad=4;
		int lenghtPad=4;
		int widthControls=5;
		int lenghtControls=6;
		
		List<String> controlList = Arrays.asList( new String[]{"LFO 2nd","Rate","WaveForm","Delay","Sync","VCO 2nd","Octave",
		"Detune","WaveForm","Other","Env 2nd","Attack","Decay","Release","Sustain","VCF 2nd","Cutoff","Resonance","Drive",
		"Type","VCA 2nd","Keyboard","Env. Amount","Gain","Other","Mix 2nd","Osc1","Osc2","Osc3","Osc4"});
		System.out.println(controlList);
		AnchorPane pad = new AnchorPane();
		Group screen = new Group();
		
		GridPane layout = new GridPane();
		
		GridPane grid = new GridPane();
		grid.setPadding(new Insets(80));
		grid.setHgap(10);
        grid.setVgap(10);
        grid.setAlignment(Pos.TOP_RIGHT);
                
	    Button[][] matrix = new Button[widthPad][lenghtPad];
	    
	    //HBox shift = new HBox();
	    GridPane controls = new GridPane();
	    controls.setPadding(new Insets(10));
	    controls.setHgap(50);
	    controls.setVgap(8);
	    controls.setAlignment(Pos.CENTER);
        
	    Button[][] knobs = new Button[widthControls][lenghtControls];// allocates the size of the matrix
	    
	    Image img = new Image("file:src/resources/img/logo.png");
	    Image img2 = new Image("file:src/resources/img/filter1.png");
	    Image img3 = new Image("file:src/resources/img/env1.png");
		ImageView imgView = new ImageView();
		imgView.setImage(img);
		screen.getChildren().add(imgView);
		((Node) imgView).addEventHandler(MouseEvent.MOUSE_CLICKED, new EventHandler<MouseEvent>() {
		     @Override
		     public void handle(MouseEvent event) {
		         System.out.println("Screen touched ");
		         event.consume();
		     }
		     /*
		     public void handle(DragEvent event) {
		    	 System.out.println("Screen drag");
		    	 imgView.setImage(img2);
		    	 event.consume();
		     }*/
		    /* 
		});
		     
	    // runs a for loop and an embedded for loop to create buttons to fill the size of the matrix
	    // these buttons are then added to the matrix
		
	    int i=0;
	    for(int x = 0; x <lenghtPad; x++){
	            for(int y = 0; y <widthPad; y++){
	            	i=i+1;
	            	matrix[x][y] = new Button(); //creates new random binary button  
	                matrix[x][y].setText("Pad"+i);   // Sets the text inside the matrix
	                matrix[x][y].setMaxSize(70,70);
	                matrix[x][y].setMinSize(70,70);
	                matrix[x][y].setOnAction(new EventHandler<ActionEvent>() {
	                    public void handle(ActionEvent event) {
	                        System.out.println("Pad pushed");
	                        /*imgView.setImage(img);
	                		screen.getChildren().add(imgView);*/
	                    /*}
	                });
	                grid.add(matrix[x][y], y, x);
	            }
	    } 
	    i=0;
	    for(int l = 0; l <lenghtControls; l++){
	            for(int k = 0; k <widthControls; k++){
	            	i=i+1;
	            	knobs[k][l] = new Button(); //creates new random binary button  
	            	knobs[k][l] .setMaxSize(90,40);
	            	knobs[k][l] .setMinSize(90,40);
	                knobs[k][l].setText(controlList.get(i-1));   // Sets the text inside the matrix
	                knobs[k][l].setOnAction(new EventHandler<ActionEvent>() {
	                    public void handle(ActionEvent event) {
	                        System.out.println("Control moved");
	                        /*imgView.setImage(img2);
	                		screen.getChildren().add(imgView);*/
	                   	/*}
	                });
	                controls.add(knobs[k][l], l, k);
	            }
	    }
	    
	    /*
	    for(int l=0; l <lenghtControls; l++){		
	    	if(l==3) {
	            for(int k = 0; k <widthControls; k++){
	            	knobs[k][l].setOnAction(new EventHandler<ActionEvent>() {
	            	public void handle(ActionEvent event) {
                        System.out.println("Control moved");
                        imgView.setImage(img2);
                		screen.getChildren().add(imgView);
                   	}
                });
	            }
	        }
	    	if(l==2) {
	            for(int k = 0; k <widthControls; k++){
	            	knobs[k][l].setOnAction(new EventHandler<ActionEvent>() {
	            	public void handle(ActionEvent event) {
                        System.out.println("Control moved");
                        imgView.setImage(img3);
                		screen.getChildren().add(imgView);
                   	}
                });
	            }
	        }
	    	if(l==5) {
	            for(int k = 0; k <widthControls; k++){
	            	knobs[k][l].setOnAction(new EventHandler<ActionEvent>() {
	            	public void handle(ActionEvent event) {
                        System.out.println("Control moved");
                        imgView.setImage(img);
                		screen.getChildren().add(imgView);
                   	}
                });
	            }
	        }
	    }*/
	    
		/*screen.setOnAction(new EventHandler<ActionEvent>() {
            public void handle(ActionEvent event) {
                System.out.println("Pad pushed");
            }
        });*/
	    /*
		pad.getChildren().add(grid);
		//AnchorPane.setRightAnchor(grid, 10d);
        //AnchorPane.setBottomAnchor(grid, 10d);
        layout.add(screen,0,0);
        layout.add(pad,1,0);
        layout.add(controls, 0,1);
		//layout.getChildren().addAll(screen,pad,controls);
		Scene s = new Scene(layout, 1280,720);
		window.setScene(s);
		window.setTitle("Pulsar");
		window.setResizable(false);
		window.show();
		
	}*/
}

