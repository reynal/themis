package view.component;

import java.util.*;
import java.io.*;
import device.*;
import model.*;

/**
 * A factory that can build a view (a bargraph, a group of leds, etc) for a synth parameter 
 * with all LEDs being controlled by an IS31FL3731 led driver.
 * 
 * @author reynal
 *
 */
public class ViewFactory {

	private IS31FL3731 device;
	private HashSet<IS31FL3731.LEDCoordinate> usedLedsA = new HashSet<IS31FL3731.LEDCoordinate>();
	private HashSet<IS31FL3731.LEDCoordinate> usedLedsB = new HashSet<IS31FL3731.LEDCoordinate>();
		
	/**
	 * Construct a new factory for views based on the given IS31FL3731 device.
	 */
	public ViewFactory(IS31FL3731 device) {		
		this.device = device;		
	}
	
	
	/**
	 * Creates a view based on a single LED.
	 */
	public LED createView(SynthParameter<?> param, IS31FL3731.LEDCoordinate ledCoordinate) throws IOException{
		
		System.out.println("ViewFactory: creating LED for " + param + " at " + ledCoordinate);
		HashSet<IS31FL3731.LEDCoordinate> usedLeds = (ledCoordinate.AorB == IS31FL3731.Matrix.A ? usedLedsA : usedLedsB);
		if (usedLeds.add(ledCoordinate) == false)
			throw new IllegalArgumentException("[SingleLED] " + ledCoordinate + " of the IS31FL3731 device is already in use");
		
		LED led = new LED(this.device, ledCoordinate);
		if (param != null) param.addSynthParameterEditListener(led);
		
		return led;
	}

	/**
	 * Creates a BarGraph for the given parameter
	 * 
	 * @param param the corresponding MIDI parameter
	 * @param row a row of 8 leds, see IS31FL3731 datasheet 
	 */
	public BarGraph createView(SynthParameter<?> param, IS31FL3731.Matrix AorB, int row) throws IOException{
		
		System.out.println("ViewFactory: creating a BarGraph for " + param + " at row " + row + " on matrix "+AorB);
		
		HashSet<IS31FL3731.LEDCoordinate> usedLeds = (AorB == IS31FL3731.Matrix.A ? usedLedsA : usedLedsB);
		
		// check use of pins:
		IS31FL3731.LEDCoordinate ledCoordinate = new IS31FL3731.LEDCoordinate(row, 0, AorB); 
		for (int col=0; col < 8; col++) {
			ledCoordinate.setColumn(col);
			if (usedLeds.add(ledCoordinate) == false)
				throw new IllegalArgumentException("[BarGraph] " + ledCoordinate + " of the IS31FL3731 device is already in use");
		}

		// parameter -> bar graph
		BarGraph barGraph = new BarGraph(device, AorB, row);
		if (param != null) param.addSynthParameterEditListener(barGraph);
		return barGraph;
	}
	
	/**
	 * Creates a view comprised of a group of LEDs for the given EnumParameter
	 * 
	 * @param row row in matrix A or B, see IS31FL3731 datasheet 
	 */
	public BarGraph createView(SynthParameter<?> param, IS31FL3731.Matrix AorB, int row, int colStart) throws IOException{
		
		System.out.println("ViewFactory: creating a group of leds for " + param + " at row " + row + " on matrix "+AorB + " starting at column " + colStart);
		
		HashSet<IS31FL3731.LEDCoordinate> usedLeds = (AorB == IS31FL3731.Matrix.A ? usedLedsA : usedLedsB);
		
		int colEnd = param == null ? colStart + 1 : colStart + param.getSize() - 1;
		IS31FL3731.LEDCoordinate ledCoordinate = new IS31FL3731.LEDCoordinate(row, colStart, AorB); 
		for (int col=colStart; col <= colEnd; col++) {
			ledCoordinate.setColumn(col);
			if (usedLeds.add(ledCoordinate) == false)
				throw new IllegalArgumentException("[LEDGroup] " + ledCoordinate + " of the IS31FL3731 device is already in use");
		}
		
		BarGraph ledGroup = new BarGraph(device, AorB, row, colStart, colEnd);
		if (param != null) param.addSynthParameterEditListener(ledGroup);
		return ledGroup;
	}


	@Override
	public String toString() {
		return super.toString() + " Used pins  " + usedLedsA + " " + usedLedsB;
	}


}
