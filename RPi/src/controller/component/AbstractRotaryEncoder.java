package controller.component;

import controller.event.*;
import javafx.geometry.Orientation;
import javafx.scene.Node;
import javafx.scene.control.*;

/**
 * An abstract base class for a physical quadratic encoder. Subclass may specialize to number (int or double) parameters, or enum's. 
 * 
 * @author SR
 * 
 */
public abstract class AbstractRotaryEncoder extends Control {

	/**
	 * Adds the specified listener to receive action events from this encoder.
	 * @param l the listener
	 */
	public void addRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.add(RotaryEncoderChangeListener.class, l);
	 }

	/**
	 * Removes the specified listener so that it no longer receives action events from this encoder.
	 * @param l the listener that was previously added
	 */
	 public void removeRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.remove(RotaryEncoderChangeListener.class, l);
	 }
	 
	/**
	 * Notify all listeners that have registered interest for
	 * notification on this event type.  The event instance
	 * is lazily created using the parameters passed into
	 * the fire method.
	 */
	 protected void fireRotaryEncoderEvent(RotaryEncoderDirection dir) {
		 
	     // Guaranteed to return a non-null array
	     Object[] listeners = listenerList.getListenerList();
	     
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     RotaryEncoderEvent e = null;
	     for (int i = listeners.length-2; i>=0; i-=2) {
	         if (listeners[i]==RotaryEncoderChangeListener.class) {
	             // Lazily create the event:
	             if (e == null) e = new RotaryEncoderEvent(this, dir);
	             ((RotaryEncoderChangeListener)listeners[i+1]).encoderRotated(e); // TODO (reynal) fire changes on EDT!
	         }
	     }
}

	
	// TODO : registerComponent(MCP23017 mcpDevice, int pintA, int pinB){...}
	
	
}
