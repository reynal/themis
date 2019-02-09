package application;

import java.util.ArrayList;
import java.util.List;

import javafx.application.Application;
import model.*;
import model.midi.*;
import model.spi.*;

public class Main {

	public static final boolean RUN_ON_RASPBERRY = false; // enables SPI
	public static final boolean ACTIVATE_MIDI = false;
	public static final boolean DEBUG_MIDI = false;
	public static final boolean SIMULATOR = true; // real themis if false
	public static final boolean JAVAFX = false; // SWING if alse

	public static void main(String[] args) throws Exception {

		SpiTransmitter spiTransmitter = null;
		if (RUN_ON_RASPBERRY) spiTransmitter = new SpiTransmitter();
		if (ACTIVATE_MIDI) new MidiInHandler(spiTransmitter); // TODO : add other listeners
		if (DEBUG_MIDI) new DumpReceiver(System.out);

		if (JAVAFX) Application.launch(JavaFXMain.class,args);
		else new SwingMain(); 
	}
	
	static List<AbstractModel> createModels(){

		List<AbstractModel> models = new ArrayList<AbstractModel>();
		models.add(new Vco3340());
		models.add(new Vco13700());
		//encoders.add(controlsGroup3,3,0);
		models.add(new MixerV2140D());
		models.add(new Vcf3320());
		models.add(new Vca());
		models.add(new ADSREnveloppe());
		return models;
	}
	
}
