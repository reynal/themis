package model.serial;

import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.ShortMessage;
import javax.swing.SwingUtilities;

import com.fazecast.jSerialComm.*;

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
	 * @param serialPortName
	 * @param openConsole
	 * @throws IOException
	 */
	public UartTransmitter(String serialPortName) throws IOException{

		currentPort = SerialPort.getCommPort(serialPortName);
		currentPort.setBaudRate(DEFAULT_BAUD_RATE);
		if (!currentPort.openPort())
			throw new IOException("UART opening failed");
		System.out.println("Opening UART on " 
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
		super();
		System.out.println("Opening UART serial connection");
	}
	
	@Override
	public void transmitMidiMessage(ShortMessage sm) throws IOException {
		
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
			for (int i = 0; i < numRead; i++) {
				System.out.println("Received : 0x" + String.format("%02X (%d)", newData[i],newData[i]));
			}
			System.out.println(new String(newData));
		}
	}
	
	// ----------------------------- static --------------------
	public static void listSerialPorts() {
		
		SerialPort[] ports = SerialPort.getCommPorts();
		System.out.println("\nAvailable Ports:\n");
		for (int i = 0; i < ports.length; ++i)
			System.out.println("   [" + i + "] "
						+ "\"" + ports[i].getSystemPortName() + "\" : "
						+ "\"" + ports[i].getDescriptivePortName() + "\"" ) ;
	}
	
	/**
	 * try to discover the device associated with the USB to Serial converter that connects to the STM32 Nucleo board...
	 * @return null if nothing matches
	 */
	public static String getTTYUsbSerialPort() {
		
		SerialPort[] ports = SerialPort.getCommPorts();
		System.out.println("\nAvailable Ports:\n");
		for (SerialPort port: ports) {
			if (port.getSystemPortName().contains("tty.usbserial")) return port.getSystemPortName();
		}
		return null;
		
	}
	
}
