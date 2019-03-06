package view.component;

import java.io.IOException;
import java.util.HashSet;
import device.IS31FL3731;
import device.MCP23017;
import model.BooleanParameter;
import model.EnumParameter;
import model.MIDIParameter;

/**
 * A factory that can build a view (a bargraph, a group of leds, etc) for a synth parameter 
 * with all LEDs being controlled by an IS31FL3731 led driver.
 * 
 * @author reynal
 *
 */
public class ViewFactory {

	private IS31FL3731 device;
	private HashSet<IS31FL3731.LEDCoordinate> usedLeds = new HashSet<IS31FL3731.LEDCoordinate>();
		
	/**
	 * @param device
	 */
	public ViewFactory(IS31FL3731 device) {		
		this.device = device;		
	}
	
	
	/**
	 * 
	 * @param param
	 * @param ledCoordinate
	 */
	public LED createView(BooleanParameter param, IS31FL3731.LEDCoordinate ledCoordinate) throws IOException{
		
		if (usedLeds.add(ledCoordinate) == false)
			throw new IllegalArgumentException("LED " + ledCoordinate + " of the MCP23017 device is already in use");
		
		LED led = new LED(this.device, ledCoordinate);
		param.addSynthParameterEditListener(led);
		
		return led;
	}

	/**
	 * Creates a BarGraph for the given parameter
	 * 
	 * @param param the corresponding MIDI parameter
	 * @param row a row of 8 leds, see IS31FL3731 datasheet 
	 */
	public BarGraph createView(MIDIParameter param, IS31FL3731.Matrix AorB, int row) throws IOException{
		
		// check use of pins:
		IS31FL3731.LEDCoordinate ledCoordinate = new IS31FL3731.LEDCoordinate(row, 0, AorB); 
		for (int col=0; col < 8; col++) {
			ledCoordinate.setColumn(col);
			if (usedLeds.add(ledCoordinate) == false)
				throw new IllegalArgumentException("LED " + ledCoordinate + " of the MCP23017 device is already in use");
		}

		// parameter -> bar graph
		BarGraph barGraph = new BarGraph(device, AorB, row);
		param.addSynthParameterEditListener(barGraph);
		return barGraph;
	}
	
	/**
	 * Creates a view comprised of a group of LEDs for the given EnumParameter
	 * 
	 * @param row row in matrix A or B, see IS31FL3731 datasheet 
	 */
	public LEDGroup createView(EnumParameter<?> param, IS31FL3731.Matrix AorB, int row, int colStart) throws IOException{
		
		int colEnd = colStart + param.getSize() - 1;
		IS31FL3731.LEDCoordinate ledCoordinate = new IS31FL3731.LEDCoordinate(row, colStart, AorB); 
		for (int col=colStart; col <= colEnd; col++) {
			ledCoordinate.setColumn(col);
			if (usedLeds.add(ledCoordinate) == false)
				throw new IllegalArgumentException("LED " + ledCoordinate + " of the MCP23017 device is already in use");
		}
		
		LEDGroup ledGroup = new LEDGroup(device, AorB, row, colStart, colEnd);
		param.addSynthParameterEditListener(ledGroup);
		return ledGroup;
	}
			
}
