package model.serial;

import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.ShortMessage;

/**
 * A dummy implementation for debugging purpose 
 * @author reynal
 *
 */
public class DebugTransmitter extends AbstractSerialTransmitter {
	
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	public DebugTransmitter(){
		LOGGER.info("Creating DEBUG serial transmitter over System.out");
	}

	@Override
	public void transmitMidiMessage(ShortMessage sm) throws IOException {
		System.out.println("[DebugTransmitter] Transmitting message " + sm.getStatus() + " " + sm.getData1() + " " + sm.getData2());
		
	}

	@Override
	public void close() {
		//do nothing
	}

}
