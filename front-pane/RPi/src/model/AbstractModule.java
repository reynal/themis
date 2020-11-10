package model;

import java.util.*;

public class AbstractModule implements ModuleParameterProvider {

	protected final List<ModuleParameter<?>> parameterList = new ArrayList<ModuleParameter<?>>();

	public List<ModuleParameter<?>> getParameters() {		
		return parameterList;
	}
	

}
