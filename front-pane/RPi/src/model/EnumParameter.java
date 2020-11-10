package model;

import controller.component.PushButtonState;
import controller.event.PushButtonActionEvent;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;

/**
 * This class represents a model parameter of type "double"
 */
public class EnumParameter<T extends Enum<T>> extends ModuleParameter<T>  {
	
	protected Class<T> clazz; //

	public EnumParameter(Class<T> clazz, String lbl) {
		super(lbl);
		this.clazz = clazz;
		value = getMin();
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
	 * May be used by the UI to to display ticks and grid labels
	 */
	public T getMin() {
		return clazz.getEnumConstants()[0];	
	}	
	
	/**
	 * @return the number of constants for this EnumParameter
	 */
	public int getValuesCount() {
		return clazz.getEnumConstants().length;
	}
	
	/**
	 * @return the rank of the current value in the set of enum fields
	 */
	public int getOrdinal() {
		return value.ordinal();
	}
	

	@Override
	public void encoderRotated(RotaryEncoderEvent e) {
		
		// depending on direction, switch to next or previous enum field
		switch (e.getDirection()) {
		case UP : 
			if (value.ordinal() < getValuesCount()-1) {
				value = clazz.getEnumConstants()[value.ordinal()+1];
			}			
			else 
				value = clazz.getEnumConstants()[0]; // wraps to origin
			fireChangeEvent();
			break;
			
		case DOWN : 
			if (value.ordinal() > 0) {
				value = clazz.getEnumConstants()[value.ordinal()-1];
				
			}			
			else
				value = clazz.getEnumConstants()[getValuesCount()-1];
			fireChangeEvent();
			break;
		}
		
	}

	@Override
	public void actionPerformed(PushButtonActionEvent e) {
		// keep only button press (not release):
		if (e.getState() != PushButtonState.PRESSED) return;
		
		if (value.ordinal() < getValuesCount()-1) 
			value = clazz.getEnumConstants()[value.ordinal()+1];
		else
			value = clazz.getEnumConstants()[0];
		fireChangeEvent();
		
	}
	
	@Override
	public double getValueAsRatio() {
			
			return getOrdinal() / (getValuesCount() - 1.0);
	}

	@Override
	public int getValueAsMIDICode() {
		
		return getOrdinal();
		
	}

	@Override
	public void setValueFromMIDICode(int v) {
		v = v % getValuesCount();
		value = clazz.getEnumConstants()[v];
		fireChangeEvent();
	}
	
	// ------------------ test ------
	
	// the following test allows one to understand how to use EnumParameter and check listener mechanics:
	// test
	
	public static void main(String[] args){
		EnumParameter<Octave> p = new EnumParameter<Octave>(Octave.class, "octave");
		System.out.println(p.getValuesCount());
		System.out.println(p.getMin());
		System.out.println(p.getMax());
		p.setValue(Octave.FOUR_INCHES);
		System.out.println(p.getValueAsRatio());
		// add a listener using functional programming:
		//p.addSynthParameterEditListener(e -> System.out.println(e));

		//new EnumParameter<Double>(Double.class, "% of duty cycle"); // "bound mismatch" compile error : Double is not an enum
		Octave o = p.getValue();
		System.out.println(o);
		p.setValue(Octave.TWO_INCHES);
		System.out.println(o);
	}



}
