package controller.component;

import javax.swing.event.EventListenerList;

/** 
 * this object represents a physical control that also has
 * the ability to produce a visual UI for an interface simulator.
 */
public abstract class Control {
	
	/** the control label for a UI simulator or debugging */
	protected String label;
	
	/** a list of event listeners for this control */
	protected EventListenerList listenerList;
	
	/**
	 * construct a Control with the given UI label
	 * @param label
	 */
	public Control(String label) {
		listenerList = new EventListenerList();
		this.label = label;
	}

	/**
	 * @return the label
	 */
	public String getLabel() {
		return label;
	}

	@Override
	public String toString() {
		return super.toString() + " \"" + getLabel() + "\" ";
	}
	
	

	
}
