package model.serial;

import java.io.IOException;
import java.util.Random;
import java.util.logging.Logger;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.ShortMessage;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import application.Preferences;
import model.MidiCCImplementation;

/**
 * This class acts as a MIDI over UART transmitter to any listening device.
 * It also allows bidirectionnal communication with the listening device 
 * by displaying received data in the console.
 * @author sydxrey
 *
 */
public class UartTransmitter extends AbstractSerialTransmitter {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	SerialPort currentPort;
	public static final int DEFAULT_BAUD_RATE = 115200;
	//public static final int[] BAUD_RATES = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
	
	/**
	 * 
	 * @param s
	 * @param openConsole
	 * @throws IOException
	 */
	public UartTransmitter(SerialPort s) throws IOException{

		if (s==null) return;
		currentPort = s;
		currentPort.setBaudRate(DEFAULT_BAUD_RATE);
		if (!currentPort.openPort())
			throw new IOException("UART opening failed");
		LOGGER.info("Opening UART on " 
				+ currentPort.getSystemPortName()
				+ " at " + currentPort.getBaudRate() + " bauds, "
				+ (currentPort.getParity()==SerialPort.NO_PARITY ? "No parity " : "Parity ")
				+ currentPort.getNumDataBits() + " databits, "
				+ currentPort.getNumStopBits() + " stopbits");
		
		currentPort.addDataListener(new UARTDataListener());
	}	
		
	/**
	 * Initialize the SPI bus with the correct speed and parameter, using Pi4J library.
	 * @throws IOException 
	 */
	public UartTransmitter() throws IOException {
		this(SerialPort.getCommPort(Preferences.getPreferences().getStringProperty(Preferences.Key.USB_DEV)));
		
	}
	
	@Override
	public void transmitMidiMessage(ShortMessage sm) throws IOException {
		
		if (currentPort != null && currentPort.isOpen()) {
			int n = currentPort.writeBytes(sm.getMessage(), sm.getMessage().length);
			LOGGER.info("Sending "+n+" bytes over " + currentPort.getSystemPortName() + " : " + sm.getCommand() + " " + sm.getData1() + " " + sm.getData2());
		}
		else LOGGER.severe("Connection not open");
		
	}

	@Override
	public void close() {
		currentPort.closePort();	
		LOGGER.info("Closing serial port " + currentPort);
	}

	class UARTDataListener implements SerialPortDataListener {
		
		@Override
		public int getListeningEvents() {
			return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
		}

		@Override
		public void serialEvent(SerialPortEvent event) {
			SerialPort comPort = event.getSerialPort();
			int nBytes = comPort.bytesAvailable();
			if (nBytes < 0) return;
			byte[] newData = new byte[nBytes];
			int numRead = comPort.readBytes(newData, newData.length);
			//printStatus("Read " + numRead + " bytes");
			//for (int i = 0; i < numRead; i++) System.out.println("Received : 0x" + String.format("%02X (%d)", newData[i],newData[i]));
			System.out.print(new String(newData));
		}
	}
	
	// ----------------------------- static --------------------
	public static void listSerialPorts() {
		
		SerialPort[] ports = SerialPort.getCommPorts();
		if (ports.length==0) {
			LOGGER.severe("No serial/USB port!");
			return;
		}
		LOGGER.info("Available UART/USB Ports:");
		for (int i = 0; i < ports.length; ++i)
			System.out.println("\t[" + i + "] "
						+ "\"" + ports[i].getSystemPortName() + " ("
						+ ports[i].getDescriptivePortName() + ")" ) ;
	}
	
	/**
	 * try to discover the device associated with the USB to Serial converter that connects to the STM32 Nucleo board...
	 * @return null if nothing matches
	 */
	public static SerialPort getTTYUsbSerialPort() {
		
		listSerialPorts(); // for debugging purpose
		
		SerialPort[] ports = SerialPort.getCommPorts();
		if (ports.length==0)  return null;
		for (SerialPort port: ports) {
			if (port.getSystemPortName().contains("tty.usb")) {
				LOGGER.info("Found USB port " + port.getSystemPortName() + " (" + port.getDescriptivePortName() + ")");
				return port;
			}
				
		}
		return null;
		
	}
	
	// ----------------------------- main --------------------
	
	public static void main(String[] args) throws Exception {
		
		listSerialPorts();
		UartTransmitter ut = new UartTransmitter();
		Random rg = new Random();
		int value = 0;
		int cc = MidiCCImplementation.OCTAVE_3340A.getCode();
		while(true) {
			//ut.transmitMidiMessage(new ShortMessage(ShortMessage.NOTE_ON, 35 + rg.nextInt(12), 100));
			//Thread.sleep(1000);
			//ut.transmitMidiMessage(new ShortMessage(ShortMessage.NOTE_OFF, 40, 100));
			
			ut.transmitMidiMessage(new ShortMessage(ShortMessage.CONTROL_CHANGE, cc, value));
			value++;
			if (value > 3) value = 0;
			Thread.sleep(1000);
		}
		
	}
	
}
