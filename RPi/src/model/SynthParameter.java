package model;

import controller.component.Control;

/**
 * This class represents a model parameter (one Model may have several Parameter's), for instance:
 * - a double (e.g.: frequency, detune), 
 * - a boolean (e.g;: a switchable feature) 
 * - an enum
 * Every parameter has a name and is able to produce the appropriate control 
 * for a UI (be it virtual through javafx or physical).
 * @author S.Rey
 *
 */
public abstract class SynthParameter<T>{
	
	private String label;
	protected T value;
		
	/**
	 * @return Return a concrete subclass of Control that is appropriate to control this paramater from the UI.
	 */
	public abstract Control getControl();
	
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
	
	

}
