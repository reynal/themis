package model;

import java.util.ArrayList;
import java.util.List;

public class AbstractModel implements SynthParameterProvider {

	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();

	public List<SynthParameter<?>> getParameters() {		
		return parameterList;
	}
	
}
