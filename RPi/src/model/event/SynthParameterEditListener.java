package model.event;

import java.util.EventListener;

/**
 * The listener interface for receiving SynthParameterEdit events.
 * The class that is interested in processing a SynthParameterEdit event implements this interface, 
 * and the object created with that class is registered with a SynthParameter, 
 * using the class's addSythParameterEditListener method. 
 * When the edit event occurs, that object's synthParameterEdited method is invoked.
 * @author sydxrey
 *
 */
public interface SynthParameterEditListener<T> extends EventListener {
	
	/**
	 * Invoked when an edit occurs.
	 * @param e
	 */
	void synthParameterEdited(SynthParameterEditEvent<T> e);

}
