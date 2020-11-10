package model.serial;

import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.ShortMessage;

import com.pi4j.io.spi.SpiChannel;
import com.pi4j.io.spi.SpiDevice;
import com.pi4j.io.spi.SpiFactory;

/**
 * This class acts as a fast MIDI over SPI transmitter to any listening device, including the STM32.
 * @author sydxrey
 *
 */
public class SpiTransmitter extends AbstractSerialTransmitter {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private SpiDevice spiDevice;
	
	
	/**
	 * Initialize the SPI bus with the correct speed and parameter, using Pi4J library.
	 * @throws IOException 
	 */
	public SpiTransmitter() throws IOException {
		super();
		spiDevice=SpiFactory.getInstance(SpiChannel.CS0, 500000);
		LOGGER.info("Opening SPI bus");
	}
		
	/**
	 * transmit the given MIDI message over the SPI bus
	 */
	public void transmitMidiMessage(ShortMessage sm) throws IOException {
		
		spiDevice.write(sm.getMessage());
		
	}
	

	// ------------------------------------------------------------------------------------

	public static void main(String[] args) throws Exception {
		SpiTransmitter st = new SpiTransmitter();
		int note =36;
		int velocity = 126;
		ShortMessage smOn = new ShortMessage(ShortMessage.NOTE_ON, note, velocity);
		ShortMessage smOff = new ShortMessage(ShortMessage.NOTE_OFF, note, velocity);
		//while (true) {
			//st.transmitMidiMessage((short)0xA0,(short)0xA5,(short)0xAF);
			System.out.println("NOTE ON");
			st.transmitMidiMessage(smOn);
			Thread.sleep(1000);
			System.out.println("NOTE OFF");
			st.transmitMidiMessage(smOff);
			Thread.sleep(1000);
		//}
	}

	@Override
	public void close() {
		// TODO Auto-generated method stub
		
	}

	
}
