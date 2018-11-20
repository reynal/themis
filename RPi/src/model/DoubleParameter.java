package model;

import controller.component.*;

public class DoubleParameter extends SynthParameter<Double> {

	private double min, max;
	
	public static final double DEFAULT_MIN = 0.0;
	public static final double DEFAULT_MAX = 1.0;
	
	@Override
	public Control getControl() {
		return new NumberRotaryEncoder<Double>(getLabel(), getMin(), getMax());
	}

	public double getMax() {
		return 1.0;
	}

	public double getMin() {
		return 0.0;
	}


}
