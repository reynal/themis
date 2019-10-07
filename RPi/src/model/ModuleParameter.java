package model;

import javax.swing.event.EventListenerList;
import controller.event.*;
import model.event.*;

/**
 * This class represents a synthetizer module parameter (one module may have several parameters), for instance:
 * - a real number encoded through a 7 bit MIDI value  (e.g.: frequency, detune)
 * - a boolean (e.g. a switchable feature) 
 * - an enum
 * 
 * @author Reynal
 */
public abstract class ModuleParameter<T> implements RotaryEncoderChangeListener, PushButtonActionListener {
	
	private String label;
	protected T value;
	/** a list of event listeners (e.g., view or serial transmitter) for this parameter */
	protected EventListenerList listenerList;
	

	/**
	 * @param label a label (aka name) for an associated UI interface, or for debugging purpose.
	 */
	public ModuleParameter(String label) {
		this.label = label;
		listenerList = new EventListenerList();
	}

	/**
	 * @return the number of values for this parameter
	 */
	public abstract int getValuesCount();
	
	/**
	 * Adds the specified listener to receive change events from this SynthParameter.
	 * 
	 * @param l the listener
	 */
	public void addModuleParameterChangeListener(ModuleParameterChangeListener l) {
		listenerList.add(ModuleParameterChangeListener.class, l);
	}

	/**
	 * Removes the specified listener so that it no longer receives change events
	 * from this SynthParameter.
	 * 
	 * @param l the listener that was previously added
	 */
	public void removeModuleParameterChangeListener(ModuleParameterChangeListener l) {
		listenerList.remove(ModuleParameterChangeListener.class, l);
	}

	/**
	 * Notify all listeners that have registered interest for notification on this
	 * event type. The event instance is lazily created using the parameters passed
	 * into the fire method.
	 *
	 */
	protected void fireModuleParameterChangeEvent() {

		// Guaranteed to return a non-null array
		Object[] listeners = listenerList.getListenerList();

		// Process the listeners last to first, notifying
		// those that are interested in this event
		ModuleParameterChangeEvent e = null;
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == ModuleParameterChangeListener.class) {
				// Lazily create the event:
				if (e == null)
					e = new ModuleParameterChangeEvent(this);
				((ModuleParameterChangeListener) listeners[i + 1]).moduleParameterChanged(e); 
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
	
	/**
	 * Change the current parameter value and fire a change event.
	 */
	public void setValue(T value) {
		this.value = value;
		fireModuleParameterChangeEvent();
	}
		
	/**
	 * Utility method for bargraphs or to construct a MIDI message for a MIDI out port.
	 * @return a MIDI data, for instance, 127*(value - min)/(max - min) or anything that is equally meaningful   
	 */
	public abstract int getValueAsMIDICode();
	
	/** Utility method for MIDI message parsing coming e.g. from external controllers or MIDI in port */
	public abstract void setValueFromMIDICode(int midiData);
	
	/**
	 * Return a percentage b/w 0.0 and 1.0
	 * @return (value - min)/(max - min)    
	 */
	public double getValueAsRatio() {
		return getValueAsMIDICode()/127.0;
	}

	@Override
	public String toString() {
		return super.toString() + label + ": midi=" + getValueAsMIDICode() + "/" + (getValuesCount()-1) + " val=" + getValue();
	}
	

}
