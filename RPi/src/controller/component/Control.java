package controller.component;

import javax.swing.event.EventListenerList;

import javafx.scene.*;
import model.SynthParameter;

/** 
 * this object represents a physical control that also has
 * the ability to produce a visual UI for an interface simulator.
 */
public abstract class Control {
	
	/** a list of event listeners for this control */
	protected EventListenerList listenerList;
	
	/** a java fx component that represents this control graphically in an interface simulator */
	protected Node javaFXView;
	
	
	/**
	 * @return the component that represents this control graphically in an interface simulator
	 */
	public Node getJavaFXView() {
		
		if (javaFXView==null) javaFXView = createJavaFXView();
		return javaFXView;
		
	}
	
	/**
	 * creates a java fx component that represents this control graphically in an interface simulator
	 */
	abstract Node createJavaFXView();

}
