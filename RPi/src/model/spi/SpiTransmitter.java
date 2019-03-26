package model.spi;

import java.io.IOException;
import java.util.*;
import javax.sound.midi.ShortMessage;

import com.pi4j.io.spi.*;

import model.event.*;
import model.*;

/**
 * This class acts as a fast MIDI over SPI transmitter to any listening device, including the STM32.
 * @author sydxrey
 *
 */
public class SpiTransmitter implements SynthParameterEditListener {

	private SpiDevice spiDevice;
	private HashMap<SynthParameter, Integer> parameterIdHashMap = new HashMap<SynthParameter, Integer>();
	
	
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
	
	/**
	 * Init the MIDI parameter ID table (used by SPI transmitter when writing to the STM32)
	 * @param vco3340
	 * @param vco13700
	 */
	public void initParameterIdHashMap(Vco3340 vco3340, Vco13700 vco13700){
		
		int i=0;
		parameterIdHashMap.put(vco3340.getDetuneParameter(), i++);
		parameterIdHashMap.put(vco3340.getOctaveParameter(), i++);
		parameterIdHashMap.put(vco3340.getSyncFrom13700Parameter(), i++);
		parameterIdHashMap.put(vco3340.getWaveShapeParameter(), i++);
		System.out.println("initParameterIdHashMap ok \n");
		
	}
	

	@Override
	public void synthParameterEdited(SynthParameterEditEvent e) {
		
		try {
			Object o =e.getSource();
			//System.out.println("SynthParameterEditEvent's source is a " + o);
			
			if (o instanceof BooleanParameter){
				BooleanParameter p = (BooleanParameter)o;
				int value = p.getValueAsMIDICode();
				int parameterId = parameterIdHashMap.get(p);
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, parameterId, value);
				transmitMidiMessage(sm);
				System.out.println("send BooleanParameter : msg="+ShortMessage.CONTROL_CHANGE+" - " + parameterId + " - " + value);
				//send minimal value (o) if false and maximal value (127) if true
			}
			else if (o instanceof MIDIParameter) {
				MIDIParameter p = (MIDIParameter)o;
				int value = p.getValueAsMIDICode();
				int parameterId = parameterIdHashMap.get(p);
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, parameterId, value);
				transmitMidiMessage(sm);
				System.out.println("send MIDIParameter : msg="+ShortMessage.CONTROL_CHANGE+" - " + parameterId + " - " + value);
				//getValueAsMIDICode return a number between 0 and 127 
			}
			else if (o instanceof EnumParameter) {
				EnumParameter p = (EnumParameter)o;
				int value = p.getValueAsMIDICode();
				int parameterId = parameterIdHashMap.get(p);
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, parameterId, value);
				transmitMidiMessage(sm);
				System.out.println("send EnumParameter : msg="+ShortMessage.CONTROL_CHANGE+" - " + parameterId + " - " + value);
				//send the position of the enum in the enum table	
			}
			else {System.out.println("nothing sent \n");}
			
		} catch (Exception imde){
			imde.printStackTrace();
			System.out.println("really nothing send \n");
		}
		
	}
	

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
	
}
