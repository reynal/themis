package controller.component;

import java.io.IOException;
import java.util.*;
import java.util.logging.Logger;

import device.*;
import model.*;

/**
 * a class that can build controls around an MCP23017 device, that are
 * appropriate for controlling synth parameters.
 * 
 * TODO : create a method that can read an XML file and create an UI from it
 * 
 * @author reynal
 *
 */
public class ControlFactory {
	
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MCP23017 device;
	private HashSet<MCP23017.Pin> allocatedPins = new HashSet<MCP23017.Pin>(); // maintains a list of pins that have been allocated

	/**
	 * @param device a previously init'd MCP23017 device
	 */
	public ControlFactory(MCP23017 device) {
		
		this.device = device;
		if (device == null) LOGGER.warning("Creating a ControlFactory with no MCP23017 attached");
	}
			
	/**
	 * Creates a PushButton that will control the given SynthParameter through a change listener mechanism.
	 * @param synthParameter the parameter that listens to changes on this control
	 * @param pin the MCP23017 pin this button is connected to
	 * @return a control of type PushButton
	 * @throws IOException if there is an I/O error on the I2C bus
	 */
	public PushButton createControl(ModuleParameter<?> synthParameter, MCP23017.Pin pin) throws IOException {
		
		if (allocatedPins.add(pin) == false)
			throw new IOException("Pin " + pin + " of the MCP23017 device is already in use");

		PushButton button = new PushButton(synthParameter == null ? "Dummy" : synthParameter.getLabel(), device, pin);
		if (synthParameter != null) 
			button.addActionListener(synthParameter);
		if (synthParameter==null)
			LOGGER.info("Creating a dummy PushButton (no synthParam attached) on MCP230A7 GPIO pin " + pin);
		else
			LOGGER.info("Creating a PushButton for \"" + synthParameter + "\" on MCP230A7 GPIO pin " + pin);
		return button;
		
	}

	/**
	 * Creates a rotary encoder to control a MIDI parameter
	 * 
	 * @param pinA the encoder A channel
	 * @param pinB the encoder B channel
	 * @throws IOException if there is an I/O error on the I2C bus
	 * @return a control of type RotaryEncoder
	 */
	public RotaryEncoder createControl(ModuleParameter<?> synthParam, MCP23017.Pin pinA, MCP23017.Pin pinB) throws IOException  {
		
		if (allocatedPins.add(pinA) == false)
			throw new IOException("Pin " + pinA + " of the MCP23017 device is already in use");
		if (allocatedPins.add(pinB) == false)
			throw new IOException("Pin " + pinB + " of the MCP23017 device is already in use");

		// push button -> parameter
		RotaryEncoder encoder = new RotaryEncoder(synthParam == null ? "Dummy" : synthParam.getLabel(), device, pinA, pinB);
		if (synthParam != null) 
			encoder.addChangeListener(synthParam);
		if (synthParam==null) 
			LOGGER.info("Creating a dummy RotaryEncoder (no synthparam attached) on MCP230A7 GPIO pins " + pinA + " and " + pinB);
		else
			LOGGER.info("Creating a RotaryEncoder for \"" + synthParam + "\" on MCP230A7 GPIO pins " + pinA + " and " + pinB);
		return encoder;
	}	
	
	// ---------- test ---------
	
	public static void main(String[] args) throws IOException {
		
		// check if hashmap works:
		ControlFactory cf = new ControlFactory(null);
		BooleanParameter bp = new BooleanParameter("parameter on/off");
		cf.createControl(bp, MCP23017.Pin.P0A);
		cf.createControl(bp, MCP23017.Pin.P0A);
		
	}
	
}
