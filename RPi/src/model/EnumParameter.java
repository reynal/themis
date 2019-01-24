package model;

import controller.component.*;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;

/**
 * This class represents a model parameter of type "double"
 */
public class EnumParameter<T extends Enum<T>> extends SynthParameter<Enum<T>> implements RotaryEncoderChangeListener {
	
	protected Class<T> clazz; //

	public EnumParameter(Class<T> clazz, String lbl) {
		super(lbl);
		this.clazz = clazz;
		value = getMin();
	}

	@Override
	public Control createControl() {
		EnumRotaryEncoder e = new EnumRotaryEncoder(getLabel());
		e.addRotaryEncoderChangeListener(this);
		return e;
	}

	/**
	 * @return the upper bound for this parameter. 
	 * May be used by the UI to be able to display ticks and grid labels
	 */
	public T getMax() {
		return clazz.getEnumConstants()[clazz.getEnumConstants().length-1];
	}

	/**
	 * @return the lower bound for this parameter. 
	 * May be used by the UI to be able to display ticks and grid labels
	 */
	public T getMin() {
		return clazz.getEnumConstants()[0];	
	}	
	
	/**
	 * Get the number of constants for this EnumParameter
	 * @return
	 */
	public int getOrdinal() {
		return clazz.getEnumConstants().length;
	}
	
	// test
	
	public static void main(String[] args){
		EnumParameter<Octave> p = new EnumParameter<Octave>(Octave.class, "octave");
		System.out.println(p.getOrdinal());
		System.out.println(p.getMin());
		System.out.println(p.getMax());
		
	}

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		System.out.println("Encoder rotated => enum parameter : " + e);
		switch (e.getDirection()) {
		case UP : 
			if (value.ordinal() < getOrdinal()) value = clazz.getEnumConstants()[value.ordinal()+1]; 
			break;
			
		case DOWN : 
			if (value.ordinal() > 0) value = clazz.getEnumConstants()[value.ordinal()-1]; 
			break;
		}
		
	}
	


}
