package controller.component;

import java.io.IOException;
import java.util.logging.Logger;

import com.pi4j.io.gpio.PinState; // SR TODO : make this class independent from pi4j

import controller.event.RotaryEncoderChangeListener;
import controller.event.RotaryEncoderEvent;
import device.MCP23017;
import device.MCP23017.InterruptEvent;
import device.MCP23017.InterruptListener;

/**
 * A hardware quadratic encoder that can fire UP or DOWN change events upon rotation.
 * 
 * @author SR
 * 
 */
public class RotaryEncoder extends Control  {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MCP23017.Pin channelA; // pin GPIO entree A sur MCP23017
	private MCP23017.Pin channelB; // pin GPIO entree B sur MCP23017

	private PinState levelA; // dernier niveau logique enregistré sur entree A
	private PinState levelB; // dernier niveau logique enregistré sur entree B
	private Channel previousTriggeringChannel; // dernier port ayant changé d'état (soit gpioA soit gpioB ; pour le debounce)
	public static enum Channel {A,B};
	
	public static enum Direction {

		UP,
		DOWN;
	}
	
	/**
	 * Constructor for a rotary encoder connected to two input pins of the MCP23017 device.
	 * MC23017 inputs have pull-up enabled, so that encoder channels must connect inputs to ground.
	 * @param mcpDevice the MCP23017 device that connects the encoder pins ; if null only the UI simulator mode is used
	 * @pram portAorB the MCP23017 port encoder outputs are connected to 
	 * @param gpioA pin number for encoder output A
	 * @param gpioB pin number for encoder output B
	 * @throws IOException in case there's an I2C bus issue 
	 */
	public RotaryEncoder(String label, MCP23017 mcpDevice, MCP23017.Pin gpioA, MCP23017.Pin gpioB) throws IOException{

		super(label + "[" + gpioA + "&" + gpioB + "]");

		if (mcpDevice != null) {
			this.channelA = gpioA;
			this.channelB = gpioB;
			levelA=PinState.LOW;
			levelB=PinState.LOW;
			previousTriggeringChannel=null;
			mcpDevice.setInput(gpioA); // encoder
			mcpDevice.setInput(gpioB); // encoder
			mcpDevice.enablePullupResistor(gpioA); // pull up enabled
			mcpDevice.enablePullupResistor(gpioB); 
			mcpDevice.enableInterruptOnChange(gpioA); // enables GPIO input pin for interrupt-on-change
			mcpDevice.enableInterruptOnChange(gpioB); 
			mcpDevice.addInterruptListener(new PhysicalEncoderChangeListener());
		}
		//else LOGGER.warning("No MCP23017 registered for " + toString()+" -> simulator mode only");
	}
	
	
	/**
	 * Simulator mode constructor
	 * @param label the name of the encoder on the UI
	 */
	public RotaryEncoder(String label) {
		super(label);
		LOGGER.info("No MCP23017 registered for " + toString());
	}

	/**
	 * Adds the specified listener to receive action events from this encoder.
	 * @param l the listener
	 */
	public void addChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.add(RotaryEncoderChangeListener.class, l);
	 }

	/**
	 * Removes the specified listener so that it no longer receives action events from this encoder.
	 * @param l the listener that was previously added
	 */
	 public void removeChangeListener(RotaryEncoderChangeListener l) {
	     listenerList.remove(RotaryEncoderChangeListener.class, l);
	 }
	 
	/**
	 * Notify all listeners that have registered interest for
	 * notification on this event type.  The event instance
	 * is lazily created using the parameters passed into
	 * the fire method.
	 */
	 public void fireChangeEvent(Direction dir) { // TODO note SR : should be protected, but we have to make it public cause SwingMain uses it in simulator mode
		 
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
	 
	 /**
	  * 	Listens to changes event coming from a real encoder through the MCP23017 GPIO expander
	  * @author sydxrey
	  */
	 private class PhysicalEncoderChangeListener implements InterruptListener {
		 
		/**
		 * Callback lorsque la pin INTA du MCP23017 est asserted ; 
		 * signifie qu'une des pins du PORT A a changé, donc qu'un encodeur a tourné, mais pas forcément celui-ci !
		 * First item: vérifier que c'est bien cet encodeur qui a tourné ! 
		 */
		@Override
		public void interruptOccured(InterruptEvent event) {

			//System.out.println(event);

			// on cherche à savoir quel GPx du MCP23017 a bougé :
			Channel triggeringChannel; // quel canal a changé, A ou B ?
			if (event.getPin() == channelA) { // C'est A qui a changé
				triggeringChannel = Channel.A;
				levelA = event.getLevel();
			} else if (event.getPin() == channelB) { // C'est B qui a changé
				triggeringChannel = Channel.B;
				levelB = event.getLevel();
			}
			else return; // ok c'était pas pour nous !!!

			//System.out.printf("mcp23017_int_handler:\tINTF=%02X \t GPIO input = %02X\t gpioMCP23017=%d \t levelMCP32017=%d\n", intfRegister, captureRegister, gpioMCP23017, channelLevel); // also clears INTB flag ce qui va provoquer le re-appel du handler mais avec level=1

			// debounce code: si c'est le premier front sur le nouvel encodeur (A ou B) on execute le code suivant
			// car sinon c'est que c'est un rebond sur le meme GPIO (auquel cas on fait rien)
			// en d'autres termes, on ne prend en compte que le premier front sur tout nouvel encoder
			// CW : 11 -> 01 -> 00 -> 10
			// CCW : 11 -> 10 -> 00 -> 01

			if (triggeringChannel != previousTriggeringChannel){ // c'est pas un rebond !
				previousTriggeringChannel = triggeringChannel;
				if ((triggeringChannel == Channel.A) && (levelA == PinState.LOW) && (levelB==PinState.LOW)){ 
					//++position; // transition 10 -> 00
					fireChangeEvent(Direction.UP);
				}
				else if ((triggeringChannel == Channel.B) && (levelB == PinState.HIGH) && (levelA==PinState.HIGH)){ 
					//--position; // transition 10 -> 11
					fireChangeEvent(Direction.DOWN);
				}
				//System.out.println("pos="+position);
			}
		}	 
	 }

	
}
