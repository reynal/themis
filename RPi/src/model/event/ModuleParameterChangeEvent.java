package model.event;

import java.util.EventObject;
import model.ModuleParameter;

/**
 * 
 * @author reynal
 *
 * @param <T> the parametrized type of the source parameter, e.g., Double or Enum or Boolean.
 */
public class ModuleParameterChangeEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	/**
	 * @param source the SynthParameter that changed
	 */
	public ModuleParameterChangeEvent(ModuleParameter<?> source) {
		super(source);
	}	
}
