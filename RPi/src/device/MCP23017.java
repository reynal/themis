package device;

import java.io.*;
import java.util.*;
import java.util.logging.Logger;

import javax.swing.event.*;

import com.pi4j.io.gpio.*;
import com.pi4j.io.gpio.event.*;
import com.pi4j.io.i2c.*;
import com.pi4j.io.i2c.I2CFactory.*;


import static device.MCP23017.Register.*;

/**
 * A class that represents the MCP23017 GPIO expander. 
 * If in addition one of the INT pins of the device is 
 * connected to a Raspberry pin, then we can even track level changes occuring
 * on pins of the MCP23017 device.  
 *
 * GPB0	1	MCP23017	28	GPA7
 * GPB1	2				27	GPA6
 * GPB2	3				26	GPA5
 * GPB3	4				25	GPA4
 * GPB4	5				24	GPA3
 * GPB5	6				23	GPA2
 * GPB6	7				22	GPA1
 * GPB7	8				21	GPA0
 * Vdd	9				20	INTA
 * Vss	10				19	INTB
 * NC	11				18	/RESET (default: connected to RPi Pin 37 = GPIO.25 in wPi numbering scheme)
 * SCK	12				17	A2
 * SDA	13				16	A1
 * NC	14				15	A0
 *
 * 
 * @author sydxrey
 *
 */
public class MCP23017  {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	// -------------- fields --------------
	
	private I2CDevice i2cDevice; 
	private I2CBus i2cBus;
	private GpioPinDigitalOutput mcp23017RstPin;
	private GpioPinDigitalInput mcp23017IntPin; 
	
	/** a list of event listeners for this device */
	protected EventListenerList listenerList;
	
	// TODO reynal move elsewhere:
	public final static com.pi4j.io.gpio.Pin DEFAULT_RST_PIN = RaspiPin.GPIO_25; // pin 37 
	public final static com.pi4j.io.gpio.Pin DEFAULT_INT_PIN = RaspiPin.GPIO_04; // pin 16
	public final static DeviceAddress DEFAULT_I2C_ADDRESS = DeviceAddress.ADR_000;
	
	//int interruptCounter = 0; // for debugging spurious interrupts
	
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
		this(DEFAULT_I2C_ADDRESS, intPin);
	}
	
	/** default 0x20 address, no RPi interrupt line */
	public MCP23017() throws UnsupportedBusNumberException, IOException {
		this(DEFAULT_I2C_ADDRESS, null);
	}

	/**
	 * close the I2C bus to which this device is connected and unprovision RST and INT RPi pins.
	 * @throws IOException
	 */
	public void close() throws IOException {
		
		GpioFactory.getInstance().unprovisionPin(mcp23017RstPin);
		GpioFactory.getInstance().unprovisionPin(mcp23017IntPin);
		mcp23017IntPin = null;
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
	public void setOutput(Pin pin) throws IOException {
		Register reg;
		switch (pin.getPort()) {
			case A : reg = Register.IODIRA; break;
			case B : reg = Register.IODIRB; break;
			default : return;
		}
		int mask = Pin.andMask(pin); // resets every output bit in initial 0xFF
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
	public void setInput(Pin pin) throws IOException {
		Register reg;
		switch (pin.getPort()) {
			case A : reg = Register.IODIRA; break;
			case B : reg = Register.IODIRB; break;
			default : return;
		}
		int mask = Pin.orMask(pin); // sets every input bit in initial 0x00
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
			case A : i2cDevice.write(INTENA.getAddress(), mask); break;
			case B : i2cDevice.write(INTENB.getAddress(), mask); break;
		}
		// clear interrupts:
		clearInterrupts(port);
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
			case A : i2cDevice.write(INTENA.getAddress(), mask); break;
			case B : i2cDevice.write(INTENB.getAddress(), mask); break;
		}
		// clear interrupts:
		clearInterrupts(port);
	}
	
	/**
	 * enables the "interrupt-on-change" (GPINTEN) behavior for the given pin
	 * @param pin
	 */
	public void enableInterruptOnChange(Pin pin) throws IOException {
		Register reg;
		switch (pin.getPort()) {
			case A : reg = Register.INTENA; break;
			case B : reg = Register.INTENB; break;
			default : return;
		}
		int mask = pin.getMask(); 
		mask |= i2cDevice.read(reg.getAddress());
		mask &= 0xFF;
		i2cDevice.write(reg.getAddress(), (byte)mask); 		
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
	 * enables a 100k pull-up resistor for the given pin
	 * @param pin
	 */
	public void enablePullupResistor(Pin pin) throws IOException {
		Register reg;
		switch (pin.getPort()) {
			case A : reg = Register.GPPUA; break;
			case B : reg = Register.GPPUB; break;
			default : return;
		}
		int mask = pin.getMask(); 
		mask |= i2cDevice.read(reg.getAddress());
		mask &= 0xFF;
		i2cDevice.write(reg.getAddress(), (byte)mask); 		
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
	 * returns the register (INTCAP) that captured the GPIO port value at the 
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
	 * clear interrupt flag register by reading the INTCAPx register (see datasheet p 23)
	 * TODO : add some timeout feature
	 */
	public void clearInterrupts(Port port) throws IOException {
		while(this.mcp23017IntPin.isLow()) readInterruptCapturedRegister(port);
	}
	
	public void clearInterrupts() throws IOException {
		
		while(this.mcp23017IntPin.isLow()) {
			readInterruptCapturedRegister(Port.A);
			readInterruptCapturedRegister(Port.B);
		}
	}
	
	/** 
	 * Both INT pins (i.e., INTA and INTB) are internally OR'd and the result is available on INTA output pin
	 * In practice, this implies that both ports A and B trigger the same interrupt line (although they still
	 * have separate INT flag registers) 
	 * */
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
	public void write(Pin pin, boolean value) throws IOException{

	  int bit  = pin.getMask();
	  int old = read(pin.getPort());
	  if (value == false)
	    old &= (~bit) ;
	  else
	    old |=   bit ;
	  old &= 0xFF; // filters out LSBs
	  write(pin.getPort(), (byte)old);
	}
	
	/**
	 * Toggle the value (false=LOW or true=HIGH) of the given pin
	 */
	public void toggle(Pin pin) throws IOException{

	  boolean old = read(pin);
	  write(pin, !old);
	  
	}	
	
	/**
	 * Read the value of the given pin
	 */
	public boolean read(Pin pin) throws IOException{

	  return (read(pin.getPort()) & pin.getMask()) != 0;
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
	 * display the value of most useful registers for port A
	 */
	public void printRegistersBriefA() {
		
		try {
			System.out.printf("\n\t" + INTFA +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(INTFA.getAddress())));
			System.out.printf("\t" + GPIOA +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(GPIOA.getAddress())));
			System.out.printf("\t" + INTCAPA +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(INTCAPA.getAddress())));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * display the value of most useful registers for port A
	 */
	public void printRegistersBriefB() {
		
		try {
			System.out.printf("\n\t" + INTFB +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(INTFB.getAddress())));
			System.out.printf("\t" + GPIOB +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(GPIOB.getAddress())));
			System.out.printf("\t" + INTCAPB +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(INTCAPB.getAddress())));
		} catch (IOException e) {
			e.printStackTrace();
		}

	}	
	
	/**
	 * Registers the given Raspberry Pi as being the input pin connected to the INTA output pin of this MCP23017 device, so that 
	 * the RPi get informed whenever an interrupt occurs on this MCP23017 device.
	 * GpioDigitalInputPin get provisioned here, so make sure you don't reuse a previously provisioned pin!
	 * @param intPin
	 * TODO : set IOCON.MIRROR=1 so that the INTn pins are functionally OR’ed (i.e. an interrupt on either port will cause both pins to activate)
	 */
	public void registerRpiPinForInterrupt(com.pi4j.io.gpio.Pin intPin) {
		 
		// provision a RPi gpio pin as an input pin with its internal pull up resistor enabled
		mcp23017IntPin = GpioFactory.getInstance().provisionDigitalInputPin(intPin, PinPullResistance.PULL_UP);

		// create and register gpio pin listener
		mcp23017IntPin.addListener(new INTPinChangeListener());
		
		Thread t = new Thread(() -> {
			try {
				while(mcp23017IntPin != null) {
					clearInterrupts();
					//System.out.print(".");
					Thread.sleep(100);
				}
			} 
			catch (InterruptedException | IOException e) {e.printStackTrace();} 
		});
		t.start();
		
	}
		
	/**
	 * Registers the given Raspberry Pi as being the output pin connected to the RST pin of this MCP23017 device, so that 
	 * the RPi can reset the device. Same pin can be reused to reset different MCP devices as this is an output pin.
	 * @param rstPin
	 */
	public void registerRpiPinForReset(com.pi4j.io.gpio.Pin rstPin) {
		 
		// check if this pin has already bin registered:
		GpioPin gpio = GpioFactory.getInstance().getProvisionedPin(rstPin);
		if (gpio == null) mcp23017RstPin = GpioFactory.getInstance().provisionDigitalOutputPin(rstPin);
		else if (gpio instanceof GpioPinDigitalOutput) mcp23017RstPin = (GpioPinDigitalOutput)gpio; // reuse already provisioned pin
		else { // not an Output (maybe an input or a multipurpose) => reprovision as output
			GpioFactory.getInstance().unprovisionPin(gpio);
			mcp23017RstPin = GpioFactory.getInstance().provisionDigitalOutputPin(rstPin);
		}
		
		mcp23017RstPin.high();
		
		
	}
	
	/**
	 * Reset this device provided a RPi pin connected to the RST pin of this device was previously registered
	 */
	public void reset() {
		
		if (mcp23017RstPin != null) {
			mcp23017RstPin.low();
			try {
				Thread.sleep(1); // 1ms
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			mcp23017RstPin.high();
		}
		else LOGGER.severe("[ERROR] No RPi pin registered for PCM23017 RST");
		
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
	 * An enumeration of pin masks for configuring registers
	 */
	public static enum Pin {
		
		// port A
		P0A(0x01, Port.A),
		P1A(0x02, Port.A),
		P2A(0x04, Port.A),
		P3A(0x08, Port.A),
		P4A(0x10, Port.A),
		P5A(0x20, Port.A),
		P6A(0x40, Port.A),
		P7A(0x80, Port.A),
		// port B
		P0B(0x01, Port.B),
		P1B(0x02, Port.B),
		P2B(0x04, Port.B),
		P3B(0x08, Port.B),
		P4B(0x10, Port.B),
		P5B(0x20, Port.B),
		P6B(0x40, Port.B),
		P7B(0x80, Port.B);
		
		private int mask; 
		private Port port;
		
		Pin(int mask, Port port){
			this.mask = mask;
			this.port = port;
		}
		
		public Port getPort() {
			return port;
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
		
		// return all the Pins that matche the given mask
		public static List<Pin> getPinListFromMask(byte mask, Port port) {
			ArrayList<Pin> list = new ArrayList<Pin>();
			for (Pin pin : Pin.values())
				if ((pin.getMask() & mask) != 0 && pin.getPort()==port) list.add(pin);
			return list;
			
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
		INTENA(0x04),
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
		INTENB(0x05),
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
	 * @author reynal
	 */
	public class InterruptEvent extends EventObject {
		
		private static final long serialVersionUID = 1L;
		private Pin pin;
		private PinState level;
		
		public InterruptEvent(Pin pin, PinState level) {
			super(MCP23017.this);
			this.pin = pin;
			this.level = level;
		}
		
		/** @return the port on which the change that triggered the interrupt occured */
		public Port getPort() {
			return pin.getPort();
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
			return super.toString() + " pin="+pin+" level="+level;
		}
	
	}
	
	/**
	 * A Pi4J listener that forwards changes on the device INTA/B pin to registered MCP23017 GPIO ports change listeners.
	 * @author reynal
	 *
	 */
	private class INTPinChangeListener implements GpioPinListenerDigital {

		/**
		 * Callback when either the INTA or INTB pin of this MCP23017 device has been asserted (=FALLING transition), 
		 * which in turns means that at least one of the pins of the device (port A or B) has changed
		 */
		@Override
		public void handleGpioPinDigitalStateChangeEvent(GpioPinDigitalStateChangeEvent event) {
			

			if (event.getEdge() == PinEdge.RISING) {
				//System.out.println("[handle...event #"+interruptCounter+"] INT RISING EDGE");
				/*try {
					System.out.printf("\t[handle...event #"+interruptCounter+"] " + INTFA +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(INTFA.getAddress())));
					System.out.printf("\t[handle...event #"+interruptCounter+"] " + INTFB +"\t%8s\n", Integer.toBinaryString(i2cDevice.read(INTFB.getAddress())));
				} catch (IOException e) { e.printStackTrace();} */
				return; // MCP23017's INT clear!
			}
			/* else if (event.getEdge() == PinEdge.FALLING) {
				interruptCounter++;
				System.out.println("[handle...event #"+interruptCounter+"] INT FALLING EDGE");
			} */
			//printRegistersBriefA();
			//printRegistersBriefB();
			
			try {
				for (Port port: Port.values()) {
					int intFlagsRegister = readInterruptFlagRegister(port);
					if (intFlagsRegister == 0) continue; // next port
					int captureRegister = readInterruptCapturedRegister(port); // this will clear interrupts and re-trigger a call to this listener
					//clearInterrupts(port); // SR : i don't think this is really useful
					//System.out.printf("\n------------------------- Port %s -------------------------\n FLAGS: %8s \t CAPTURE: %8s\n", port.toString(), Integer.toBinaryString(intfRegister), Integer.toBinaryString(captureRegister));
					for (Pin pin : Pin.getPinListFromMask((byte)intFlagsRegister, port)) {
						PinState lvl =  (captureRegister & pin.getMask()) != 0 ? PinState.HIGH : PinState.LOW;
						fireInterruptEvent(pin, lvl);
					}
					
				}
			} catch (IOException e) { e.printStackTrace(); } 		
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
	 protected void fireInterruptEvent(Pin pin, com.pi4j.io.gpio.PinState level) {
		 
		 //System.out.println("[fire...event#"+interruptCounter+"]");
	     // Guaranteed to return a non-null array
	     Object[] listeners = listenerList.getListenerList();
	     
	     // Process the listeners last to first, notifying
	     // those that are interested in this event
	     InterruptEvent e = null;
	     for (int i = listeners.length-2; i>=0; i-=2) {
	         if (listeners[i]==InterruptListener.class) {
	             // Lazily create the event:
	             if (e == null) e = new InterruptEvent(pin, level);
	             ((InterruptListener)listeners[i+1]).interruptOccured(e); 
	         }
	     }
	 }
	
	
	
	// -------------- test methods --------------
	
	public static void main(String[] args) throws Exception  {


		//for (int i : I2CFactory.getBusIds()) System.gpout.println(i);

		//MCP23017 device = new MCP23017();
		MCP23017 device = new MCP23017(DeviceAddress.ADR_000, RaspiPin.GPIO_04);
		//MCP23017 device = new MCP23017(DeviceAddress.ADR_001, RaspiPin.GPIO_05);

		device.registerRpiPinForReset(DEFAULT_RST_PIN); // pin 37
		device.reset();
		device.printRegisters();
		
		device.enableIntPinsMirror();
		device.setInput(Port.A);
		device.setInput(Port.B);
		device.setPullupResistors(Port.A, true);
		device.setPullupResistors(Port.B, true);
		//device.setInterruptOnChange(Port.A, true);
		device.setInterruptOnChange(Port.B, true);
		//device.addInterruptListener(e -> System.out.println("[main #"+ device.interruptCounter +"] INT occured: " + e));
		device.addInterruptListener(e -> System.out.println(e));
		device.clearInterrupts();
		device.printRegisters();

		int i=0;
		while ((i++)<120) {
			//System.out.printf("INTFA: %02X \t GPIOA: %02X\n", device.readInterruptFlagRegister(Port.A), device.read(Port.A)); //, device.read(Port.B));
			//System.out.printf("A: %02X \t B: %02X\n", device.read(Port.A), device.read(Port.B));
			//System.out.print(i+" ");
			//System.out.print(".");
			//System.out.println("#" + device.interruptCounter + " : " + (device.mcp23017IntPin.isHigh() ? "INT HIGH" : "INT LOW"));
			//System.out.println("INTFA="+device.readInterruptFlagRegister(Port.A));
			//System.out.println("INTFB="+device.readInterruptFlagRegister(Port.B));
			//device.clearInterrupts();
			//device.printRegistersBriefA();
			//device.printRegistersBriefB();			
			Thread.sleep(1000);
		}
		System.out.println("closing device");
		device.close();
	}
}
