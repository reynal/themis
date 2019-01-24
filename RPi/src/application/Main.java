package application;
	
import javafx.application.Application;
import javafx.event.*;
import javafx.scene.*;
import view.*;
import model.*;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
//import javafx.geometry.Pos;
//import javafx.application.Preloader;
import javafx.stage.Stage;
//import javafx.stage.Window;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import java.util.*;
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
		
		BorderPane layout = new BorderPane();
		layout.setStyle("-fx-background-color: #222;");
        layout.setPadding(new Insets(10));
        
        GridPane encoders = new GridPane();
        //encoders.setStyle("-fx-background-color: black;");
        encoders.setHgap(10);
        encoders.setVgap(10);
        
        GridPane pads = new GridPane();
        pads.setPadding(new Insets(80));
        pads.setHgap(10);
        pads.setVgap(10);
        
        AnchorPane screen = new AnchorPane();
        AnchorPane.setLeftAnchor(screen, 410.0);
        AnchorPane.setLeftAnchor(screen, 230.0);
        
        ImageView iv1 = new ImageView(new Image("file:src/resources/img/logo.png"));
        iv1.setStyle("-fx-border-color : grey");
        screen.getChildren().add(iv1);
        
        GridPane buttonGroup1 = new GridPane();
        buttonGroup1.setStyle("-fx-background-color: black;"
        		+"-fx-border-color: magenta;");
        buttonGroup1.setHgap(10);
        buttonGroup1.setVgap(10);
        buttonGroup1.setMinSize(250,200);
        buttonGroup1.setMaxSize(250,200);
        buttonGroup1.setPadding(new Insets(0, 10, 0, 10));
        
        GridPane buttonGroup2 = new GridPane();
        buttonGroup2.setStyle("-fx-background-color: black;"
        		+"-fx-border-color: magenta;");
        buttonGroup2.setHgap(10);
        buttonGroup2.setVgap(10);
        buttonGroup2.setMinSize(250,200);
        buttonGroup2.setMaxSize(250,200);
        buttonGroup2.setPadding(new Insets(0, 10, 0, 10));
        
        GridPane buttonGroup3 = new GridPane();
        buttonGroup3.setStyle("-fx-background-color: black;"
        		+"-fx-border-color: cyan;");
        buttonGroup3.setHgap(10);
        buttonGroup3.setVgap(10);
        buttonGroup3.setMinSize(250,200);
        buttonGroup3.setMaxSize(250,200);
        buttonGroup3.setPadding(new Insets(0, 10, 0, 10));
        
        GridPane buttonGroup4 = new GridPane();
        buttonGroup4.setStyle("-fx-background-color: black;"
        		+"-fx-border-color: yellow;");
        buttonGroup4.setHgap(10);
        buttonGroup4.setVgap(10);
        buttonGroup4.setMinSize(250,200);
        buttonGroup4.setMaxSize(250,200);
        buttonGroup4.setPadding(new Insets(0, 10, 0, 10));
        
        GridPane buttonGroup5 = new GridPane();
        buttonGroup5.setStyle("-fx-background-color: black;"
        		+"-fx-border-color: yellow;");
        buttonGroup5.setHgap(10);
        buttonGroup5.setVgap(10);
        buttonGroup5.setMinSize(250,200);
        buttonGroup5.setMaxSize(250,200);
        buttonGroup5.setPadding(new Insets(0, 10, 0, 10));
        
        GridPane buttonGroup6 = new GridPane();
        buttonGroup6.setStyle("-fx-background-color: black;"
        		+"-fx-border-color: magenta;");
        buttonGroup6.setHgap(10);
        buttonGroup6.setVgap(10);
        buttonGroup6.setMinSize(250,200);
        buttonGroup6.setMaxSize(250,200);
        buttonGroup6.setPadding(new Insets(0, 10, 0, 10));
 
		VcoCEM3340 vco1 = new VcoCEM3340();
		VcoLM13700 vco2 = new VcoLM13700();
		MixerV2140D mixer = new MixerV2140D();
		VcfCEM3320 vcf = new VcfCEM3320();
		VcaLM13700 vca = new VcaLM13700();
		ADSREnveloppe adsr = new ADSREnveloppe();
		List<SynthParameter<?>> paramsVCO1 = vco1.getParameters();
		List<SynthParameter<?>> paramsVCO2 = vco2.getParameters();
		List<SynthParameter<?>> paramsMixer = mixer.getParameters();
		List<SynthParameter<?>> paramsVcf = vcf.getParameters();
		List<SynthParameter<?>> paramsVca = vca.getParameters();
		List<SynthParameter<?>> paramsAdsr = adsr.getParameters();
		
		int i=0;
		for (SynthParameter<?> p : paramsVCO1) {
			i=i+1;
			Label title = new Label("VCO1");
			title.setStyle("-fx-text-fill: magenta;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightpink;");
			Control c = p.getControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			buttonGroup1.add(title,2,0);
			buttonGroup1.add(n,i,1);
			//layout.add(lbl,i,1);
			buttonGroup1.add(label,i,2);
		}
		i=0;
		for (SynthParameter<?> p : paramsVCO2) {
			i=i+1;
			Label title = new Label("VCO2");
			title.setStyle("-fx-text-fill: magenta;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightpink;");
			Control c = p.getControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			buttonGroup2.add(title,2,0);
			buttonGroup2.add(n,i,1);
			//layout.add(lbl,i,1);
			buttonGroup2.add(label,i,2);
		}
		i=0;
		for (SynthParameter<?> p : paramsMixer) {
			i=i+1;
			Label title = new Label("MIXER");
			title.setStyle("-fx-text-fill: cyan;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightblue;");
			Control c = p.getControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			buttonGroup3.add(title,2,0);
			buttonGroup3.add(n,i,1);
			//layout.add(lbl,i,1);
			buttonGroup3.add(label,i,2);
		}
		i=0;
		for (SynthParameter<?> p : paramsVcf) {
			i=i+1;
			Label title = new Label("VCF");
			title.setStyle("-fx-text-fill: yellow;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightyellow;");
			Control c = p.getControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			buttonGroup4.add(title,2,0);
			buttonGroup4.add(n,i,1);
			//layout.add(lbl,i,1);
			buttonGroup4.add(label,i,2);
		}
		i=0;
		for (SynthParameter<?> p : paramsVca) {
			i=i+1;
			Label title = new Label("VCA");
			title.setStyle("-fx-text-fill: yellow;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightyellow;");
			Control c = p.getControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			buttonGroup5.add(title,2,0);
			buttonGroup5.add(n,i,1);
			//layout.add(lbl,i,1);
			buttonGroup5.add(label,i,2);
		}
		i=0;
		for (SynthParameter<?> p : paramsAdsr) {
			i=i+1;
			Label title = new Label("ADSR");
			title.setStyle("-fx-text-fill: magenta;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightpink;");
			Control c = p.getControl();
			System.out.println(p);
			Node n = c.getJavaFXView();
			buttonGroup6.add(title,2,0);
			buttonGroup6.add(n,i,1);
			//layout.add(lbl,i,1);
			buttonGroup6.add(label,i,2);
		}		
		i=0;
	    for(int x = 0; x <8; x++){
	    	for(int y = 0; y <4; y++){
	           	i=i+1;
	           	Button butt = new Button();
	           	butt.setMinSize(70.0,70.0);
	           	butt.setStyle("-fx-background-color : white;");
	           	pads.add(butt,x,y);
	        }
	    }
	    
		encoders.add(buttonGroup1,0,0);
		encoders.add(buttonGroup2,1,0);
		encoders.add(buttonGroup3,2,0);
		encoders.add(buttonGroup4,0,1);
		encoders.add(buttonGroup5,1,1);
		encoders.add(buttonGroup6,2,1);

	    layout.setBottom(encoders);
	    layout.setLeft(screen);
	    layout.setRight(pads);
		//layout.getChildren().add(encoders);
		Scene s = new Scene(layout, 1600,910);
		window.setScene(s);
		window.setTitle("Pulsar");
		window.setResizable(true);
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
