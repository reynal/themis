package model;

import controller.event.*;

/**
 * This class represents a model parameter whose value is an integer that can vary b/w 0 and 127
 */
public class MIDIParameter extends ModuleParameter<Integer> {

	// inherited : Integer value

	/**
	 * Construct a new DoubleParameter with the given bounds and increment step
	 * @param min lower bound
	 * @param max upper bound
	 * @param step the increment/decrement step
	 */
	public MIDIParameter(String lbl) {
		super(lbl);
		this.value=0;
	}
	
	/**
	 * @return the number of constants for this param
	 */
	public int getValuesCount() {
		return 128;
	}
	
	/**
	 * Utility method for, e.g., bargraphs.
	 * @return (value - min)/(max - min), that is, the parameter value as a ratio from 0.0 to 1.0.   
	 */
	public int getValueAsMIDICode() {		
		return value;		
	}		

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		double old = getValue();
		switch (e.getDirection()) {
		case UP : 
			this.value++;
			if (this.value > 127) this.value=127;
			break;
			
		case DOWN : 
			this.value--;
			if (this.value < 0) this.value = 0;
			break;
		}
		if (this.value != old) fireChangeEvent();
	}	
	
	@Override
	public void actionPerformed(PushButtonActionEvent e) {

		this.value++;
		if (this.value > 127) this.value=0;
		fireChangeEvent();
		
	}

	@Override
	public void setValueFromMIDICode(int v) {
		value = v;
		fireChangeEvent();
	}
	
	// -------------------------------
	
	// test method to check the listener mechanics
	public static void main(String[] args) {
		
		Vco3340AModule model = new Vco3340AModule();

		//model.getDetuneParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #1 needs update : " + e));
		//model.getOctaveParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #2 needs update : " + e));
		
		model.setDetune(3);
		model.setOctave(Octave.TWO_INCHES);

	}




}

