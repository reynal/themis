package model.event;

import java.util.EventObject;
import model.SynthParameter;

/**
 * 
 * @author sydxrey
 *
 * @param <T> the parametrized type of the source parameter, e.g., Double or Enum or Boolean.
 */
public class SynthParameterEditEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	/**
	 * @param source the SynthParameter that changed
	 */
	public SynthParameterEditEvent(SynthParameter<?> source) {
		super(source);
	}	
}
