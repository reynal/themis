package model;

import controller.component.*;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;

/**
 * This class represents a model parameter of type "double"
 */
public class DoubleParameter extends SynthParameter<Double> implements RotaryEncoderChangeListener {

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
		value=0.0;
		this.min = min;
		this.max = max;
		this.step = step;
	}

	@Override
	public Control createControl() {
		NumberRotaryEncoder e = new NumberRotaryEncoder(getLabel());
		e.addRotaryEncoderChangeListener(this);
		return e;
	}

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		switch (e.getDirection()) {
		case UP : 
			value+=step;
			if (value > max) value=max;
			break;
			
		case DOWN : 
			value-=step;
			if (value < min) value =min;
			break;
		}
		
	}	
	
	// -------------------------------
	
	// test method to check the listener mechanics
	public static void main(String[] args) {
		
		VcoCEM3340 model = new VcoCEM3340();

		model.getDetuneParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #1 needs update : " + e));
		model.getOctaveParameter().addSynthParameterEditListener(e -> System.out.println("Bargraph #2 needs update : " + e));
		
		model.setDetune(0.03);
		model.setOctave(Octave.TWO_INCHES);

	}
}
