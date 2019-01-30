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
import java.util.*;

import javafx.beans.value.*;
import controller.component.Control;
//import model.midi.*;
import model.spi.*;
/*
 SPIDevice pour faire sp�cif mat�riel (module de communication)
 Permet de communiquer entre Code Java et ce qui arrive/part sur le bus SPI
 */
	
public class Main extends Application {
	
	Stage window;
	private VcoCEM3340 vco3340;
	private VcoLM13700 vco13700;
	private VcoCEM3340 vcoDig;
	//private ChangeListener change;
	
	private GridPane controlsGroup1,controlsGroup2,controlsGroup3,controlsGroup4,controlsGroup5,controlsGroup6,controlsGroup7;
	private List<SynthParameter<?>> paramsVco3340,paramsVco13700,paramsVcoDig,paramsMixer,paramsVcf,paramsVca,paramsAdsr;
	private MixerV2140D mixer;
	private VcfCEM3320 vcf;
	private VcaLM13700 vca;
	private ADSREnveloppe adsr;
	
	private SpiTransmitter spi;
	//private DumpReceiver midiDump;
	//private MidiInHandler midiIn;
	
	
	
	private void createControlsGroup(GridPane group) {
		group.setStyle("-fx-background-color: black;"+"-fx-border-color: magenta;");
		group.setHgap(10);
		group.setVgap(10);
		group.setMinSize(250,200);
        //group.setMaxSize(250,200);
		group.setPadding(new Insets(0, 10, 0, 10));
	}
	
	private void fillControlsGroup(GridPane group,List<SynthParameter<?>> params ) {
		int i=0;
		for (SynthParameter<?> p : params) {
			i=i+1;
			//Label title = new Label(((SynthParameter<?>) params).getLabel()); //comment avoir un label pour la liste de param�tres
			//title.setStyle("-fx-text-fill: magenta;");
			Label label = new Label(p.getLabel());
			label.setStyle("-fx-text-fill: lightpink;");
			Control c = p.getControl();
			System.out.println(p);
			
			Node n = c.getJavaFXView();
			System.out.println(c.getJavaFXView());
			//group.add(title,2,0);
			group.add(n,i,1);
			//layout.add(lbl,i,1);
			group.add(label,i,2);
			
			if (n instanceof Slider) {
				Slider slider = (Slider) n;
				slider.valueProperty().addListener(new ChangeListener<Number>() {
		         @Override
		         public void changed(ObservableValue<? extends Number> observable, //
		               Number oldValue, Number newValue) {
		        	 	System.out.println("New Value " + newValue);
		            //infoLabel.setText("New value: " + newValue);
		         }
		      });
				}
			}
		}	
	
	
	
	public static void main(String[] args) {
		Application.launch(Main.class,args);
		
	}
	
	
	@Override
	public void start(Stage window) throws Exception {
		
		spi = new SpiTransmitter();
		//midiDump = new DumpReceiver(System.out);
		//midiIn = new MidiInHandler();
		
		
		controlsGroup1 = new GridPane();
		controlsGroup2 = new GridPane();
		controlsGroup3 = new GridPane();
		controlsGroup4 = new GridPane();
		controlsGroup5 = new GridPane();
		controlsGroup6 = new GridPane();
		controlsGroup7 = new GridPane();
		
        vco3340 = new VcoCEM3340();
		vco13700 = new VcoLM13700();
		vcoDig = new VcoCEM3340();
		mixer = new MixerV2140D();
		vcf = new VcfCEM3320();
		vca = new VcaLM13700();
		adsr = new ADSREnveloppe();
		
		

		paramsVco3340 = vco3340.getParameters();
		paramsVco13700 = vco13700.getParameters();
		paramsVcoDig = vcoDig.getParameters();
		paramsMixer = mixer.getParameters();
		paramsVcf = vcf.getParameters();
		paramsVca = vca.getParameters();
		paramsAdsr = adsr.getParameters();
		
		
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
        
        
        createControlsGroup(controlsGroup1);
        createControlsGroup(controlsGroup2);
        createControlsGroup(controlsGroup3);
        createControlsGroup(controlsGroup4);
        createControlsGroup(controlsGroup5);
        createControlsGroup(controlsGroup6);
        createControlsGroup(controlsGroup7);		
		
		fillControlsGroup(controlsGroup1,paramsVco3340);
		fillControlsGroup(controlsGroup2,paramsVco13700);
		fillControlsGroup(controlsGroup3,paramsVcoDig);
		fillControlsGroup(controlsGroup4,paramsMixer);
		fillControlsGroup(controlsGroup5,paramsVcf);
		fillControlsGroup(controlsGroup6,paramsVca);
		fillControlsGroup(controlsGroup7,paramsAdsr);
		

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
	    
	    //vco3340.getDetuneParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #1 needs update : " + e));
		//vco3340.getOctaveParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #2 needs update : " + e));
		//vco1.setDetune(0.03);
		//vco1.setOctave(Octave.TWO_INCHES);
	    
		encoders.add(controlsGroup1,0,0);
		encoders.add(controlsGroup2,1,0);
		encoders.add(controlsGroup3,3,0);
		encoders.add(controlsGroup4,0,1);
		encoders.add(controlsGroup5,1,1);
		encoders.add(controlsGroup6,2,1);
		encoders.add(controlsGroup7,2,0);
		
		//TODO gestion message erreur communication bus SPI (fen�tre ou pop up)
		
	    layout.setBottom(encoders);
	    layout.setLeft(screen);
	    layout.setRight(pads);
		//layout.getChildren().add(encoders);
		Scene s = new Scene(layout, 1600,910);
		window.setScene(s);
		window.setTitle("Themis");
		window.setResizable(true);
		window.show();
		
		//MidiInHandler.main(null);
	}
}
