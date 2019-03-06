package view.component;

import java.io.IOException;

import javax.swing.JComponent;
import javax.swing.JLabel;

import com.pi4j.io.i2c.I2CBus;
import com.pi4j.io.i2c.I2CDevice;
import com.pi4j.io.i2c.I2CFactory;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import device.IS31FL3731;
import device.IS31FL3731.LEDCoordinate;
import model.DoubleParameter;
import model.event.SynthParameterEditEvent;
import model.event.SynthParameterEditListener;

/**
 * A view that represents a hardware single LED based on the IS31FL3731 device.
 * This device is able to represent data from 0.0 to 1.0 continuously by
 * progressively switching this led on using PWM, or to represent boolean data
 * by simply switching b/w off and on.
 * 
 * This view is an appropriate listener for a BooleanParameter
 * 
 * @author reynal
 * @author lucien
 */
public class LED extends AbstractView implements SynthParameterEditListener  {

	// --------------------- fields ---------------------

	public final int LED_COUNT = 8;
	private int col, row;
	private JLabel lblForUISimulator;

	// ------------- CONSTRUCTORS ---------------

	/**
	 * 
	 * @param is31fl3731
	 * @param row
	 * @param col
	 * @param matrix
	 */
	public LED(IS31FL3731 is31fl3731, IS31FL3731.LEDCoordinate ledCoordinate) {

		super(is31fl3731, ledCoordinate.AorB);
		this.row = row;
		this.col = col;
	}

	/**
	 * Switch this LED on or off
	 */
	public void setValue(boolean v) {
		
		if (is31fl3731 != null) {
			// TODO (lucien)
		}
		
		if (lblForUISimulator != null) lblForUISimulator.setText(v ? "ON":"OFF");
	}

	/**
	 * Switch this LED from 0* to 100% according to v
	 * @param v intensity b/w 0.0 and 1.0
	 */
	public void setValue(double v) {
		
		// TODO (lucien)
		
	}

	/**
	 * Listener method for parameter changes
	 */
	public void synthParameterEdited(SynthParameterEditEvent e) {
			
		// TODO (lucien)
		
	}	
	
	
	// Binary way to turn on LEDs
	private void updateBargraphBinary(int row, int val, int col) throws IOException, InterruptedException, UnsupportedBusNumberException {

		val = val | 7;
		if (col == 1) {
			val = val + this.LED_MAX_VALUE;
		}
		int i;
		for (i = this.LED_MIN_VALUE; i < val; i++) {
			//is31fl3731.switchLED(row, i, HIGH); TODO lucien
			Thread.sleep(50);
		}
		for (i = val; i < this.LED_MAX_VALUE; i++) {
			// is31fl3731.switchLED(row, i, LOW); TODO lucien
			Thread.sleep(50);
		}
	}

	// --------------------- UI ----------------------

	public JComponent getUIForSimulator() {

		if (lblForUISimulator == null) {
			lblForUISimulator = new JLabel();
		}
		return lblForUISimulator;

	}
	
	// --------------------- test ----------------------
	
	// Change state of LED by a smooth way thanks to PWM when we use MIDI


	public static void main(String args[]) throws Exception {

		/* LED led1 = new LED(1, 1, 0); // information of B 8-LED bargraph of the
										// row 2, on state 0 (Turn on or turn of
										// a LED)
		LED led2 = new LED(2, 0, 1); // information of A 8-LED bargraph of the
										// row 3, on state 1 (Use midi
										// information, then PWM)

		while (true) {

			led1.setLEDValue(3); // received value by the listener (TODO), Here
									// in example, the value is 3)
			led1.updateBargraphBinary(led1.row, led1.ledValue, led1.col);

			Thread.sleep(50);

			led2.setMidiValue(48); // received value by the listener (TODO),
									// Here in example, the value is 48)
			led2.updateBargraphPWM(led2.row, led2.midiValue, led2.col);

			Thread.sleep(50);

		} */
	}

}
