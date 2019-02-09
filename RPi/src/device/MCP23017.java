package device;

import java.io.IOException;
import java.util.EventListener;
import java.util.EventObject;

import javax.swing.event.EventListenerList;

import com.pi4j.io.gpio.*;
import com.pi4j.io.gpio.event.*;
import com.pi4j.io.i2c.*;
import com.pi4j.io.i2c.I2CFactory.*;


import static device.MCP23017.Register.*;

/**
 * A class that represents the MCP23017 GPIO expander.
 *
 * GPB0	1	MCP23017		28	GPA7
 * GPB1	2				27	GPA6
 * GPB2	3				26	GPA5
 * GPB3	4				25	GPA4
 * GPB4	5				24	GPA3
 * GPB5	6				23	GPA2
 * GPB6	7				22	GPA1
 * GPB7	8				21	GPA0
 * Vdd	9				20	INTA
 * Vss	10				19	INTB
 * NC	11				18	/RESET
 * SCK	12				17	A2
 * SDA	13				16	A1
 * NC	14				15	A0
 * 
 * @author sydxrey
 *
 */
public class MCP23017 implements GpioPinListenerDigital {

	// -------------- fields --------------
	
	private I2CDevice i2cDevice; 
	private I2CBus i2cBus;
	
	/** a list of event listeners for this device */
	protected EventListenerList listenerList;
	
	// -------------- constructors --------------
	
	/**
	 * @throws IOException 
	 * @throws UnsupportedBusNumberException
	 * @address I2C device address 
	 * @intPin input pin of the RPi connected to the INTA pin of this MCP23017 device ; if null, no pin gets registered
	 */
	public MCP23017(DeviceAddress address, com.pi4j.io.gpio.Pin intPin) throws UnsupportedBusNumberException, IOException {
		
		i2cBus = I2CFactory.getInstance(I2CBus.BUS_1); 				
		i2cDevice = i2cBus.getDevice(address.getValue()); 
		listenerList = new EventListenerList();
		if (intPin != null) registerRpiPinForInterrupt(intPin);
		
	}
	
	/** no RPi interrupt line, argument sets I2C address */
	public MCP23017(DeviceAddress address) throws UnsupportedBusNumberException, IOException {
		this(address, null);
	}

	/** default 0x20 address, RPi interrupt line connected to intPin */
	public MCP23017(com.pi4j.io.gpio.Pin intPin) throws UnsupportedBusNumberException, IOException {
		this(DeviceAddress.ADR_000, intPin);
	}
	
	/** default 0x20 address, no RPi interrupt line */
	public MCP23017() throws UnsupportedBusNumberException, IOException {
		this(DeviceAddress.ADR_000, null);
	}

	/**
	 * close the corresponding I2C bus
	 * @throws IOException
	 */
	public void close() throws IOException {
		
		i2cBus.close();
		
	}
	
	// -------------- public and protected methods --------------
	
	/**
	 * set the pin direction for the given port (IODIR)
	 * call setInterruptOnChange() method with the same mask to make
	 * the corresponding inputs able to trigger interrupts. 
	 * @param port A or B
	 * @param mask 0 for OUTPUT pins, 1 for INPUT pins ; e.g. mask = Pin.or(P0, P2, P5) means 0, 2 and 5 are inputs, other are outputs
	 */
	public void setDirection(Port port, byte mask) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(IODIRA.getAddress(), mask); break;
			case B : i2cDevice.write(IODIRB.getAddress(), mask); break;
		}
	}			

	/**
	 * set all pins of the given port as outputs (IODIR)
	 * @param port A or B
	 */
	public void setOutput(Port port) throws IOException {
		
		byte mask = (byte)0x00;
		switch (port) {
			case A : i2cDevice.write(IODIRA.getAddress(), mask); break;
			case B : i2cDevice.write(IODIRB.getAddress(), mask); break;
		}
	}
	
	/**
	 * Set the given pins of the given port as outputs
	 * @param port
	 * @param pin
	 */
	public void setOutput(Port port, Pin... pins) throws IOException {
		Register reg;
		switch (port) {
			case A : reg = Register.IODIRA; break;
			case B : reg = Register.IODIRB; break;
			default : return;
		}
		int mask = Pin.andMask(pins); // resets every output bit in initial 0xFF
		mask &= i2cDevice.read(reg.getAddress());
		mask &= 0xFF;
		i2cDevice.write(reg.getAddress(), (byte)mask); 		
	}
	
	/**
	 * set all pins of the given port as inputs (IODIR)
	 * @param port A or B
	 */
	public void setInput(Port port) throws IOException {
		
		byte mask = (byte)0xFF;
		switch (port) {
			case A : i2cDevice.write(IODIRA.getAddress(), mask); break;
			case B : i2cDevice.write(IODIRB.getAddress(), mask); break;
		}
	}
	
	/**
	 * Set the given pins of the given port as inputs
	 * @param port
	 * @param pin
	 */
	public void setInput(Port port, Pin... pins) throws IOException {
		Register reg;
		switch (port) {
			case A : reg = Register.IODIRA; break;
			case B : reg = Register.IODIRB; break;
			default : return;
		}
		int mask = Pin.orMask(pins); // sets every input bit in initial 0x00
		mask |= i2cDevice.read(reg.getAddress());
		mask &= 0xFF;
		i2cDevice.write(reg.getAddress(), (byte)mask); 		
	}	

	/**
	 * set the pin polarity register for the given port (IPOL)
	 * @param port A or B
	 * @param mask 1 inverts pin values, 0 doesn't
	 */
	public void setPolarity(Port port, byte mask) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(IPOLA.getAddress(), mask); break;
			case B : i2cDevice.write(IPOLB.getAddress(), mask); break;
		}
	}

	/**
	 * set the "interrupt-on-change" (GPINTEN) behavior for the given port ; 
	 * should be coherent with the values of the IODIR register
	 * @param port A or B
	 * @param mask 1 enable int-on-change for the given (input) pin
	 */
	public void setInterruptOnChange(Port port, byte mask) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(GPINTENA.getAddress(), mask); break;
			case B : i2cDevice.write(GPINTENB.getAddress(), mask); break;
		}
		// clear interrupts:
		readInterruptCapturedRegister(MCP23017.Port.A);
	}

	/**
	 * set the "interrupt-on-change" (GPINTEN) behavior for the given port
	 * @param port A or B
	 * @param mask 1 enable int-on-change for the given (input) pin
	 */
	public void setInterruptOnChange(Port port, boolean enableInterrupt) throws IOException {
		
		byte mask;
		if (enableInterrupt) mask = (byte)0xFF;
		else mask = (byte)0x00;
		switch (port) {
			case A : i2cDevice.write(GPINTENA.getAddress(), mask); break;
			case B : i2cDevice.write(GPINTENB.getAddress(), mask); break;
		}
		// clear interrupts:
		readInterruptCapturedRegister(MCP23017.Port.A);
	}
	
	/**
	 * set the default value register (DEFVAL) to be compared to the 8 inputs of the given port when "interrupt-on-change" behavior is enabled 
	 * @param port A or B
	 * @param value register value
	 */
	public void writeDefaultCompareRegister(Port port, byte value) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(DEFVALA.getAddress(), value); break;
			case B : i2cDevice.write(DEFVALB.getAddress(), value); break;
		}
	}

	/**
	 * controls how the associated pin value is compared for the interrupt-on-change feature. 
	 * @param port A or B
	 * @param mask is 1, the corresponding I/O pin is compared against the associated bit in the DEFVAL register ; against the previous value otherwise 
	 */
	public void writeInterruptControlRegister(Port port, byte mask) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(INTCONA.getAddress(), mask); break;
			case B : i2cDevice.write(INTCONB.getAddress(), mask); break;
		}
	}

	/**
	 * enables a 100k pull-up resistor for each pin configured as input 
	 * @param port A or B
	 * @param mask if 1, the corresponding pull-up resistor is enabled 
	 */
	public void setPullupResistors(Port port, byte mask) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(GPPUA.getAddress(), mask); break;
			case B : i2cDevice.write(GPPUB.getAddress(), mask); break;
		}
	}
	
	/**
	 * activate pullup resistors for all pins of the given port
	 * @param port A or B
	 */
	public void setPullupResistors(Port port, boolean enablePullup) throws IOException {
		
		byte mask;
		if (enablePullup) mask = (byte)0xFF;
		else mask = (byte)0x00;
		switch (port) {
			case A : i2cDevice.write(GPPUA.getAddress(), mask); break;
			case B : i2cDevice.write(GPPUB.getAddress(), mask); break;
		}
	}	
	/**
	 * returns the register (INTF) that reflects the interrupt condition on the port pins of 
	 * any pin that is enabled for interrupts via the GPINTEN register. A 1 indicated that
	 * the corresponding pin caused the interrupt.
	 * @param port A or B
	 */
	public int readInterruptFlagRegister(Port port) throws IOException {
		
		switch (port) {
			case A : return i2cDevice.read(INTFA.getAddress());
			case B : return i2cDevice.read(INTFB.getAddress());
		}
		return 0; // unreachable
	}	

	/**
	 * returns the register (INTCAP) that captured the the GPIO port value at the 
	 * time the interrupt occurred. Also CLEARS pending interrupts!
	 * @param port A or B
	 */
	public int readInterruptCapturedRegister(Port port) throws IOException {
		
		switch (port) {
			case A : return i2cDevice.read(INTCAPA.getAddress());
			case B : return i2cDevice.read(INTCAPB.getAddress());
		}
		return 0; // unreachable
	}
	
	/**
	 * clear interrupt flag register
	 */
	public void clearInterrupts(Port port) throws IOException {
		while(readInterruptFlagRegister(port) != 0) readInterruptCapturedRegister(port);			
	}
	
	/** The INT pins are internally connected */
	public void enableIntPinsMirror() throws IOException {
		
		int iocon = i2cDevice.read(IOCON.getAddress());
		iocon |= IOCONFields.MIRROR.getMask(); // set corresponding bit to 1
		iocon &= 0xFF;
		i2cDevice.write(IOCON.getAddress(), (byte)iocon);
	}

	/**
	 * returns the register (GPIO) that reflects the value on the port.
	 * Reading from this register reads the port.
	 * @param port A or B
	 */
	public int read(Port port) throws IOException {
		
		switch (port) {
			case A : return i2cDevice.read(GPIOA.getAddress());
			case B : return i2cDevice.read(GPIOB.getAddress());
		}
		return 0; // unreachable
	}		
	
	/**
	 * writes the register (GPIO) that reflects the value on the port.
	 * Writing to this register modifies the Output Latch (OLAT) register.
	 * @param port A or B
	 */
	public void write(Port port, byte mask) throws IOException {
		
		switch (port) {
			case A : i2cDevice.write(GPIOA.getAddress(), mask); break;
			case B : i2cDevice.write(GPIOB.getAddress(), mask); break;
		}
	}		
	
	/**
	 * Write the given value (false=LOW or true=HIGH) to the given pin
	 */
	public void write(Port port, Pin pin, boolean value) throws IOException{

	  int bit  = pin.getMask();
	  int old = read(port);
	  if (value == false)
	    old &= (~bit) ;
	  else
	    old |=   bit ;
	  old &= 0xFF; // filters out LSBs
	  write(port, (byte)old);
	}
	
	/**
	 * Toggle the value (false=LOW or true=HIGH) of the given pin
	 */
	public void toggle(Port port, Pin pin) throws IOException{

	  boolean old = read(port, pin);
	  write(port, pin, !old);
	  
	}	
	
	/**
	 * Read the value of the given pin for the given port
	 */
	public boolean read(Port port, Pin pin) throws IOException{

	  return (read(port) & pin.getMask()) != 0;
	}
	
	/**
	 * display the value of all registers
	 */
	public void printRegisters() {
		
		try {
			System.out.println("MCP23017 REGISTERS :");
			for (Register r : Register.values()) {
				System.out.printf("\t" + r +"\t%02X\n", i2cDevice.read(r.getAddress()));
			}

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
	/**
	 * Registers the given Raspberry Pi as being the input pin connected to the INTA pin of this MCP23017 device, so that 
	 * the RPi get informed whenever an interrupt occurs on this MCP23017 device.
	 * @param intPin
	 * TODO : set IOCON.MIRROR=1 so that the INTn pins are functionally OR’ed (i.e. an interrupt on either port will cause both pins to activate)
	 */
	public void registerRpiPinForInterrupt(com.pi4j.io.gpio.Pin intPin) {
		 
		// provision a RPi gpio pin as an input pin with its internal pull up resistor enabled
		final GpioPinDigitalInput mcp23017IntPin = GpioFactory.getInstance().provisionDigitalInputPin(intPin, PinPullResistance.PULL_UP);

		// create and register gpio pin listener
		mcp23017IntPin.addListener(this);
		
	}
		
	/**
	 * Callback when either the INTA or INTB pin of this MCP23017 device has been asserted (=FALLING transition), 
	 * which in turns means that at least one of the pins of the device (port A or B) has changed
	 */
	@Override
	public void handleGpioPinDigitalStateChangeEvent(GpioPinDigitalStateChangeEvent event) {

		//System.out.println(event + " ; edge=" + event.getEdge());
		if (event.getEdge() == PinEdge.RISING) return; // INT clear!
		
		try {
			Port port = Port.A;
			int intfRegister = readInterruptFlagRegister(Port.A); // also clears interrupt
			int captureRegister = readInterruptCapturedRegister(Port.A); // valeurs capturées sur le Port A au moment de l'interruption
			clearInterrupts(Port.A);
			if (intfRegister==0) { // it's not on Port.A
				intfRegister = readInterruptFlagRegister(Port.B); 
				captureRegister = readInterruptCapturedRegister(Port.B);
				clearInterrupts(Port.B);
				port = Port.B;
				/*if (intfRegister ==0) throw new IOException("there's a bug?");
				else System.out.println("MCP23017 int on port B");*/
			}		
			//else System.out.println("MCP23017 int on port A");
			Pin pin = Pin.getPinFromMask((byte)intfRegister);
			if (pin == null) return; // means no PIN found! (happens when encoder turn very quickly, but why???)
			PinState lvl =  (captureRegister & pin.getMask()) != 0 ? PinState.HIGH : PinState.LOW;
			fireInterruptEvent(port, pin, lvl);
			System.out.println("Int-event: port="+port + " pin="+pin+" level="+lvl);
		} catch (IOException e) {
			e.printStackTrace();
		} 		
	}
	
	// -------------- private or package methods --------------


	
	// -------------- enums and inner classes --------------
	
	// an enum for the two ports of this MCP23017 device
	public static enum Port {
		A, B;
	}
		
	// an enum for the GPIO pin directions of this MCP23017 device
	public static enum PinDirection {
		OUTPUT, INPUT;
	}
	
	/**
	 * An enumeration of pin masks (valid for both ports) for configuring registers
	 */
	public static enum Pin {
		
		P0(0x01),
		P1(0x02),
		P2(0x04),
		P3(0x08),
		P4(0x10),
		P5(0x20),
		P6(0x40),
		P7(0x80);
		
		private int mask; 
		
		Pin(int mask){
			this.mask = mask;
		}
		
		public byte getMask() {
			return (byte)mask;
		}		
		
		// 1 for every pin in the arg list, 0 otherwise
		public static byte orMask(Pin ... pins) {
			int m = 0; // initial mask
			for (Pin pin : pins) {
				m |= pin.mask;
			}
			return (byte)m;
		}

		// 0 for every pin in the arg list, 1 otherwise
		public static byte andMask(Pin ... pins) {
			int m = 0xFF; // initial mask
			for (Pin pin : pins) {
				m &= ~pin.mask;
			}
			m &= 0xFF; // remove LSBs
			return (byte)m;
		}
		
		// return the lowest Pin that matches the given mask
		public static Pin getPinFromMask(byte mask) {
			for (Pin pin : Pin.values())
				if ((pin.getMask() & mask) != 0) return pin;
			return null;
			
		}
	}
	
	
	/**
	 * An enumeration of possible I2C device addresses depending on the connection of the "A2.A1.A0" pins
	 * @author SR
	 */
	public static enum DeviceAddress {
		
		ADR_000(0x20), 
		ADR_001(0x21), 
		ADR_002(0x22),
		ADR_003(0x23),
		ADR_004(0x24),
		ADR_005(0x25),
		ADR_006(0x26),
		ADR_007(0x27);

		private int address; 
		
		DeviceAddress(int address){
			this.address = address;
		}
		
		public int getValue() {
			return address;
		}
	}
		
	
	/**
	 * An enumeration of registers whenever IOCON.BANK=0 (see datasheet page 17)
	 * @author SR
	 */
	public static enum Register {
		
		/**  */
		IODIRA(0x00),
		IPOLA(0x02),
		GPINTENA(0x04),
		DEFVALA(0x06),
		INTCONA(0x08),
		IOCON(0x0A),
		GPPUA(0x0C),
		INTFA(0x0E),
		INTCAPA(0x10),
		GPIOA(0x12),
		OLATA(0x14),
		IODIRB(0x01),
		IPOLB(0x03),
		GPINTENB(0x05),
		DEFVALB(0x07),
		INTCONB(0x09),
		// IOCON(0x0B), same as 0x0A
		GPPUB(0x0D),
		INTFB(0x0F),
		INTCAPB(0x11),
		GPIOB(0x13),
		OLATB(0x15);	
		
		
		private int address; 
		
		Register(int address){
			this.address = address;
		}
		
		public byte getAddress() {
			return (byte)address;
		}
	}	

	/**
	 * an enumeration of available parameters for the IOCON register
	 * @author SR
	 */
	public static enum IOCONFields{
		
		/** Controls how the registers are addressed
		 * 1 = The registers associated with each port are separated into different banks. 
		 * 0 = The registers are in the same bank (addresses are sequential). */
		BANK(0x80),
		
		/** INT Pins Mirror bit
		 * 1 = The INT pins are internally connected
		 * 0 = The INT pins are not connected. INTA is associated with PORTA and INTB is associated with PORTB */
		MIRROR(0x40),
		
		/** Sequential Operation mode bit
		 * 1 = Sequential operation disabled, address pointer does not increment. 
		 * 0 = Sequential operation enabled, address pointer increments. */
		SEQOP(0x20),
		
		/** Slew Rate control bit for SDA output 
		 * 1 = Slew rate disabled
		 * 0 = Slew rate enabled */
		DISSLW(0x10),
		
		// not implemented: HAEN (SPI version only)
		
		/** Configures the INT pin as an open-drain output
		 * 1 = Open-drain output (overrides the INTPOL bit.)
		 * 0 = Active driver output (INTPOL bit sets the polarity.) */
		ODR(0x04),
		
		/** This bit sets the polarity of the INT output pin 
		 * 1 = Active-high
		 * 0 = Active-low*/
		INTPOL(0x02);
		
		// not implemented: bit 0
		
		private int mask; 
		
		IOCONFields(int mask){
			this.mask = mask;
		}
		
		public int getMask() {
			return mask;
		}		
	}
	
	/**
	 * The listener interface for receiving interrupt events from an MCP23017 device. 
	 */
	public static interface InterruptListener extends EventListener {
		
		/**
		 * Invoked when one of the port of the device has a pin in input mode, and an interrupt was 
		 * triggered either on a line INTA or INTB
		 * @param e
		 */
		public void interruptOccured(InterruptEvent e);
		
	}
	
	/**
	 * A low-level event that indicates that an interrupt was generated by this MCP23017 device
	 * @author sydxrey
	 */
	public class InterruptEvent extends EventObject {
		
		private static final long serialVersionUID = 1L;
		private Port port;
		private Pin pin;
		private PinState level;
		
		public InterruptEvent(Port port, Pin pin, PinState level) {
			super(MCP23017.this);
			this.port = port;
			this.pin = pin;
			this.level = level;
		}
		
		/** @return the port on which the change that triggered the interrupt occured */
		public Port getPort() {
			return port;
		}
		
		/** @return the pin on which the change that triggered the interrupt occured */
		public Pin getPin() {
			return pin;
		}
		
		public PinState getLevel() {
			return level;
		}

		@Override
		public String toString() {
			return super.toString() + " port="+port + " pin="+pin+" level="+level;
		}
	
	}
	
	/**
	 * Adds the specified listener to receive interrupt events from this MCP23017 device.
	 * @param l the listener
	 */
	public void addInterruptListener(InterruptListener l) {
	     listenerList.add(InterruptListener.class, l);
	 }

	/**
	 * Removes the specified listener so that it no longer receives interrupt events from this MCP23017 device.
	 * @param l the listener that was previously added
	 */
	 public void removeInterruptListener(InterruptListener l) {
	     listenerList.remove(InterruptListener.class, l);
	 }
	 
	/**
	 * Notify all listeners that have registered interest for
	 * notification on this event type.  The event instance
	 * is lazily created using the parameters passed into
	 * the fire method.
	 */
	 protected void fireInterruptEvent(Port port, Pin pin, com.pi4j.io.gpio.PinState level) {
		 
	     // Guaranteed to return a non-null array
	     Object[] listeners = listenerList.getListenerList();
	     
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     InterruptEvent e = null;
	     for (int i = listeners.length-2; i>=0; i-=2) {
	         if (listeners[i]==InterruptListener.class) {
	             // Lazily create the event:
	             if (e == null) e = new InterruptEvent(port, pin, level);
	             ((InterruptListener)listeners[i+1]).interruptOccured(e); 
	         }
	     }
	 }
	
	
	
	// -------------- test methods --------------
	
	public static void main(String[] args) throws Exception  {


		//for (int i : I2CFactory.getBusIds()) System.out.println(i);

		MCP23017 device = new MCP23017();

		if (args.length == 0) {
			device.printRegisters();
			device.close();
			System.exit(0);
		}

		
		device.setInput(MCP23017.Port.A);
		device.setPullupResistors(MCP23017.Port.A, true);
		device.setInterruptOnChange(Port.A, true);
		device.registerRpiPinForInterrupt(RaspiPin.GPIO_27);
		device.addInterruptListener(e -> System.out.println("mylistener -> " + e));
				
		device.printRegisters();

		//System.out.println("IODIRA_REGISTER="+device.read(IODIRA_REGISTER));

		//device.write(IODIRB_REGISTER, (byte) 0x00);

		//System.out.println("IOCON REG="+device.read(IOCON_REGISTER)); 


		//device.write(GPPUA_REGISTER, (byte)0xFF); // Port A pull up enabled (le bouton doit connecter le port a la masse)
		//System.out.println("GPPUA_REGISTER="+device.read(GPPUA_REGISTER)); 

		//device.write(GPINTENA_REGISTER,	0xFF); // Port A : enables GPIO input pin for interrupt-on-change
		//System.out.println("MCP23017: port A en input + pull-up + interrupt enabled (encoders sur 0/1 + 2/3 + 4/5 + 6/7)\n");

		//System.out.println("GPIOA REG=" + device.read(GPIOA_REGISTER)); 

		//device.write(MCP23017.Port.A, (byte)0xFF);


		int counter=0; 
		while(counter<10){
				//device.toggle(Port.A, Pin.P7);
				System.out.println(counter++);
				Thread.sleep(1000);
		}
			 
		device.close();



	}
	
	
}
