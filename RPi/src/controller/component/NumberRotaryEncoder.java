package controller.component;

import javax.swing.event.EventListenerList;

import controller.*;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;
import javafx.scene.Node;

public class NumberRotaryEncoder<T extends Number> extends AbstractRotaryEncoder {

	private T encoderValue, min, max;
	
	public NumberRotaryEncoder(String label, T min, T max) {
		super(label);
		this.min = min;
		this.max = max;
	}
	
	public T getValue() {
        return encoderValue;
    }
    	
}
