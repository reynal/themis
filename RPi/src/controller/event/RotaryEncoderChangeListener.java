package controller.event;

import java.util.EventListener;

public interface RotaryEncoderChangeListener extends EventListener {

	void encoderRotated(RotaryEncoderEvent e);
	
}