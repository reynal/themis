package model;

import javax.swing.event.EventListenerList;
import controller.event.*;
import model.event.*;

/**
 * This class represents a model parameter (one Model may have several Parameter's), for instance:
 * - a real number encoded through a 7 bit MIDI value  (e.g.: frequency, detune)
 * - a boolean (e.g;: a switchable feature) 
 * - an enum
 * Every parameter has a name and is able to produce the appropriate control 
 * for a UI (be it virtual through javafx or physical).
 * @author S.Rey
 */
public abstract class SynthParameter<T> implements RotaryEncoderChangeListener, PushButtonActionListener {
	
	private String label;
	protected T value;
	/** a list of event listeners (e.g., view or spi transmitter) for this parameter */
	protected EventListenerList listenerList;
	

	/**
	 * @param label
	 */
	public SynthParameter(String label) {
		this.label = label;
		listenerList = new EventListenerList();
	}

	/**
	 * @return the number of values for this parameter
	 */
	public abstract int getSize();
	
	/**
	 * Adds the specified listener to receive action events from this button.
	 * 
	 * @param l the listener
	 */
	public void addSynthParameterEditListener(SynthParameterEditListener l) {
		listenerList.add(SynthParameterEditListener.class, l);
	}

	/**
	 * Removes the specified listener so that it no longer receives action events
	 * from this button.
	 * 
	 * @param l
	 *            the listener that was previously added
	 */
	public void removeSynthParameterEditListener(SynthParameterEditListener l) {
		listenerList.remove(SynthParameterEditListener.class, l);
	}

	/**
	 * Notify all listeners that have registered interest for notification on this
	 * event type. The event instance is lazily created using the parameters passed
	 * into the fire method.
	 *
	 */
	protected void fireSynthParameterEditEvent() {

		// Guaranteed to return a non-null array
		Object[] listeners = listenerList.getListenerList();

		// Process the listeners last to first, notifying
		// those that are interested in this event
		SynthParameterEditEvent e = null;
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == SynthParameterEditListener.class) {
				// Lazily create the event:
				if (e == null)
					e = new SynthParameterEditEvent(this);
				((SynthParameterEditListener) listeners[i + 1]).synthParameterEdited(e); 
			}
		}
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
		fireSynthParameterEditEvent();
	}
		
	/**
	 * Utility method for, e.g., bargraphs or the construction of a MIDI message.
	 * @return for instance, 127*(value - min)/(max - min) or anything that is equally meaningful   
	 */
	public abstract int getValueAsMIDICode();
	
	/** Utility method for MIDI message parsing coming e.g. from external controllers */
	public abstract void setValueAsMIDICode(int v);
	
	/**
	 * Return a percentage b/w 0.0 and 1.0
	 * @return (value - min)/(max - min)    
	 */
	public double getValueAsRatio() {
		return getValueAsMIDICode()/127.0;
	}

	@Override
	public String toString() {
		return super.toString() + "; label=\"" + label + "\"; midi = " + getValueAsMIDICode();
	}
	

}
