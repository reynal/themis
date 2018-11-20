package controller.component;

import javafx.scene.*;
import model.SynthParameter;

/** 
 * this object represents a physical or virtual control 
 */
public abstract class Control {
	
	protected String label;
	private SynthParameter param;
	
	/*public Control(String label) {
		this.label = label;
	}*/
	/*
	public Control(SynthParameter param) {
		this.param=param;
	}*/
	
	/**
	 * @return a java fx component that represents this control graphically in an interface simulator
	 */
	public abstract Node getJavaFXView();

}
