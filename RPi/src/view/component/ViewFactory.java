package view.component;

import java.io.IOException;
import java.util.HashSet;
import java.util.logging.Logger;

import com.pi4j.io.gpio.RaspiPin;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import controller.component.RotaryEncoder;
import device.IS31FL3731;
import device.MCP23017;
import device.MCP23017.DeviceAddress;
import device.MCP23017.Port;
import model.EnumParameter;
import model.MIDIParameter;
import model.ModuleParameter;
import model.Octave;
import model.Vco3340AModule;

/**
 * A factory that can build a view (a bargraph, a group of leds, etc) for a synth parameter 
 * with all LEDs being controlled by an IS31FL3731 led driver.
 * 
 * TODO : possible bug with allocatedLedsA/B (same led appears multiple times)
 * 
 * @author reynal
 *
 */
public class ViewFactory {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private IS31FL3731 device;
	private HashSet<IS31FL3731.LEDCoordinate> allocatedLedsA = new HashSet<IS31FL3731.LEDCoordinate>(); // IS31FL3731 first matrix (=72 leds)
	private HashSet<IS31FL3731.LEDCoordinate> allocatedLedsB = new HashSet<IS31FL3731.LEDCoordinate>(); // second matrix (72 leds)
		
	/**
	 * Construct a new factory for views based on the given IS31FL3731 device.
	 */
	public ViewFactory(IS31FL3731 device) {		
		this.device = device;		
		if (device == null) LOGGER.warning("Creating a ViewFactory with no IS31FL3731 attached");
	}
	
	
	/**
	 * Creates a view based on a single LED.
	 * 
	 * @param ledCoordinate the LED (connected to the IS31FL3731 deviceà that implements this view
	 */
	public LED createView(ModuleParameter<?> param, IS31FL3731.LEDCoordinate ledCoordinate) throws IOException{
		
		LOGGER.info("ViewFactory: creating LED for SynthParameter \"" + param + "\" at " + ledCoordinate);
		HashSet<IS31FL3731.LEDCoordinate> allocatedLeds = (ledCoordinate.AorB == IS31FL3731.Matrix.A ? allocatedLedsA : allocatedLedsB);
		
		if (allocatedLeds.add(ledCoordinate) == false)
			throw new IOException("[SingleLED] " + ledCoordinate + " of the IS31FL3731 device is already in use");
		
		LED led = new LED(this.device, ledCoordinate);
		if (param != null) param.addChangeListener(led);
		
		return led;
	}

	/**
	 * Creates a BarGraph of 8 leds in the same row. Might apply preferably to MIDIParameter, but also works
	 * with EnumParameter and BooleanParameter (only pb is this will waste some leds).
	 * 
	 * @param synthParameter the corresponding MIDI parameter whose value is displayed by this BarGraph 
	 * @param row a row of 8 leds, see IS31FL3731 datasheet 
	 * @param AorB matrix A or B of the IS31FL3731 device
	 */
	public BarGraph createView(ModuleParameter<?> synthParameter, IS31FL3731.Matrix AorB, int row) throws IOException{
		
		LOGGER.info("ViewFactory: creating an 8-led BarGraph row=" + row + " & matrix=" + AorB + " for \"" + synthParameter + "\"");
		
		HashSet<IS31FL3731.LEDCoordinate> allocatedLeds = (AorB == IS31FL3731.Matrix.A ? allocatedLedsA : allocatedLedsB);
		
		// check use of pins:
		IS31FL3731.LEDCoordinate ledCoordinate; 
		for (int col=0; col < 8; col++) {
			ledCoordinate = new IS31FL3731.LEDCoordinate(row, col, AorB);
			if (allocatedLeds.add(ledCoordinate) == false)
				throw new IOException("[BarGraph] " + ledCoordinate + " of the IS31FL3731 device is already in use");
		}

		// parameter -> bar graph
		BarGraph barGraph = new BarGraph(device, AorB, row);
		if (synthParameter != null) synthParameter.addChangeListener(barGraph);
		return barGraph;
	}
	
	/**
	 * Creates a view comprised of a group of LEDs for the given EnumParameter
	 * 
	 * @param row row in matrix A or B, see IS31FL3731 datasheet 
	 */
	public BarGraph createView(ModuleParameter<?> synthParameter, IS31FL3731.Matrix AorB, int row, int colStart) throws IOException{
		
		// if synthParameter is null, this is probably a dummy bargraph for debugging purpose (i.e. one that does displays nothing), 
		// and we arbitrarily allocate a single led:
		int ledCount = (synthParameter == null ? 1 : synthParameter.getValuesCount());
		
		// compute the rightmost led position and check if it's within the permitted bounds (there are 8 led per row in a IS31FL3731 matrix):
		int colEnd = colStart + ledCount - 1;
		if (colEnd >= 8) throw new IllegalArgumentException("colEnd must be lower than 8 : " + colEnd);
		
		LOGGER.info("ViewFactory: creating a group of leds row=" + row + " & col=" + colStart + "->" + colEnd + " & matrix=" + AorB + " for \"" + synthParameter + "\"");
		
		
		HashSet<IS31FL3731.LEDCoordinate> allocatedLeds = (AorB == IS31FL3731.Matrix.A ? allocatedLedsA : allocatedLedsB);
		
		
		IS31FL3731.LEDCoordinate ledCoordinate; 
		for (int col=colStart; col <= colEnd; col++) {
			ledCoordinate = new IS31FL3731.LEDCoordinate(row, col, AorB); 
			if (allocatedLeds.add(ledCoordinate) == false)
				throw new IOException("[LEDGroup] " + ledCoordinate + " of the IS31FL3731 device is already in use");
		}
		
		BarGraph ledGroup = new BarGraph(device, AorB, row, colStart, ledCount);
		if (synthParameter != null) synthParameter.addChangeListener(ledGroup);
		return ledGroup;
	}


	@Override
	public String toString() {
		return super.toString() + " IS31FL3731 allocated pins=" + allocatedLedsA + " & " + allocatedLedsB;
	}

	// ---------- test ---------
	
	public static void main(String[] args) throws Exception {
		
		testWithHW();
	}
	
	private static void testHashset() throws IOException {
		
		// check if hashmap works:
		ViewFactory vf = new ViewFactory(null);
		//MIDIParameter p = new MIDIParameter("param MIDI");
		EnumParameter<Octave> p = new EnumParameter<Octave>(Octave.class, "octave");
		vf.createView(p, IS31FL3731.Matrix.A, 6, 0);
		//vf.createView(p, IS31FL3731.Matrix.A, 0, 4);
		System.out.println(vf);
		
	}
	
	private static void testWithHW() throws Exception {
		
		IS31FL3731 is31fl3731 = new IS31FL3731();
		ViewFactory vf = new ViewFactory(is31fl3731);
		
		Vco3340AModule vco3340 = new Vco3340AModule();		
		
		MIDIParameter p = vco3340.getDutyParameter();
		
		BarGraph bar = new BarGraph(is31fl3731, IS31FL3731.Matrix.A, 0); // row=0
		p.addChangeListener(bar);
		
		//MCP23017 device = new MCP23017(DeviceAddress.ADR_000, RaspiPin.GPIO_04);
		MCP23017 device = new MCP23017(DeviceAddress.ADR_001, RaspiPin.GPIO_05);
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
		//device.addInterruptListener(e -> System.out.println(e));
		device.clearInterrupts(Port.A);
		device.clearInterrupts(Port.B);
		device.printRegisters();
		
		RotaryEncoder encoder = new RotaryEncoder("Test encoder", device, MCP23017.Pin.P7B, MCP23017.Pin.P5B);
		encoder.addChangeListener(p);
		
		
		int i=0;
		while (i < 100) {
			//System.out.print(".");
			i = p.getValue();
			Thread.sleep(1000);
			device.clearInterrupts(Port.A);
			device.clearInterrupts(Port.B);
		}
		
		device.close();
		System.out.println("MCP closed");
	}
		
}
