package view.component;

import model.event.SynthParameterEditEvent;
import model.event.SynthParameterEditListener;

/**
 * This component represents a bargraph with 8 LEDs based on one row (A or B) of the IS31FL3731 device.
 * @author SR
 */
public class BarGraph implements SynthParameterEditListener<Double> {
	
	// -------------- fields --------------
	
	protected IS31FL3731 device;
	protected IS31FL3731.Matrix matrix;
	protected int row;
	
	public static final int MIN_VALUE = 0;
	public static final int MAX_VALUE = 10;

	private int value; // must be b/w 0 (no LED active) and MAX_VALUE;

	public static final int LED_MIN_VALUE = 0;
	public static final int LED_MAX_VALUE = 8;
	public static final int MIDI_MIN_VALUE = 0;
	public static final int MIDI_MAX_VALUE = 128;	
	
	private static int ledValue; // must be b/w 0 (no LED active) and 8
	private static int midiValue; // must be b/w 0 (no LED active) and 128
	private static int incValue; // must be 1 or -1
	
	
	// -------------- constructors --------------
	
	/**
	 * Constructs a BarGraph based on 8 led located on the given matrix (A or B) and given
	 * row of the given IS31FL3731 device, that can accept up to 128 distinct values according
	 * to the MIDI standard.
	 * @param m A or B
	 * @param device a properly init'd IS31FL3731
	 * @param the matrix row (must be b/w 0 and 9)
	 */
	public BarGraph(IS31FL3731 device, IS31FL3731.Matrix matrix, int row) {
		
		if (row < 0 || row > 9) throw new IllegalArgumentException("For IS31FL3731 row must be b/w 0 and 9 inclusive!");
		this.device = device;
		this.matrix = matrix;
		this.row = row;
		
	}

	// -------------- public and protected methods --------------
	
	
	
	public void setValue(int v) {
		if (v > MAX_VALUE)
			throw new IllegalArgumentException(v + " greater than " + MAX_VALUE);
		else if (v < MIN_VALUE)
			throw new IllegalArgumentException(v + " lower than " + MIN_VALUE);
		value = v;
	}
	
	
	//Accessor
	
	//LED VALUE
	public  void setLedValue(int v) {
		if (v > LED_MAX_VALUE)
			throw new IllegalArgumentException(v + " greater than " + LED_MAX_VALUE);
		else if (v < LED_MIN_VALUE)
			throw new IllegalArgumentException(v + " lower than " + LED_MIN_VALUE);
		ledValue = v;
	}
	
	public  int getLedValue() {
		return ledValue;
	}
	
	//MIDI VALUE
	
	public  void setMidiValue(int v) {
		if (v > MIDI_MAX_VALUE)
			throw new IllegalArgumentException(v + " greater than " + MIDI_MAX_VALUE);
		else if (v < MIDI_MIN_VALUE)
			throw new IllegalArgumentException(v + " lower than " + MIDI_MIN_VALUE);
		midiValue = v;
		// SR : ok mais ensuite faut faire quelque chose avec ça !
	}
	
	public  int getMidiValue() {
		return midiValue;
		
	}
	
	
	/*public  int getRowAddress() {
		return registerOfStateLED(row);
		
	}*/
	
	//INCREMENTAION VALUE
	
	public  void setIncValue(int i) {
		if (i == 1 | i == -1 | i==0 )
			throw new IllegalArgumentException("inc value is wrong"); // ah ? pq ?
		incValue = i;
	}
	
	public  int getIncValue() {
		return incValue;
	}
	
	
	// ------------------- test methods ------------------------
	
	public static void main (String args[]) throws Exception {
		
		BarGraph barGraph = new BarGraph(null, IS31FL3731.Matrix .A, 0);
		int i;
		//for (i=0;i<16;i++) barGraph.resetLED(i); //reset all LED before starting [SR : dot être fait dans le constructeur, pas ici]
		
		
		//MODIFIED VALUES
		int led=0;
		int midi=65;
		int inc=1;
		
		while(true) {
			int old; //OLD VALUE TO COMPARE WITH
			
			//SWITCHING OFF/ON LED
			old = ledValue;
			barGraph.setLedValue(led);
			if (old != ledValue)
				barGraph.setIncValue(inc);
				//barGraph.LedInDecBinary(row, getIncValue(), getLedValue(), 0);
			
			
			//INCREASING INTENSITY
			old = midiValue;
			barGraph.setMidiValue(midi);
			if (old != midiValue)
				barGraph.setIncValue(inc);
			//barGraph.LedInDecPWM(row, getLedValue(), getIncValue(),getMidiValue(), 0);
			
			
			Thread.sleep(50);
			
			
		}
	}

	@Override
	public void synthParameterEdited(SynthParameterEditEvent<Double> e) {
		double x = e.getValue();
		// TODO : utiliser x pour mettre à jour le bargraph via la méthode setLEDPwm()
		
	}

			
}