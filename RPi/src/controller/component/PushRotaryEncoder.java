package controller.component;

import javax.swing.event.EventListenerList;

import controller.*;
import controller.event.PushButtonActionEvent;
import controller.event.PushButtonActionListener;
import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;

public class PushRotaryEncoder {

	private long encoderValue = 0;
	private boolean state;
	EventListenerList listenerList;
	
	public long getValue() {
        return encoderValue;
    }
	
	public boolean getState() {
		return state;
	}
    
	public void addRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.add(RotaryEncoderChangeListener.class, l);
	 }

	 public void removeRotaryEncoderChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.remove(RotaryEncoderChangeListener.class, l);
	 }

	public void addPushButtonActionListener(PushButtonActionListener l) {
		 listenerList.add(PushButtonActionListener.class, l);
	}

	public void removePushButtonActionListener(PushButtonActionListener l) {
		 listenerList.remove(PushButtonActionListener.class, l);
	}
	 
	 /**
	 * Notify all listeners that have registered interest for
	 * notification on this event type.  The event instance
	 * is lazily created using the parameters passed into
	 * the fire method.
	 *
	 * To be called from Pi4J code
	 */
	 protected void fireEncoderRotatedEvent() {
		 
	     // Guaranteed to return a non-null array
	     Object[] listeners = listenerList.getListenerList();
	     
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     RotaryEncoderEvent e = null;
	     for (int i = listeners.length-2; i>=0; i-=2) {
	         if (listeners[i]==RotaryEncoderChangeListener.class) {
	             // Lazily create the event:
	             if (e == null) e = new RotaryEncoderEvent(this);
	             ((RotaryEncoderChangeListener)listeners[i+1]).encoderRotated(e); // TODO (reynal) fire changes on EDT!
	         }
	     }	   
	 }	 
	 
		/**
		 * Notify all listeners that have registered interest for
		 * notification on this event type.  The event instance
		 * is lazily created using the parameters passed into
		 * the fire method.
		 *
		 * To be called from Pi4J code
		 */
		 protected void fireEncoderPushedEvent() {
			 
		     // Guaranteed to return a non-null array
		     Object[] listeners = listenerList.getListenerList();
		     
		     // Process the listeners last to first, notifying
		     // those that are interested in this event
		     PushButtonActionEvent e = null;
		     for (int i = listeners.length-2; i>=0; i-=2) {
		         if (listeners[i]==PushButtonActionListener.class) {
		             // Lazily create the event:
		             if (e == null) e = new PushButtonActionEvent(this);
		             ((PushButtonActionListener)listeners[i+1]).actionPerformed(e); // TODO (reynal) fire changes on EDT!
		         }
		     }
} 
}
