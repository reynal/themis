package controller.component;

import controller.event.*;
import device.MCP23017;

/**
 * a class that represents a push button on the front pane
 * 
 * @author SR
 *
 */
public class PushButton extends Control {

	/**
	 * construct a pushbutton with the given UI label that is connected to the given pin of the given MCP23017 device and port
	 */
	public PushButton(String label, MCP23017 mcpDevice, MCP23017.Port portAorB, MCP23017.Pin gpio) {
		super(label + "[" + portAorB + ":" + gpio + "]");
		if (mcpDevice != null) {
			// TODO : handle interrupt from MCP23017
		}
		else System.out.println("No MCP23017 registered for " + toString() +" -> simulator mode only");

	}

	/**
	 * construct a pushbutton with the given UI label
	 */
	public PushButton(String label) {
		super(label);
	}

	/**
	 * Adds the specified listener to receive action events from this button.
	 * 
	 * @param l the listener
	 */
	public void addPushButtonActionListener(PushButtonActionListener l) {
		listenerList.add(PushButtonActionListener.class, l);
	}

	/**
	 * Removes the specified listener so that it no longer receives action events
	 * from this button.
	 * 
	 * @param l
	 *            the listener that was previously added
	 */
	public void removePushButtonActionListener(PushButtonActionListener l) {
		listenerList.remove(PushButtonActionListener.class, l);
	}

	/**
	 * Notify all listeners that have registered interest for notification on this
	 * event type. The event instance is lazily created using the parameters passed
	 * into the fire method.
	 *
	 */
	public void firePushButtonActionEvent() { // note SR : should be protected, but we have to make it public cause SwingMain uses it in simumlator mode

		// Guaranteed to return a non-null array
		Object[] listeners = listenerList.getListenerList();

		// Process the listeners last to first, notifying
		// those that are interested in this event
		PushButtonActionEvent e = null;
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == PushButtonActionListener.class) {
				// Lazily create the event:
				if (e == null)
					e = new PushButtonActionEvent(this);
				((PushButtonActionListener) listeners[i + 1]).actionPerformed(e); 
			}
		}
	}

}
