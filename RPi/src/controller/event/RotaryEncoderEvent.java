package controller.event;

import java.util.EventObject;

import controller.component.RotaryEncoderDirection;

public class RotaryEncoderEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	long encoderValue;
	RotaryEncoderDirection direction;

	public RotaryEncoderEvent(Object source) {
		super(source); // appelle le constructeur de la superclasse
	}
	
	public long getValue() {
		return encoderValue;
	}
	
}
