package model;

import java.util.List;
import controller.component.Control;

/**
 * One parameter for a model: a double (frequency, detune), a boolean, an enum etc
 * Every parameter has a name, is able to produce the appropriate control (java fx or physical).
 * @author Bastien Fratta
 *
 */
public abstract class SynthParameter<T extends Number>{
	
	private String label;
		
	public abstract Control getControl();
	
	public String getLabel() {
		return label;
	}
	
	

}
