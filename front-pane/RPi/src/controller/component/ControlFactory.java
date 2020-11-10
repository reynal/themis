package controller.component;

import java.io.IOException;
import java.util.*;
import java.util.logging.Logger;

import com.pi4j.io.gpio.RaspiPin;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import device.*;
import device.MCP23017.DeviceAddress;
import device.MCP23017.Port;
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
	
	public static void main(String[] args) throws Exception {
		
		//testHashset();
		testWithHW();
		
	}
	
	private static void testHashset() throws IOException {
		
		// check if hashmap works:
		ControlFactory cf = new ControlFactory(null);
		BooleanParameter bp = new BooleanParameter("parameter on/off");
		cf.createControl(bp, MCP23017.Pin.P0A);
		cf.createControl(bp, MCP23017.Pin.P0A);

	}
	
	private static void testWithHW() throws Exception {
		
		MCP23017 device = new MCP23017(DeviceAddress.ADR_000, RaspiPin.GPIO_04);
		//MCP23017 device = new MCP23017(DeviceAddress.ADR_001, RaspiPin.GPIO_05);

		device.registerRpiPinForReset(MCP23017.DEFAULT_RST_PIN); // pin 37
		device.reset();
		device.printRegisters();
		
		device.enableIntPinsMirror();
		device.setInput(Port.A);
		device.setInput(Port.B);
		device.setPullupResistors(Port.A, true);
		device.setPullupResistors(Port.B, true);
		device.setInterruptOnChange(Port.A, true);
		device.setInterruptOnChange(Port.B, true);
		device.addInterruptListener(e -> System.out.println(e));
		device.clearInterrupts(Port.A);
		device.clearInterrupts(Port.B);
		device.printRegisters();
		
		ControlFactory cf = new ControlFactory(device);
		Vco3340AModule vco3340 = new Vco3340AModule();
		cf.createControl(vco3340.getDetuneParameter(), MCP23017.Pin.P1B, MCP23017.Pin.P2B);

		int i=0;
		while ((i++)<120) {
			//System.out.printf("INTFA: %02X \t GPIOA: %02X\n", device.readInterruptFlagRegister(Port.A), device.read(Port.A)); //, device.read(Port.B));
			//System.out.printf("A: %02X \t B: %02X\n", device.read(Port.A), device.read(Port.B));
			//System.out.print(i+" ");
			System.out.print(".");
			//device.printRegistersBriefA();
			//device.printRegistersBriefB();
			Thread.sleep(1000);
		}
		System.out.println("closing device");
		device.close();		
	}
	
}
