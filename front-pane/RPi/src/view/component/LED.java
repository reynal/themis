package view.component;

import java.awt.*;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.swing.*;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import device.IS31FL3731;
import model.*;
import model.event.*;

/**
 * A view that represents a hardware single LED based on the IS31FL3731 device.
 * This device is able to represent data from 0.0 to 1.0 continuously by
 * progressively switching this led on using PWM, or to represent boolean data
 * by simply switching b/w off and on.
 * 
 * 
 * @author reynal
 * @author lucien
 */
public class LED extends AbstractView implements ModuleParameterChangeListener  {

	// --------------------- fields ---------------------

	public final int LED_COUNT = 8;
	private JLabel lblForUISimulator;
	private IS31FL3731.LEDCoordinate ledCoordinate;
	private final static Logger LOGGER = Logger.getLogger(LED.class.getName());

	// ------------- CONSTRUCTORS ---------------

	/**
	 * Creates a view associated based on the given led
	 * @param is31fl3731 hardware device this view is based upon
	 * @param ledCoordinate coordinate of the led in the IS31FL3731 matrix coordinate system 
	 * @throws IOException 
	 */
	public LED(IS31FL3731 is31fl3731, IS31FL3731.LEDCoordinate ledCoordinate) throws IOException {

		super(is31fl3731);
		LOGGER.setLevel(Level.INFO);		
		this.ledCoordinate = ledCoordinate;
		// make sure LED is on but not lit
		if (is31fl3731 != null) {
			is31fl3731.switchLED(ledCoordinate, true);
			is31fl3731.setLEDpwm(ledCoordinate, 0);
			LOGGER.info("LED init ok");
		}
	}


	/**
	 * Switch this LED on or off (i.e. no PWM)
	 */
	public void setValue(boolean v) {
		
		if (is31fl3731 != null) {
			try {
				is31fl3731.setLEDpwm(ledCoordinate, v ? IS31FL3731.MAX_PWM : IS31FL3731.MIN_PWM);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		if (lblForUISimulator != null) 
			lblForUISimulator.setText(getDisplayString() + (v ? "ON":"OFF"));
	}

	/**
	 * Switch this LED from 0 to 100% according to midiValue
	 * @param v intensity b/w 0 and 127
	 */
	public void setValue(int midiValue) {

		midiValue &= 0x7F; // make sure it's 7 bit wide

		if (is31fl3731 != null) {
			try {
				is31fl3731.setLEDpwm(ledCoordinate, 2 * midiValue);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

		if (lblForUISimulator != null) 
			lblForUISimulator.setText(getDisplayString() + Integer.toString(midiValue));

	}

	/**
	 * Listener method for parameter changes
	 */
	public void valueChanged(ModuleParameterChangeEvent e) {

		ModuleParameter<?> source = (ModuleParameter<?>)e.getSource();
		if (source instanceof BooleanParameter) {
			boolean b = ((BooleanParameter)source).getValue();
			setValue(b);
		}
		else setValue(source.getValueAsMIDICode());
	}	


	// --------------------- UI ----------------------

	public JComponent getUIForSimulator() {

		if (lblForUISimulator == null) {
			lblForUISimulator = new JLabel(getDisplayString() + "OFF");
		}
		return lblForUISimulator;

	}

	/**
	 * for UI simulator debugging purpose
	 */
	private String getDisplayString() {

		return "LED[" + ledCoordinate.AorB + ":" + ledCoordinate.getRow() + "," + ledCoordinate.getColumn() + "] is ";
	}

	// --------------------- test ----------------------

	// Change state of LED by a smooth way thanks to PWM when we use MIDI


	public static void main(String args[]) throws Exception {

		//test1();
		//test2();
		testHeadless();
	}

	// basic headless test
	private static void testHeadless() throws IOException, UnsupportedBusNumberException, InterruptedException {
		
		IS31FL3731 is31fl3731 = new IS31FL3731();
		LED led = new LED(is31fl3731, new IS31FL3731.LEDCoordinate(0,0, IS31FL3731.Matrix.B));
		
		// testing on/off behavior: (aka BooleanParameter)
		for (int i=0; i<10; i++) {
			led.setValue(false);
			Thread.sleep(100);
			led.setValue(true);
			Thread.sleep(100);
		}
		
		// testing
		for (int i=0; i<128; i++) {
			led.setValue(i);
			System.out.print(".");
			Thread.sleep(100);
		}
		led.setValue(0);
	}
	
	
	// basic test for UI simulator
	private static void test1() throws IOException {

		LED led = new LED(null, new IS31FL3731.LEDCoordinate(0, 0, IS31FL3731.Matrix.A));
		JFrame f = new JFrame("LED test");
		f.setSize(600,200);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new FlowLayout());
		f.add(led.getUIForSimulator());
		JToggleButton b;
		JSlider s;
		f.add(b=new JToggleButton("ON/OFF"));
		f.add(s=new JSlider(0, 127));
		b.addActionListener(e -> led.setValue(b.isSelected()));
		s.addChangeListener(e -> led.setValue(s.getValue()));
		f.setVisible(true);

	}

	// test with Vco3340 model
	private static void test2() throws IOException {

		Vco3340AModule vco3340 = new Vco3340AModule();		
		LED led1 = new LED(null, new IS31FL3731.LEDCoordinate(0, 0, IS31FL3731.Matrix.A));
		LED led2 = new LED(null, new IS31FL3731.LEDCoordinate(0, 0, IS31FL3731.Matrix.A));
		vco3340.getDetuneParameter().addChangeListener(led1);
		vco3340.getSyncFrom13700Parameter().addChangeListener(led2);
		JFrame f = new JFrame("LED test");
		f.setSize(600,400);
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new GridLayout(2,3));
		
		f.add(new JLabel("Duty cycle:"));
		JSlider s;
		f.add(s=new JSlider(-64, 63));
		f.add(led1.getUIForSimulator());
		s.addChangeListener(e -> vco3340.setDetune(s.getValue()));
		
		JToggleButton b;
		f.add(new JLabel("Sync:"));
		f.add(b=new JToggleButton("ON/OFF"));
		f.add(led2.getUIForSimulator());
		b.addActionListener(e -> vco3340.setSyncFrom13700(b.isSelected()));

		
		
		
		f.pack();
		f.setVisible(true);

	}

	private static void testLucien() {

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
