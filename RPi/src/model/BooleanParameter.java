package model;

import controller.component.*;
import controller.event.PushButtonActionEvent;
import controller.event.PushButtonActionListener;

/**
 * This class represents a model parameter of type "double"
 */
public class BooleanParameter extends SynthParameter<Boolean> implements PushButtonActionListener {

	/**
	 * Constructs a default parameter with "false" as the default value
	 */
	public BooleanParameter(String name) {
		super(name);
		value = false;
	}

	@Override
	public Control createControl() {
		PushButton b = new PushButton(getLabel());
		b.addPushButtonActionListener(this);
		return b;
	}

	@Override
	public void actionPerformed(PushButtonActionEvent e) {
		
		value = e.getState();
		
	}

}
