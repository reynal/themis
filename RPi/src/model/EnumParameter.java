package model;

import controller.component.*;

/**
 * This class represents a model parameter of type "double"
 */
public class EnumParameter<T extends Enum<?>> extends SynthParameter<Enum<?>> {

	@Override
	public Control createControl() {
		return null;
	}

	/**
	 * @return the upper bound for this parameter. 
	 * May be used by the UI to be able to display ticks and grid labels
	 */
	public T getMax() {
		//return value.getClass().getEnumConstants()[0];
		return null; // TODO
	}

	/**
	 * @return the lower bound for this parameter. 
	 * May be used by the UI to be able to display ticks and grid labels
	 */
	public T getMin() {
		//return 0.0;
		return null; // TODO
	}	
	
	// test
	
	public static void main(String[] args){
		EnumParameter<Octave> p = new EnumParameter<Octave>();
		p.getMin().ordinal();
		
	}

}
