package controller.component;

import java.io.IOException;

import com.pi4j.io.gpio.*;

import controller.event.*;
import device.MCP23017;
import device.MCP23017.InterruptEvent;
import device.MCP23017.InterruptListener;
import device.MCP23017.Port;

/**
 * a class that represents a push button on the front pane
 * 
 * @author SR
 *
 */
public class PushButton extends Control {

	private MCP23017.Pin buttonPin; 
	
	/**
	 * construct a pushbutton with the given UI label that is connected to the given pin of the given MCP23017 device and port
	 * @throws IOException 
	 */
	public PushButton(String label, MCP23017 mcpDevice, MCP23017.Pin gpio) throws IOException {
		super(label + "[" + gpio + "]");
		if (mcpDevice != null) {
			this.buttonPin = gpio;
			mcpDevice.enableIntPinsMirror(); // make sure intA and intB are connected
			mcpDevice.setInput(gpio); 
			mcpDevice.enablePullupResistor(gpio); // pull up enabled
			mcpDevice.enableInterruptOnChange(gpio); // enables GPIO input pin for interrupt-on-change
			mcpDevice.addInterruptListener(new PhysicalButtonChangeListener());
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
	public void firePushButtonActionEvent(PushButtonState state) { // note SR : should be protected, but we have to make it public cause SwingMain uses it in simumlator mode

		// Guaranteed to return a non-null array
		Object[] listeners = listenerList.getListenerList();

		// Process the listeners last to first, notifying
		// those that are interested in this event
		PushButtonActionEvent e = null;
		for (int i = listeners.length - 2; i >= 0; i -= 2) {
			if (listeners[i] == PushButtonActionListener.class) {
				// Lazily create the event:
				if (e == null)
					e = new PushButtonActionEvent(this, state);
				((PushButtonActionListener) listeners[i + 1]).actionPerformed(e); 
			}
		}
	}

	 /**
	  * Listens to changes event coming from a physical button through the MCP23017 GPIO expander
	  * @author sydxrey
	  */
	 private class PhysicalButtonChangeListener implements InterruptListener {
		 
		@Override
		public void interruptOccured(InterruptEvent event) {

			if (event.getPin() == buttonPin) {
				//System.out.println(event);
				firePushButtonActionEvent(event.getLevel() == PinState.HIGH ? PushButtonState.RELEASED : PushButtonState.PRESSED);
			}
			
		}	 
	 }	
	 
	 // ---------------- test --------------
	 
		public static void main(String[] args) throws Exception  {


			//for (int i : I2CFactory.getBusIds()) System.gpout.println(i);

			MCP23017 device = new MCP23017();
			device.registerRpiPinForReset(RaspiPin.GPIO_25);
			device.registerRpiPinForInterrupt(RaspiPin.GPIO_04);
			device.reset();
			device.printRegisters();
			
			PushButton pb = new PushButton("Push P4A", device, MCP23017.Pin.P4A);
			pb.addPushButtonActionListener(e -> System.out.println(e));
			
			pb = new PushButton("Push P7A", device, MCP23017.Pin.P7A);
			pb.addPushButtonActionListener(e -> System.out.println(e));

			pb = new PushButton("Push P0B", device, MCP23017.Pin.P0B);
			pb.addPushButtonActionListener(e -> System.out.println(e));

			pb = new PushButton("Push P3B", device, MCP23017.Pin.P3B);
			pb.addPushButtonActionListener(e -> System.out.println(e));

			int i=0;
			while ((i++)<100) {
				System.out.print(".");
				Thread.sleep(1000);
			}
			System.out.println("closing device");
			device.close();
		}	 
}
