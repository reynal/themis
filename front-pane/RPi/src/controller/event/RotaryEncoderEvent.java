package controller.event;

import java.util.EventObject;

import controller.component.RotaryEncoder;

/**
 * This event is generated by a hardware component such as a RotaryEncoder when the component-specific action occurs (such as being rotated CW or CCW). 
 * @author sydxrey
 *
 */
public class RotaryEncoderEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	RotaryEncoder.Direction direction;

	public RotaryEncoderEvent(Object source, RotaryEncoder.Direction direction) {
		super(source);
		this.direction = direction;
	}
	
	public RotaryEncoder.Direction getDirection() {
		return direction;
	}

	@Override
	public String toString() {
		
		return super.toString() + ", direction = " + direction;
	}

}