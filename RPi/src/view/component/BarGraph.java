package view.component;

import java.awt.*;
import java.io.*;
import java.util.*;
import java.util.logging.*;
import javax.swing.*;
import com.pi4j.io.i2c.I2CFactory.*;
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
public class BarGraph extends AbstractView implements ModuleParameterChangeListener  {

	// --------------------- fields ---------------------

	private IS31FL3731.LEDCoordinate[] ledArray; // array containing every LED that makes up this BarGraph
	private JLabel lblForUISimulator;
	private final static Logger LOGGER = Logger.getLogger("confLogger");


	// ------------- CONSTRUCTORS ---------------

	/**
	 * Creates a BarGraph based on LEDs
	 * @param is31fl3731 the hardware device (or null if only the simulator mode is needed)
	 * @param ledArray an array of LEDs (array is copied so that original may be modified with no consequence)
	 */
	public BarGraph(IS31FL3731 is31fl3731, IS31FL3731.LEDCoordinate[] ledArray) throws IOException {

		super(is31fl3731);
		this.ledArray = new IS31FL3731.LEDCoordinate[ledArray.length];
		System.arraycopy(ledArray, 0, this.ledArray, 0, ledArray.length);		
		initLEDArrayState();
	}
		
	/**
	 * Creates a BarGraph based on a full row of LEDs of the IS31FL3731 device
	 * @param is31fl3731 the hardware device (or null if only the simulator mode is needed)
	 * @param row row index from 0 to 8
	 * @param matrix A or B
	 */
	public BarGraph(IS31FL3731 is31fl3731, IS31FL3731.Matrix matrix, int row) throws IOException {

		super(is31fl3731);
		
		this.ledArray = new IS31FL3731.LEDCoordinate[8];
		for (int col = 0; col < this.ledArray.length; col++) this.ledArray[col] = new IS31FL3731.LEDCoordinate(row, col, matrix);
		initLEDArrayState();
	}

	/**
	 * Creates a BarGraph based on a partial row of LEDs of the IS31FL3731 device
	 * @param is31fl3731 the hardware device (or null if only the simulator mode is needed)
	 * @param row row index from 0 to 8
	 * @param matrix A or B
	 * @param colStart column index of first LED
	 * @param ledCount number of leds
	 */
	public BarGraph(IS31FL3731 is31fl3731, IS31FL3731.Matrix matrix, int row, int colStart, int ledCount) throws IOException {

		super(is31fl3731);
		
		this.ledArray = new IS31FL3731.LEDCoordinate[ledCount];
		for (int i = 0; i < this.ledArray.length; i++) this.ledArray[i] = new IS31FL3731.LEDCoordinate(row, colStart+i, matrix);
		initLEDArrayState();
	}
	
	/**
	 * Make sure all LED's attached to this object are in the proper initial state.
	 */
	private void initLEDArrayState() throws IOException {
		
		if (is31fl3731 != null) {
			for (IS31FL3731.LEDCoordinate ledCoordinate : ledArray) {
				is31fl3731.switchLED(ledCoordinate, true); // switch on all LEDs for this bargraph
				is31fl3731.setLEDpwm(ledCoordinate, 0); // but keep 'em at 0%
			}
		}
		
	}
	
	/**
	 * @return the number of LEDs in the group
	 */
	public int getLEDCount() {
		return this.ledArray.length;
	}

	/**
	 * Primarily for use by EnumParameter: switch on led number "i" (and only that one)
	 * @param led led number, from 0 to LED count - 1
	 */
	public void switchLed(int led) {

		// check argument:
		if (led >= getLEDCount()) led = getLEDCount()-1;
		else if (led<0) led=0;
		
		// hardware:
		if (is31fl3731 != null) {
			try {
				for (IS31FL3731.LEDCoordinate ledCoordinate : ledArray) is31fl3731.setLEDpwm(ledCoordinate, 0);
				is31fl3731.setLEDpwm(ledArray[led], IS31FL3731.MAX_PWM); // TODO : animate!
			} catch (IOException e) {
				LOGGER.severe("Hardware error");
				e.printStackTrace();
			}
		}

		// simulator:
		if (lblForUISimulator != null) {
			String s="";
			for (int i=0; i<getLEDCount(); i++) {
				if (i==led) s+="O";
				else s+="o";
				
			}
			//lblForUISimulator.setText(getSimulatorDisplayString() + " led" + ledArray[led] + " is ON");
			lblForUISimulator.setText(getSimulatorDisplayString() + " "+s);
		}
	}
	
	/**
	 * For primary use by MIDIParameter.
	 * LED are progressively switched on as the value gets increased from 0 to 127
	 * @param v between 0 and 127
	 * @throws IOException in case there's an issue on the I2C bus
	 */
	public void setValue(int midiValue) throws IOException{

		if (is31fl3731 != null) {
			
			midiValue &= MIDI_MAX_VALUE; // make sure it's b/w 0 and 127
			int fullLeds = (midiValue+1) * getLEDCount() / (MIDI_MAX_VALUE+1); // number of leds at 100% 			


			// first we light up the first leds at 100%
			for (int led=0; led < getLEDCount(); led++) {
				if (led < fullLeds) 
					is31fl3731.setLEDpwm(ledArray[led], IS31FL3731.MAX_PWM);
				else  if (led == fullLeds)
					is31fl3731.setLEDpwmGammaCorrected16(ledArray[led], (midiValue + 1) % ((MIDI_MAX_VALUE+1) / getLEDCount()));
				else
					is31fl3731.setLEDpwm(ledArray[led], IS31FL3731.MIN_PWM);
			}
		}
		
		if (lblForUISimulator != null)
			lblForUISimulator.setText(getSimulatorDisplayString() + " at " + Integer.toString(midiValue));
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
	public void moduleParameterChanged(ModuleParameterChangeEvent e) {

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
			lblForUISimulator = new JLabel(getSimulatorDisplayString() + "=?");
			lblForUISimulator.setForeground(Color.red);
		}
		return lblForUISimulator;

	}

	/**
	 * for UI simulator debugging purpose
	 */
	private String getSimulatorDisplayString() {
		
		return "BarGraph[" + ledArray[0] + "..." + ledArray[ledArray.length-1] +"]";
		//return "BarGraph[" + Arrays.deepToString(ledArray) +"]";
	}		
	// --------------------- test ----------------------

	public static void main (String args[]) throws Exception {

		//test1();
		test2();		
		//testHeadless();
	}			
	
	// basic headless test
	private static void testHeadless() throws IOException, UnsupportedBusNumberException, InterruptedException {
		
		Scanner in = new Scanner(System.in);
		
		IS31FL3731 is31fl3731 = new IS31FL3731();
		BarGraph bg = new BarGraph(is31fl3731, IS31FL3731.Matrix.B, 0);
		
		// test switchLed()
		//bg.switchLed(3);
		while(true) {
		for (int i=0; i<128; i++) {
			bg.setValue(i);
			System.out.println(i);
			//in.nextLine();
			Thread.sleep(50);
		}
		}
	}
	
	// basic test with no hardware
	private static void test1() throws IOException {

		BarGraph group = new BarGraph(null, IS31FL3731.Matrix.A, 0, 0, 4);
		//BarGraph group = new BarGraph(null, IS31FL3731.Matrix.A, 0);
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
				e1.printStackTrace();
			}
		});
		//f.pack();
		f.setVisible(true);

	}	

	// real hardware test with Vco3340 model
	private static void test2() throws IOException, UnsupportedBusNumberException {
		
		//IS31FL3731 is31fl3731 = new IS31FL3731();
		IS31FL3731 is31fl3731 = null;

		Vco3340AModule vco3340 = new Vco3340AModule();		
		
		BarGraph bar1 = new BarGraph(is31fl3731, IS31FL3731.Matrix.B, 0); // row=0
		vco3340.getDutyParameter().addModuleParameterChangeListener(bar1);
		
		BarGraph bar2 = new BarGraph(is31fl3731, IS31FL3731.Matrix.B, 6, 0, 4); // row=6 lower
		vco3340.getOctaveParameter().addModuleParameterChangeListener(bar2);

		JFrame f = new JFrame("BarGraph test");
		f.setSize(1000,400);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new GridLayout(2,3));
		
		f.add(new JLabel("Duty cycle:"));
		JSlider s;
		f.add(s=new JSlider(0, 127));
		f.add(bar1.getUIForSimulator());
		s.addChangeListener(e -> vco3340.getDutyParameter().setValueFromMIDICode(((JSlider)e.getSource()).getValue()));
		
		f.add(new JLabel("Octave:"));
		f.add(s=new JSlider(0, 3));
		f.add(bar2.getUIForSimulator());
		s.addChangeListener(e -> vco3340.setOctave(Octave.values()[((JSlider)e.getSource()).getValue()]));

		//f.pack();
		f.setVisible(true);

	}	
}
