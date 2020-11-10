package model;

import java.util.List;

/**
 * an interface for modules that are able to tell what parameters they use
 * @author Bastien Fratta
 *
 */
public interface ModuleParameterProvider {
	
	List<ModuleParameter<?>> getParameters();
	
}
