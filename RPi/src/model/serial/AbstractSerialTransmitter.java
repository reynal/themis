package model.serial;

import java.io.IOException;
import java.util.HashMap;
import java.util.logging.Logger;

import javax.sound.midi.ShortMessage;

import application.ModuleFactory;
import model.BooleanParameter;
import model.EnumParameter;
import model.MIDIParameter;
import model.ModuleParameter;
import model.event.ModuleParameterChangeEvent;
import model.event.ModuleParameterChangeListener;

/**
 * 
 * @author sydxrey
 *
 */
public abstract class AbstractSerialTransmitter implements ModuleParameterChangeListener {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	public AbstractSerialTransmitter() {
		super();
	}

	/**
	 * transmit the given MIDI message over the serial bus
	 * @param sm message
	 * @throws IOException 
	 */
	public abstract void transmitMidiMessage(ShortMessage sm) throws IOException ;
	
	
	public abstract void close();
	

	


	@Override
	public void moduleParameterChanged(ModuleParameterChangeEvent e) {
		
		try {
			Object source = e.getSource();
			//System.out.println("SynthParameterEditEvent's source is a " + o);
			
			if (source instanceof BooleanParameter){
				
				BooleanParameter synthParam = (BooleanParameter)source;
				int value = synthParam.getValueAsMIDICode();
				int midiCcId = ModuleFactory.getDefault().getMidiCC(synthParam);
				if (midiCcId == -1) {
					LOGGER.warning("ModuleParameter" + source + " has no associated MIDI CC => can't send anything to the serial bus");
					return;
				}
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, midiCcId, value);
				transmitMidiMessage(sm);
				LOGGER.info("sending BooleanParameter: msg=" + ShortMessage.CONTROL_CHANGE+" " + midiCcId + " " + value);
			}
			
			else if (source instanceof MIDIParameter) {
				
				MIDIParameter p = (MIDIParameter)source;
				int value = p.getValueAsMIDICode();
				int midiCcId = ModuleFactory.getDefault().getMidiCC(p);
				if (midiCcId == -1) {
					LOGGER.warning("ModuleParameter" + source + " has no associated MIDI CC => can't send anything to the serial bus");
					return;
				}
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, midiCcId, value);
				transmitMidiMessage(sm);
				LOGGER.info("sending MIDIParameter: msg=" + ShortMessage.CONTROL_CHANGE+" " + midiCcId + " " + value);
			}
			
			else if (source instanceof EnumParameter) {
				
				EnumParameter<?> p = (EnumParameter<?>)source;
				int value = p.getValueAsMIDICode();
				int midiCCId = ModuleFactory.getDefault().getMidiCC(p);
				if (midiCCId == -1) {
					LOGGER.warning("ModuleParameter" + source + " has no associated MIDI CC => can't send anything to the serial bus");
					return;
				}
				ShortMessage sm = new ShortMessage(ShortMessage.CONTROL_CHANGE, midiCCId, value);
				transmitMidiMessage(sm);
				LOGGER.info("send EnumParameter : msg=" + ShortMessage.CONTROL_CHANGE + " " + midiCCId + " " + value);
			}
			else {
				LOGGER.warning("nothing sent \n");
			}
			
		} catch (Exception ee){
			ee.printStackTrace();
			LOGGER.warning("really nothing send \n");
		}
	}
}
