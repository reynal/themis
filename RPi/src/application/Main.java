package application;

import model.midi.*;
import model.spi.*;
import javax.sound.midi.ShortMessage; 

public class Main {

	public static final boolean RUN_ON_RASPBERRY = false; // enables SPI
	public static final boolean ACTIVATE_MIDI = true;
	public static final boolean DEBUG_MIDI = false;
	public static final boolean SIMULATOR = true; // real themis if false
	public static final boolean JAVAFX = false; // SWING if alse

	public static void main(String[] args) throws Exception {

		SpiTransmitter spiTransmitter = null;
		if (RUN_ON_RASPBERRY) {
			spiTransmitter = new SpiTransmitter();
			/*ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, 0x0, 0x0);
			spiTransmitter.transmitMidiMessage(sm);
			while (true){
				spiTransmitter.transmitMidiMessage(sm);
			}*/
		}
		if (ACTIVATE_MIDI) new MidiInHandler(spiTransmitter); // TODO : add other listeners
		if (DEBUG_MIDI) new DumpReceiver(System.out);

		if (JAVAFX) javafx.application.Application.launch(JavaFXMain.class,args);
		else new SwingMain(spiTransmitter); 
	}
		
}
