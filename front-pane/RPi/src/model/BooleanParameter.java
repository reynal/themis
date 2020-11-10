package model;

import controller.event.*;

/**
 * This class represents a module parameter of type "boolean"
 */
public class BooleanParameter extends ModuleParameter<Boolean> {
	
	public static int FALSE = 0;
	public static int TRUE = 1;

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
	public int getValuesCount() {
		return 2;
	}	
	
	@Override
	public void actionPerformed(PushButtonActionEvent e) {
		
		value = !value;
		fireChangeEvent();

	}

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {

		value = !value;
		fireChangeEvent();
		
	}

	@Override
	public double getValueAsRatio() {
		
		if (value) return 1.0;
		else return 0.0;
	}

	@Override
	public int getValueAsMIDICode() {
		if (value) return TRUE;
		return FALSE;
	}


	@Override
	public void setValueFromMIDICode(int v) {
		if (v < TRUE) value = false;
		else value = true;
		fireChangeEvent();
	}

}
