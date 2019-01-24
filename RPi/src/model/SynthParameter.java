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
 * TODO : ajouter des listener de ModelChangedEvent pour les bargraphs
 *
 */
public abstract class SynthParameter<T> {
	
	private String label;
	protected T value;
	protected Control control;

	/**
	 * 
	 * @param label
	 */
	public SynthParameter(String label) {
		this.label = label;
	}

	/**
	 * @return Return a concrete subclass of Control that is appropriate to control this paramater from the UI.
	 */
	public abstract Control createControl();
	
	/**
	 * Lazily creates a UI (physical or virtual) control for this class 
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
		// todo : fire ModelChangedEvent

	}

	@Override
	public String toString() {
		
		return super.toString() + "; label=\"" + label + "\"; value = \"" + value + "\"";
	}
	

}
