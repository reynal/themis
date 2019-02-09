package model;

import javax.swing.event.EventListenerList;

import controller.component.Control;
import model.event.*;

/**
 * This class represents a model parameter (one Model may have several Parameter's), for instance:
 * - a double (e.g.: frequency, detune), 
 * - a boolean (e.g;: a switchable feature) 
 * - an enum
 * Every parameter has a name and is able to produce the appropriate control 
 * for a UI (be it virtual through javafx or physical).
 * @author S.Rey
 * 
 *
 */
public abstract class SynthParameter<T> {
	
	private String label;
	protected T value;
	protected Control control;
	/** a list of event listeners (e.g., view or spi transmitter) for this parameter */
	protected EventListenerList listenerList;
	

	/**
	 * 
	 * @param label
	 */
	public SynthParameter(String label) {
		this.label = label;
		listenerList = new EventListenerList();
	}

	/**
	 * Adds the specified listener to receive action events from this button.
	 * 
	 * @param l the listener
	 */
	public void addSynthParameterEditListener(SynthParameterEditListener<?> l) {
		listenerList.add(SynthParameterEditListener.class, l);
	}

	/**
	 * Removes the specified listener so that it no longer receives action events
	 * from this button.
	 * 
	 * @param l
	 *            the listener that was previously added
	 */
	public void removeSynthParameterEditListener(SynthParameterEditListener<?> l) {
		listenerList.remove(SynthParameterEditListener.class, l);
	}

	/**
	 * Notify all listeners that have registered interest for notification on this
	 * event type. The event instance is lazily created using the parameters passed
	 * into the fire method.
	 *
	 */
	protected void fireSynthParameterEditEvent(T value) {

		// Guaranteed to return a non-null array
		Object[] listeners = listenerList.getListenerList();

		// Process the listeners last to first, notifying
		// those that are interested in this event
		SynthParameterEditEvent<T> e = null;
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == SynthParameterEditListener.class) {
				// Lazily create the event:
				if (e == null)
					e = new SynthParameterEditEvent<T>(this, value);
				((SynthParameterEditListener<T>) listeners[i + 1]).synthParameterEdited(e); 
			}
		}
	}	
	/**
	 * @return Return a concrete subclass of Control that is appropriate to control this paramater from the UI.
	 */
	public abstract Control createControl();
	
	/**
	 * Lazily creates a UI control for this class. The control itself may be either purely physical or have
	 * a UI representation (e.g., JavaFX) when in simulator mode. 
	 */
	public Control getControl(){
		if (control == null) control = createControl();
		return control;
	}
	
	/**
	 * @return An UI label for this parameter
	 */
	public String getLabel() {
		return label;
	}
	
	/**
	 * @return the current parameter value
	 */
	public T getValue(){
		return value;
	}
	
	public void setValue(T value) {
		this.value = value;
		fireSynthParameterEditEvent(value);
	}

	@Override
	public String toString() {
		return super.toString() + "; label=\"" + label + "\"; value = \"" + value + "\"";
	}
	

}
