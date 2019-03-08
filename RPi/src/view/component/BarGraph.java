package view.component;

import java.awt.*;
import java.io.*;
import javax.swing.*;

import device.*;
import model.*;
import model.event.*;

/**
 * A view that represents a group of hardware LEDs based on the IS31FL3731 device, aka BarGraph
 * (with the constraint that all LEDs must be on the same row and inside the same IS31FL3731 matrix bloc) 
 * This device is able to represent:
 * - integer data up to the number of leds in the group, @see model.EnumParameter 
 * - integer data extending the number of leds, by virtue of PWM capability, @see model.MIDIParameter
 * - boolean data (which is a soft of enum with only two constants), @see model.BooleanParameter
 * 
 * @author reynal
 * @author lucien
 */
public class BarGraph extends AbstractView implements SynthParameterEditListener  {

	// --------------------- fields ---------------------

	public final int LED_COUNT = 8;
	private IS31FL3731.LEDCoordinate[] ledArray; // array of every led comprised in this BarGraph
	int colStart, colEnd;
	private JLabel lblForUISimulator;

	// ------------- CONSTRUCTORS ---------------

	/**
	 * Creates a BarGraph based on the given LEDs
	 * @param is31fl3731 the hardware device (or null if only the simulator mode is needed)
	 * @param matrix either A or B, see IS31FL3731 datasheet
	 * @param row from 0 to 9, see device datasheet
	 * @param colStart the first column hosting a led, b/w 0 and 7
	 * @param colEnd the last column hosting a led, b/w 0 and 7
	 */
	public BarGraph(IS31FL3731 is31fl3731, IS31FL3731.Matrix matrix, int row, int colStart, int colEnd) throws IOException {

		super(is31fl3731);
		
		// check params against device constraints:
		if (colStart < 0 || colStart >= LED_COUNT || colStart > colEnd || colEnd < 0 || colEnd >= LED_COUNT)
			throw new IllegalArgumentException("Illegal interval values for colStart and colEnd:" + colStart + " -> " + colEnd);
		if (colStart == colEnd) // make sure we've at least two leds
			throw new IllegalArgumentException("colStart = colEnd ! Use class LED instead in this case");
		
		this.colStart = colStart;
		this.colEnd = colEnd;		
		ledArray = new IS31FL3731.LEDCoordinate[colEnd-colStart+1];
		for (int col=colStart; col<=colEnd; col++) {
			ledArray[col-colStart] =  new IS31FL3731.LEDCoordinate(row, col, matrix);
		}
		if (is31fl3731 != null) is31fl3731.switchLEDRow(row, matrix, 0xFF); // switch on all LEDs for this row

	}
	
	/**
	 * Creates a BarGraph based on an entire row of the given matrix. Aka BarGraph.
	 */
	public BarGraph(IS31FL3731 is31fl3731, IS31FL3731.Matrix matrix, int row) throws IOException {
		
		this(is31fl3731, matrix, row, 0, 7);
		
	}
	
	/**
	 * @return the number of LEDs in the group
	 */
	public int getLEDCount() {
		return colEnd - colStart + 1;
	}

	/**
	 * For primary use by EnumParameter: switch on led number "i" (and only that one)
	 * @param led led number, from 0 to LED count - 1
	 */
	public void switchLed(int led) {

		// check argument:
		if (led > colEnd - colStart) led = colEnd - colStart;
		else if (led<0) led=0;
		
		// hardware:
		if (is31fl3731 != null) {
			// TODO (lucien)
			// switch led number (colStart + i)
		}

		// simulator:
		if (lblForUISimulator != null) 
			lblForUISimulator.setText(getDisplayString() + " led" + ledArray[led].getColumn() + " is ON");
	}
	
	/**
	 * For primary use by MIDIParameter.
	 * LED are progressively switched on as the value gets increased from 0 to 127
	 * @param v between 0 and 127
	 * @throws IOException in case there's an issue on the I2C bus
	 */
	public void setValue(int midiValue) throws IOException{

		if (is31fl3731 != null) {
			
			// TODO @lucien so far your code is based on 8 leds => should be versatile enough to work with any number of leds!

			midiValue &= MIDI_MAX_VALUE; // make sure it's b/w 0 and 127
			int fullLeds = (midiValue+1) >> 4; // number of leds at 100% (always .le. than 8)
			if (fullLeds > getLEDCount()) fullLeds = getLEDCount();

			// following display code is inspired from Lucien's initial code but would need extensive testing 
			// to check if this kind of display is really efficient in actual concert situation

			// first we light up the first leds at 100% 
			for (int led=0; led < fullLeds; led++)
				is31fl3731.setLEDpwm(ledArray[led], 255);

			// then the remaining leds are lit up according to exp(-n/N)
			double pwm = 255;
			double a = Math.exp(-10.0/(midiValue+0.1)); // directly depends on val
			for (int led=fullLeds; led < getLEDCount(); led++){
				pwm *= a;
				is31fl3731.setLEDpwm(ledArray[led], (byte)pwm);
			}
		}
		
		if (lblForUISimulator != null)
			lblForUISimulator.setText(getDisplayString() + " at " + Integer.toString(midiValue));
	}	
	
	/**
	 * For primary use by BooleanParameter
	 * @param b if true, switch LED #0 otherwise switch LED #1
	 */
	public void setValue(boolean b) {
		if (b) switchLed(1);
		else switchLed(0);
	}

	/**
	 * Listener method for parameter changes
	 */
	public void synthParameterEdited(SynthParameterEditEvent e) {

		if (e.getSource() instanceof EnumParameter) {
			EnumParameter<?> p = (EnumParameter<?>)e.getSource();
			switchLed(p.getOrdinal());
		}
		else if (e.getSource() instanceof BooleanParameter) {
			BooleanParameter p = (BooleanParameter)e.getSource();
			setValue(p.getValue());
		}
		else if (e.getSource() instanceof MIDIParameter) {
			try {
				MIDIParameter p = (MIDIParameter)e.getSource();
				setValue(p.getValueAsMIDICode());
			} catch (IOException e1) {
				e1.printStackTrace();
			}
		}

	}	

	// --------------------- UI ----------------------

	public JComponent getUIForSimulator() {

		if (lblForUISimulator == null) {
			lblForUISimulator = new JLabel(getDisplayString() + "=?");
			lblForUISimulator.setForeground(Color.red);
		}
		return lblForUISimulator;

	}

	/**
	 * for UI simulator debugging purpose
	 */
	private String getDisplayString() {
		
		return "BarGraph[" + ledArray[0].AorB + ":row" + ledArray[0].getRow() + ":" + ledArray[0].getColumn() 
				+ "->" +  ledArray[ledArray.length-1].getColumn()+"]";
	}		
	// --------------------- test ----------------------

	public static void main (String args[]) throws Exception {

		//test1();
		test2();		
	}			
	
	// basic test
	private static void test1() throws IOException {

		//BarGraph group = new BarGraph(null, IS31FL3731.Matrix.A, 0, 0, ledCount-1);
		BarGraph group = new BarGraph(null, IS31FL3731.Matrix.A, 0);
		JFrame f = new JFrame("BarGraph test");
		f.setSize(600,400);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new FlowLayout());
		f.add(group.getUIForSimulator());
		JSlider s;
		
		//f.add(s=new JSlider(0, group.getLEDCount()-1));
		//s.addChangeListener(e -> group.switchLed(s.getValue()));
		
		f.add(s=new JSlider(0, 127));
		s.addChangeListener(e -> {
			try {
				group.setValue(s.getValue());
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		});
		//f.pack();
		f.setVisible(true);

	}	

	// test with Vco3340 model
	private static void test2() throws IOException {

		Vco3340 vco3340 = new Vco3340();		
		
		BarGraph bar1 = new BarGraph(null, IS31FL3731.Matrix.A, 0);
		vco3340.getDetuneParameter().addSynthParameterEditListener(bar1);
		
		BarGraph bar2 = new BarGraph(null, IS31FL3731.Matrix.A, 0, 4, 7);
		vco3340.getOctaveParameter().addSynthParameterEditListener(bar2);

		JFrame f = new JFrame("BarGraph test");
		f.setSize(1000,400);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new GridLayout(2,3));
		
		f.add(new JLabel("Duty cycle:"));
		JSlider s;
		f.add(s=new JSlider(-64, 63));
		f.add(bar1.getUIForSimulator());
		s.addChangeListener(e -> vco3340.setDetune(((JSlider)e.getSource()).getValue()));
		
		f.add(new JLabel("Octave:"));
		f.add(s=new JSlider(0, 3));
		f.add(bar2.getUIForSimulator());
		s.addChangeListener(e -> vco3340.setOctave(Octave.values()[((JSlider)e.getSource()).getValue()]));

		//f.pack();
		f.setVisible(true);

	}	
}
