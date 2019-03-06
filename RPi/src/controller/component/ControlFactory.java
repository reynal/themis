package controller.component;

import java.io.IOException;
import java.util.*;
import device.*;
import model.*;

/**
 * a class that can build controls around an MCP23017 device appropriate to control synth parameters.
 * TODO : create a method that can read an XML file and create an UI from it
 * 
 * @author reynal
 *
 */
public class ControlFactory {
	
	private MCP23017 device;
	private HashSet<MCP23017.Pin> usedPins = new HashSet<MCP23017.Pin>();

	public ControlFactory(MCP23017 device) {
		
		this.device = device;
	}
			
	public PushButton createControl(SynthParameter<?> parameter, MCP23017.Pin pin) {
		
		if (parameter instanceof BooleanParameter) return createControl((BooleanParameter)parameter, pin);
		else if (parameter instanceof MIDIParameter) return createControl((MIDIParameter)parameter, pin);
		else return null;
	}
	
	/**
	 * @param pin the MCP23017 pin the control is connected to
	 * @return
	 */	
	public PushButton createControl(BooleanParameter parameter, MCP23017.Pin pin) {
		
		if (usedPins.add(pin) == false)
			throw new IllegalArgumentException("Pin " + pin + " of the MCP23017 device is already in use");
		PushButton b = new PushButton(parameter.getLabel(), device, pin);
		b.addPushButtonActionListener(parameter);		
		return b;
		
	}
	
	
	/**
	 * Creates a push button to control an enum param
	 * 
	 * @param pin the pin this button is connected to
	 */
	public PushButton createControl(EnumParameter<?> param, MCP23017.Pin pin)  {
		
		if (usedPins.add(pin) == false)
			throw new IllegalArgumentException("Pin " + pin + " of the MCP23017 device is already in use");

		// push button -> parameter
		PushButton  pb = new PushButton(param.getLabel(), device, pin);
		pb.addPushButtonActionListener(param);
		return pb;
	}	

	/**
	 * Creates a rotary encoder to control a MIDI parameter
	 * 
	 * @param pinA the encoder A channel
	 * @param pinB the encoder B channel
	 */
	public RotaryEncoder createControl(MIDIParameter param, MCP23017.Pin pinA, MCP23017.Pin pinB)  {
		
		if (usedPins.add(pinA) == false)
			throw new IllegalArgumentException("Pin " + pinA + " of the MCP23017 device is already in use");
		if (usedPins.add(pinB) == false)
			throw new IllegalArgumentException("Pin " + pinB + " of the MCP23017 device is already in use");

		// push button -> parameter
		try {
			RotaryEncoder re = new RotaryEncoder(param.getLabel(), device, pinA, pinB);
			re.addRotaryEncoderChangeListener(param);
			return re;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}	
}
