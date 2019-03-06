package view.component;

import javax.swing.*;
import device.*;
import model.*;
import model.event.*;

/**
 * A view that represents a group of hardware LEDs based on the IS31FL3731 device 
 * (with the constraint that all LEDs must be on the same row and inside the same IS31FL3731 matrix bloc) 
 * This device is able to represent integer data up to the number of leds in the group 
 * This view is thus an appropriate listener for an EnumParameter
 * 
 * @author reynal
 * @author lucien
 */
public class LEDGroup extends AbstractView implements SynthParameterEditListener  {

	// --------------------- fields ---------------------

	public final int LED_COUNT = 8;
	private int colStart, colEnd, row;
	private JLabel lblForUISimulator;

	// ------------- CONSTRUCTORS ---------------

	/**
	 * 
	 * @param is31fl3731 the hardware device (or null if only the simulator mode is needed)
	 * @param row the row index, see IS31FL3731 datasheet
	 * @param colStart the first column hosting a led, b/w 0 and 7
	 * @param colEnd the last column hosting a led, b/w 0 and 7
	 * @param blocAB either A or B, see IS31FL3731 datasheet
	 */
	public LEDGroup(IS31FL3731 is31fl3731, IS31FL3731.Matrix blocAB, int row, int colStart, int colEnd) {

		super(is31fl3731, blocAB);
		this.row = row;
		if (colStart < 0 || colStart > 7 || colStart > colEnd || colEnd < 0 || colEnd > 7)
			throw new IllegalArgumentException("Illegal interval values for colStart and colEnd:" + colStart + " -> " + colEnd);
		this.colStart = colStart;
		this.colEnd= colEnd;
	}
	
	/**
	 * @return the number of LEDs in the group
	 */
	public int getLEDCount() {
		return colEnd - colStart + 1;
	}

	/**
	 * Switch on led number "i" 
	 * @param i led number, from 0 to LED count - 1
	 */
	public void setValue(int i) {

		// check argument:
		if (i > colEnd - colStart) i = colEnd - colStart;
		else if (i<0) i=0;
		
		// hardware:
		if (is31fl3731 != null) {
			// TODO (lucien)
			// switch led number (colStart + i)
		}

		// simulator:
		if (lblForUISimulator != null) lblForUISimulator.setText("LED (" + row + "," + (i+colStart)+")"	);
	}

	/**
	 * Listener method for parameter changes
	 */
	public void synthParameterEdited(SynthParameterEditEvent e) {

		// TODO (lucien)
		if (e.getSource() instanceof EnumParameter) {
			
			// switch 
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


}
