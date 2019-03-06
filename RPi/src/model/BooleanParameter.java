package model;

import controller.event.*;

/**
 * This class represents a model parameter of type "double"
 */
public class BooleanParameter extends SynthParameter<Boolean> {

	/**
	 * Constructs a default parameter with "false" as the default value
	 */
	public BooleanParameter(String name) {
		super(name);
		value = false;
	}


	
	@Override
	public void actionPerformed(PushButtonActionEvent e) {
		
		value = !value;
		fireSynthParameterEditEvent();

	}

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {

		value = !value;
		fireSynthParameterEditEvent();
		
	}

	public double getValueAsRatio() {
		
		if (value) return 1.0;
		else return 0.0;
	}

	@Override
	public int getValueAsMIDICode() {
		if (value) return 0xFF;
		return 0;
	}

}
