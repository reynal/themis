package controller.component;

import javax.swing.event.EventListenerList;

import controller.event.PushButtonActionEvent;
import controller.event.PushButtonActionListener;
import javafx.scene.Node;
import javafx.scene.control.Button;
import model.SynthParameter;

	/**
	 * a class that represents a push button on the front pane
	 * @author sydxrey
	 *
	 */
public class PushButton extends Control {
		
	private boolean pushed;
	private EventListenerList listenerList;
	private SynthParameter param;
		
	/** 
	 * @param label the button label
	 */
	public PushButton(String label){
		super();
		//this.param = param;
	}
	
	public boolean getState() {
        return pushed;
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
	 protected void firePushButtonActionEvent() {
		 
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

	@Override
	public Node getJavaFXView() {
		Button b = new Button();
		
		return b;
	} 


}
