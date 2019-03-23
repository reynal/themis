package application;

import model.midi.*;
import model.spi.*;

public class Main {

	public static final boolean RUN_ON_RASPBERRY = false; // enables SPI
	public static final boolean ACTIVATE_MIDI = false;
	public static final boolean DEBUG_MIDI = false;
	public static final boolean SIMULATOR = true; // real themis if false
	public static final boolean JAVAFX = false; // SWING if false
	

	public static void main(String[] args) throws Exception {

		SpiTransmitter spiTransmitter = null;
		if (RUN_ON_RASPBERRY) spiTransmitter = new SpiTransmitter();
		if (ACTIVATE_MIDI) new MidiInHandler(spiTransmitter); // TODO : add other listeners
		if (DEBUG_MIDI) new DumpReceiver(System.out);
		if (JAVAFX) javafx.application.Application.launch(JavaFXMain.class,args);
		else new SwingMain(); 
	}
		
}
