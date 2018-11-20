package controller.component;


public class EnumRotaryEncoder<T extends Enum<T>> extends AbstractRotaryEncoder {

	T currentValue;
	
	public EnumRotaryEncoder(String label) {
		super(label);
		Enum<T> e;
		//int i = e.ordinal();
		
	}
	
	public T getValue() {
        return null;
    }
    	
}
