package model;

import controller.component.*;

/**
 * This class represents a model parameter of type "double"
 */
public class DoubleParameter extends SynthParameter<Double> {

	private double min, max;
	
	public static final double DEFAULT_MIN = 0.0;
	public static final double DEFAULT_MAX = 1.0;
	
	@Override
	public Control getControl() {
		return new NumberRotaryEncoder<Double>(getLabel(), getMin(), getMax());
	}

	/**
	 * @return the upper bound for this parameter. 
	 * May be used by the UI to be able to display ticks and grid labels
	 */
	public double getMax() {
		return 1.0;
	}

	/**
	 * @return the lower bound for this parameter. 
	 * May be used by the UI to be able to display ticks and grid labels
	 */
	public double getMin() {
		return 0.0;
	}	

}
