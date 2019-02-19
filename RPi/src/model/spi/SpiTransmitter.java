package model.spi;

import java.io.IOException;

import javax.sound.midi.ShortMessage;

import com.pi4j.io.spi.*;

import model.event.SynthParameterEditEvent;
import model.event.SynthParameterEditListener;

/**
 * This class acts as a fast MIDI over SPI transmitter to any listening device, including the STM32.
 * @author sydxrey
 *
 */
public class SpiTransmitter implements SynthParameterEditListener<Object> {

	private SpiDevice spiDevice;
	
	/**
	 * Initialize the SPI bus with the correct speed and parameter, using Pi4J library.
	 * @throws IOException 
	 */
	public SpiTransmitter() throws IOException {
		super();
		spiDevice=SpiFactory.getInstance(SpiChannel.CS0, 500000);
		System.out.println("Opening SPI bus");
		
	}
	
	/**
	 * transmit the given MIDI message over the SPI bus
	 * @param sm
	 * @throws IOException 
	 */
	public void transmitMidiMessage(ShortMessage sm) throws IOException {
		
		spiDevice.write(sm.getMessage());
		
	}

	/**
	 * transmit the given MIDI message over the SPI bus, 3 bytes in a row
	 * @param sm
	 * @throws IOException 
	 */
	public void transmitMidiMessage(short status, short data1, short data2) throws IOException {
		
		spiDevice.write(status, data1, data2);
		
	}

	@Override
	public void synthParameterEdited(SynthParameterEditEvent<Object> e) {
		
				try {
			Object o = e.getValue();
			int parameterId = 0x00;
			int value = 0x00;
			if (o instanceof Boolean){
				boolean b = (Boolean)o;
				if (b) value = 0x7F;
				else value = 0x01;
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, parameterId, value);
				transmitMidiMessage(sm);
			}
			else if (o instanceof Double) {
				DoubleParameter param = (DoubleParameter)e.getSource();
				value = (int)(127.0 * param.getValueAsRatio());
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, parameterId, value);
				transmitMidiMessage(sm);
			}
			else if (o instanceof Enum) {
				EnumParameter enu = (EnumParameter)e.getSource();
				value = enu.getOrdinal();
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, parameterId, value);
				transmitMidiMessage(sm);
			}
			//Mettre le transmitLidiMessage dans les if : si on ne reconnait pas o on n'envoie pas de message
			//pour avoir le parameterId on peut utiliser le String lbl ou name (faire une méthode getParameter qui renvoie un entier en fonction du nom avec un switch case)


	public static void main(String[] args) throws Exception {
		SpiTransmitter st = new SpiTransmitter();
		int note = 80;
		int velocity = 100;
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
	
}
