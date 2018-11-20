package controller.event;

import java.util.EventObject;

import javafx.event.EventHandler;

public class PushButtonActionEvent extends EventObject {

	private static final long serialVersionUID = 1L;
	
	boolean state;

	public PushButtonActionEvent(Object source) {
		super(source);		
	}

	public PushButtonActionEvent(Object source, boolean state) {
		super(source);
		this.state = state;
}
}
