package controller.event;

import java.util.EventObject;

import controller.component.RotaryEncoderDirection;

public class RotaryEncoderEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	RotaryEncoderDirection direction;

	public RotaryEncoderEvent(Object source, RotaryEncoderDirection direction) {
		super(source);
		this.direction = direction;
	}
	
	public RotaryEncoderDirection getDirection() {
		return direction;
	}

	@Override
	public String toString() {
		
		return super.toString() + ", direction = " + direction;
	}

}
