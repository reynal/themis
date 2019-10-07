package model.event;

import java.util.EventListener;

/**
 * The listener interface for receiving ModuleParameterChange events.
 * The class that is interested in processing a ModuleParameterChangeEvent implements this interface, 
 * and the object created with that class is registered with a ModuleParameter, 
 * using the class's addModuleParameterChangeListener method. 
 * When the edit event occurs, that object's moduleParameterChanged method is invoked.
 * @author sydxrey
 *
 */
public interface ModuleParameterChangeListener extends EventListener {
	
	/**
	 * Invoked when an edit occurs.
	 * @param e
	 */
	void moduleParameterChanged(ModuleParameterChangeEvent e);

}
