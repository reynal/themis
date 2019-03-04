package view.component;

import java.io.*;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import device.*;
import model.*;
import model.event.*;

/**
 * A view that represents a hardware BarGraph based on the IS31FL3731 device.
 * This device is able to represent data from 0.0 to 1.0 continuously by 
 * progressively switching leds on using PWM. 
 * @author reynal 
 * @author lucien
 */
public class BarGraph extends AbstractView implements SynthParameterEditListener<Integer> {
		
	// --------------------- fields ---------------------

	public final int LED_COUNT = 8; // TODO not used ?
	private int row;
	
	// --------------------- CONSTRUCTORS ---------------------
	
	/**
	 * 
	 * @param is31fl3731 the hardware device if any, or null if in simulation mode
	 * @param row row of LED from 0 to 9, see device datasheet
	 * @param matrix either A or B, see device datasheet
	 * @throws Exception
	 */
	public BarGraph(IS31FL3731 is31fl3731, IS31FL3731.Matrix matrix, int row) throws IOException { //if we want to impose the state
		
		super(is31fl3731, matrix);
		this.row = row;
	}	

	/**
	 * LED are progressively switched on as the value gets increased from 0 to 127
	 * @param v between 0 and 127
	 * @throws IOException in case there's an issue on the I2C bus
	 */
	public void setValue(int val) throws IOException{
		
		val &= MIDI_MAX_VALUE; // make sure it's b/w 0 and 127
		int fullLeds = (val+1) >> 4; // number of leds at 100% (always .le. than 8)
				
		// following display code is inspired from Lucien's initial code but would need extensive testing 
		// to check if this kind of display is really efficient in actual concert situation
		
		// first we light up the first leds at 100% 
		int onLeds = (1 << fullLeds) - 1;
		is31fl3731.switchLEDs(row, onLeds, matrix);
		for (int led=0; led < fullLeds; led++)
			is31fl3731.setLEDpwm(row, led, 255);
		
		// then the remaining leds are lit up according to exp(-n/N)
		double pwm = 255;
		double a = Math.exp(-10.0/(val+0.1)); // directly depends on val
		for (int led=fullLeds; led < 9; led++){
			pwm *= a;
			is31fl3731.setLEDpwm(row, led, (int)pwm);
		}
	}	
	
	/**
	 * 
	 */
	public void synthParameterEdited(SynthParameterEditEvent<Integer> e) {
			
			DoubleParameter source = (DoubleParameter)e.getSource();
			try {
				setValue(source.getValueAsMIDICode());
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			
	}
	
	// --------------------- test ------------------------------------------
	
	public static void main (String args[]) throws Exception {
		
	}			
}