package model;

import controller.component.*;

/**
 * This class represents a model parameter of type "double"
 */
public class BooleanParameter extends SynthParameter<Boolean> {

	/**
	 * Constructs a default parameter with "false" as the default value
	 */
	public BooleanParameter() {
		super();
		value = false;
	}

	@Override
	public Control getControl() {
		return new PushButton(getLabel());
	}

}
