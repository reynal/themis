package model;

import controller.event.*;

/**
 * This class represents a model parameter of type "double"
 * @deprecated use MIDIParameter instead
 */
public class DoubleParameter extends ModuleParameter<Double> {

	// inherited : Double value
	private double min, max, step;

	/**
	 * Construct a new DoubleParameter with the given bounds and increment step
	 * @param min lower bound
	 * @param max upper bound
	 * @param step the increment/decrement step
	 */
	public DoubleParameter(String lbl, double min, double max, double step) {
		super(lbl);
		this.value=0.0;
		this.min = min;
		this.max = max;
		this.step = step;
	}
	
	/**
	 * Construct a new DoubleParameter that can vary b/w 0 and 1 by steps of 0.01
	 * This is perfect for percentages.
	 */
	public DoubleParameter(String lbl) {
		this(lbl, 0.0, 1.0, 0.01);
	}

	/**
	 * Utility method for, e.g., bargraphs.
	 * @return (value - min)/(max - min), that is, the parameter value as a ratio from 0.0 to 1.0.   
	 */
	public int getValueAsMIDICode() {		
		return (int)(127.0 * (value - min)/(max - min));		
	}
	
	/**
	 * @return the lower bound for this DoubleParameter
	 */
	public double getMin() {
		return min;
	}

	/**
	 * @return the upper bound for this DoubleParameter
	 */
	public double getMax() {
		return max;
	}

	/**
	 * @return the step
	 */
	public double getStep() {
		return step;
	}

	/**
	 * @param step the step to set
	 */
	public void setStep(double step) {
		this.step = step;
	}	

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		double old = getValue();
		switch (e.getDirection()) {
		case UP : 
			this.value+=step;
			if (this.value > max) this.value=max;
			break;
			
		case DOWN : 
			this.value-=step;
			if (this.value < min) this.value = min;
			break;
		}
		if (this.value != old) fireChangeEvent();
	}	
	
	@Override
	public void actionPerformed(PushButtonActionEvent e) {

		this.value+=step;
		if (this.value > max) this.value=min;
		fireChangeEvent();
		
	}

	@Override
	public int getValuesCount() {
		return 0; // does not make sense here...
	}

	@Override
	public void setValueFromMIDICode(int v) {
		value = v/127.0 * (max-min) + min;		
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
