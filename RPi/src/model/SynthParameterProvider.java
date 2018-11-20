package model;

import java.util.List;

/**
 * an interface for models that are able to tell what parameters they're based on
 * @author Bastien Fratta
 *
 */
public interface SynthParameterProvider {
	
	List<SynthParameter> getParameters();
	
}
