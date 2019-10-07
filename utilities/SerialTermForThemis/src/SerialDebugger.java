import java.awt.BorderLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JToolBar;
import javax.swing.SwingUtilities;

import com.fazecast.jSerialComm.*;

public class SerialDebugger extends JFrame {


	private static final long serialVersionUID = 1L;
	SerialPort currentPort;
	int currentBaudRate = DEFAULT_BAUD_RATE;

	public static final int DEFAULT_BAUD_RATE = 9600;
	public static final int[] BAUD_RATES = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
	JComboBox<String> portCB;
	JComboBox<Integer> baudRateCB;
	JButton openBUT, closeBUT;
	JLabel statusLBL;
	
	Console themisConsole;
	MidiCCPanel midiCCPanel;

	
	public SerialDebugger(){

		this(null, DEFAULT_BAUD_RATE);
	}

	public SerialDebugger(String serialPortName, int baudRate){

		super("Serial terminal for Themis/STM32");
		if (serialPortName != null) currentPort = SerialPort.getCommPort(serialPortName);
		currentBaudRate = baudRate;
		setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		setLayout(new BorderLayout());
		add(createToolBar(), BorderLayout.NORTH);
		add(createStatusBar(), BorderLayout.SOUTH);
		//f.setSize(400,400);
		pack();
		setVisible(true);
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				closeConnection();
				dispose();
				System.exit(0);
			}});
		
		themisConsole = new Console(this);
		midiCCPanel = new MidiCCPanel(this);

	}	
	
	/**
	 * */
	JToolBar createToolBar() {

		JToolBar tb = new JToolBar();
		tb.add(openBUT = new JButton("OPEN"));
		openBUT.addActionListener(e -> openConnection());
		tb.addSeparator();

		tb.add(new JLabel("Port : "));
		portCB = new JComboBox<String>();
		for (SerialPort port : SerialPort.getCommPorts()) {
			portCB.addItem(port.getSystemPortName());
			if (currentPort == null && port.getSystemPortName().contains("usb")) currentPort = port;
		}
		if (currentPort != null) portCB.setSelectedItem(currentPort.getSystemPortName());
		portCB.addActionListener(e -> currentPort = SerialPort.getCommPort((String)portCB.getSelectedItem()));
		tb.add(portCB);

		tb.addSeparator();
		tb.add(new JLabel("Baudrate : "));
		baudRateCB = new JComboBox<Integer>();
		for (int r : BAUD_RATES) baudRateCB.addItem(r);
		baudRateCB.setSelectedItem(currentBaudRate);
		baudRateCB.addActionListener(e -> currentBaudRate = (Integer)baudRateCB.getSelectedItem());

		tb.add(baudRateCB);

		tb.add(closeBUT = new JButton("CLOSE"));
		closeBUT.addActionListener(e -> closeConnection());

		return tb;
	}	
	
	/**
	 * */
	Box createStatusBar() {

		Box b = new Box(BoxLayout.X_AXIS);
		b.add(statusLBL=new JLabel("STATUS : "));
		return b;
	}
	
	/**
	 * Ouvre une connection en écoute sur le port série actuellement sélectionné par la combobox
	 */
	boolean openConnection() {

		//currentPort = SerialPort.getCommPort();

		if (currentPort == null) {
			printStatus("select a valid serial port first!");
			return false;
		}

		currentPort.setBaudRate(currentBaudRate);
		boolean success = currentPort.openPort();
		printStatus("Opening " + currentPort.getSystemPortName()
				+ " at " + currentPort.getBaudRate() + " bauds, "
				+ (currentPort.getParity()==SerialPort.NO_PARITY ? "No parity " : "Parity ")
				+ currentPort.getNumDataBits() + " databits, "
				+ currentPort.getNumStopBits() + " stopbits, "
				+ " : " + (success ? "ok" : "failed"));
		if (!success)
			return false;

		printConnectionStatus(true);
		currentPort.addDataListener(themisConsole);

		return true;
	}

	/**
	 * release serial port resource
	 */
	boolean closeConnection() {

		if (currentPort == null || currentPort.isOpen()==false) {
			printStatus("Already closed");
			return true;
		}
		currentPort.removeDataListener();

		boolean success = currentPort.closePort();
		printStatus("Closing " + currentPort.getSystemPortName() + ": " + currentPort.getDescriptivePortName() + ": " + (success ? "ok" : "failed"));
		if (!success)
			return false;

		printConnectionStatus(false);
		currentPort = null;
		return true;

	}

	void send(byte i) {
		if (currentPort != null && currentPort.isOpen()) {
			byte[] buffer = new byte[1];
			buffer[0] = i;
			currentPort.writeBytes(buffer, 1);
		}
		else System.out.println("Connection not open");
	}

	void send(String s) {
		if (currentPort != null && currentPort.isOpen()) {
			byte[] buffer = s.getBytes();
			currentPort.writeBytes(buffer, buffer.length);
			System.out.println("Sending "+s);
		}
		else System.out.println("Connection not open");
	}

	/**
	 * Affiche la chaine dans la barre de status
	 */
	public void printStatus(String s) {
		System.out.println("[STATUS] " + s);
		//SwingUtilities.invokeLater(() -> statusLBL.setText("[STATUS] " + s));
		themisConsole.printMessage(s);
	}

	/**
	 * Display a msg in the status bar
	 */
	void printConnectionStatus(boolean isActive) {
		//statusBar.connectionStatus(isActive);
		if (isActive) {
			printStatus("Active connection on " + currentPort.getSystemPortName() + " at " + currentPort.getBaudRate());
			SwingUtilities.invokeLater(() -> statusLBL.setText("Connected on " + currentPort.getSystemPortName() + " at " + currentPort.getBaudRate() + " bauds"));
		}
		else printStatus("CONNECTION is CLOSED");
	}

	/**
	 * */
	static void listSerialPorts() {
		SerialPort[] ports = SerialPort.getCommPorts();
		System.out.println("\nAvailable Ports:\n");
		for (int i = 0; i < ports.length; ++i)
			System.out.println("   [" + i + "] "
						+ "\"" + ports[i].getSystemPortName() + "\" : "
						+ "\"" + ports[i].getDescriptivePortName() + "\"" ) ;
	}


	
	// -----------------------------------------------

	public static void main(String[] args) {

		if (args.length >= 2) new SerialDebugger(args[0], Integer.parseInt(args[1]));
		else new SerialDebugger();


	}
	
}
