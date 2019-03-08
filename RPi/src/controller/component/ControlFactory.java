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
	private HashSet<MCP23017.Pin> usedPinsPortA = new HashSet<MCP23017.Pin>();
	private HashSet<MCP23017.Pin> usedPinsPortB = new HashSet<MCP23017.Pin>();

	public ControlFactory(MCP23017 device) {
		
		this.device = device;
	}
			
	public PushButton createControl(SynthParameter<?> parameter, MCP23017.Port portAorB, MCP23017.Pin pin) {
		
		HashSet<MCP23017.Pin> usedPins = (portAorB == MCP23017.Port.A ? usedPinsPortA : usedPinsPortB); 
		if (usedPins.add(pin) == false)
			throw new IllegalArgumentException("Pin " + pin + " of the MCP23017 device is already in use");

		PushButton b = new PushButton(parameter == null ? "Dummy" : parameter.getLabel(), device, portAorB, pin);
		if (parameter != null) b.addPushButtonActionListener(parameter);		
		return b;
		
	}

	/**
	 * Creates a rotary encoder to control a MIDI parameter
	 * 
	 * @param pinA the encoder A channel
	 * @param pinB the encoder B channel
	 */
	public RotaryEncoder createControl(SynthParameter<?> param, MCP23017.Port portAorB, MCP23017.Pin pinA, MCP23017.Pin pinB)  {
		
		HashSet<MCP23017.Pin> usedPins = (portAorB == MCP23017.Port.A ? usedPinsPortA : usedPinsPortB);
		if (usedPins.add(pinA) == false)
			throw new IllegalArgumentException("Pin " + pinA + " of the MCP23017 device is already in use");
		if (usedPins.add(pinB) == false)
			throw new IllegalArgumentException("Pin " + pinB + " of the MCP23017 device is already in use");

		// push button -> parameter
		try {
			RotaryEncoder re = new RotaryEncoder(param == null ? "Dummy" : param.getLabel(), device, portAorB, pinA, pinB);
			if (param != null) re.addRotaryEncoderChangeListener(param);
			return re;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}	
}
