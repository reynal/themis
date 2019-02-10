package model;

import java.util.*;

public class AbstractModel implements SynthParameterProvider {

	protected final List<SynthParameter<?>> parameterList = new ArrayList<SynthParameter<?>>();

	public List<SynthParameter<?>> getParameters() {		
		return parameterList;
	}
	

}
