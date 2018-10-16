package controller.event;

import java.util.EventObject;

import controller.RotaryEncoderDirection;

public class RotaryEncoderEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	long encoderValue;
	RotaryEncoderDirection direction;

	public RotaryEncoderEvent(Object source) {
		super(source);
		// TODO Auto-generated constructor stub
	}

}
