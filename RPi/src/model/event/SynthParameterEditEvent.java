package model.event;

import java.util.EventObject;

import javafx.event.EventHandler;

/**
 * 
 * @author sydxrey
 *
 * @param <T> the parametrized type of the source parameter, e.g., Double or Enum or Boolean.
 */
public class SynthParameterEditEvent<T> extends EventObject {

	private static final long serialVersionUID = 1L;
	
	T value;

	/**
	 * 
	 * @param source the SynthParameter that changed
	 * @param state
	 */
	public SynthParameterEditEvent(Object source, T value) {
		super(source);
		this.value = value;
	}
	
	public T getValue() {
		
		return value;
	}
	
	
}
