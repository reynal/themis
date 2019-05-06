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


	/**
	 * @return the number of constants for this param
	 */
	public int getSize() {
		return 2;
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

	@Override
	public double getValueAsRatio() {
		
		if (value) return 1.0;
		else return 0.0;
	}

	@Override
	public int getValueAsMIDICode() {
		if (value) return 127;
		return 0;
	}


	@Override
	public void setValueAsMIDICode(int v) {
		if (v < 64) value = false;
		else value = true;
		fireSynthParameterEditEvent();
	}

}
