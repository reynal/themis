package model;

import controller.component.*;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;

/**
 * This class represents a model parameter of type "double"
 */
public class DoubleParameter extends SynthParameter<Double> implements RotaryEncoderChangeListener, ChangeListener<Number> {

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
	 * Utility method for, e.g., bargraphs.
	 * @return (value - min)/(max - min), that is, the parameter value as a ratio from 0.0 to 1.0.   
	 */
	public double getValueAsRatio() {
		
		return (value - min)/(max - min);
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
	public Control createControl() {
		RotaryEncoder e = new RotaryEncoder(getLabel());
		e.addRotaryEncoderChangeListener(this);
		return e;
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
		if (this.value != old) fireSynthParameterEditEvent(value);
	}	
	
	@Override
	public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number newValue) {
			
		System.out.println(toString() + " : New Value " + newValue);
		value = newValue.doubleValue();
	}

	// -------------------------------
	
	// test method to check the listener mechanics
	public static void main(String[] args) {
		
		VcoCEM3340 model = new VcoCEM3340();

		//model.getDetuneParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #1 needs update : " + e));
		//model.getOctaveParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #2 needs update : " + e));
		
		model.setDetune(0.03);
		model.setOctave(Octave.TWO_INCHES);

	}

}
