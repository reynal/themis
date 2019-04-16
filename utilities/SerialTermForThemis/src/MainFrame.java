import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.StringTokenizer;

import javax.swing.*;
import com.fazecast.jSerialComm.*; // cf jSerialComm-1.3.11.jar



/**
 * 
 * @author sydxrey
 *
 */
public class MainFrame {
	
	private static final long serialVersionUID = 1L;

	private SerialPort currentPort; 
	private int currentBaudRate = DEFAULT_BAUD_RATE;
	public static final int DEFAULT_BAUD_RATE = 9600;
	public static final int[] BAUD_RATES = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
	private JComboBox<String> portCB;
	private JComboBox<Integer> baudRateCB;
	private JSpinner noteSPI, velocitySPI, ccvalSPI, ccSPI;
	private JScrollPane consoleSP;
	private JTextArea consoleTA;
	private JTextField inputTF;
	private JLabel statusLBL;
	private JButton openBUT, closeBUT, noteOnBUT, noteOffBUT, midiccBUT, sendBUT;


	public MainFrame(){
		
		this(null, DEFAULT_BAUD_RATE);
	}
	
	public MainFrame(String serialPortName, int baudRate){
		
		if (serialPortName != null) currentPort = SerialPort.getCommPort(serialPortName);
		currentBaudRate = baudRate;
		JFrame f = new JFrame("Serial terminal for Themis/STM32");
		f.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		f.setLayout(new BorderLayout());
		f.add(createToolBar(), BorderLayout.NORTH);
		f.add(createTextArea(), BorderLayout.CENTER);
		f.add(createStatusBar(), BorderLayout.SOUTH);
		//f.setSize(400,400);
		f.pack();
		f.setVisible(true);
		f.addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent e) {
				closeConnection();
				f.dispose();
			}});

		//currentBaudRate = DEFAULT_BAUD_RATE;
	}
	
	/** 
	 * */
	JComponent createTextArea() {
		
		JPanel p = new JPanel();
		p.setLayout(new BorderLayout());
		consoleTA = new JTextArea(30, 100);
		consoleSP = new JScrollPane(consoleTA);
		p.add(consoleSP, BorderLayout.CENTER);
		inputTF = new JTextField();
		Box inputBox = new Box(BoxLayout.X_AXIS);
		inputBox.add(inputTF);
		inputBox.add(sendBUT=new JButton("SEND!"));
		inputBox.add(noteOnBUT=new JButton("Note On"));
		inputBox.add(noteOffBUT=new JButton("Note Off"));
		//inputBox.add(new JLabel("note="));
		inputBox.add(noteSPI = new JSpinner(new SpinnerNumberModel(69, 0, 127, 1)));
		//inputBox.add(new JLabel("vel="));
		inputBox.add(velocitySPI = new JSpinner(new SpinnerNumberModel(100, 0, 127, 1)));
		inputBox.add(midiccBUT=new JButton("Midi CC"));
		//inputBox.add(new JLabel("cc="));
		inputBox.add(ccSPI = new JSpinner(new SpinnerNumberModel(1, 0, 127, 1)));
		//inputBox.add(new JLabel("vv="));
		inputBox.add(ccvalSPI = new JSpinner(new SpinnerNumberModel(64, 0, 127, 1)));
		p.add(inputBox,BorderLayout.NORTH);
		noteOffBUT.addActionListener(e -> inputTF.setText("128 " + noteSPI.getValue() + " 0"));
		noteOnBUT.addActionListener(e -> inputTF.setText("144 " + noteSPI.getValue() + " " + velocitySPI.getValue()));
		midiccBUT.addActionListener(e -> inputTF.setText("176 " + ccSPI.getValue() + " " + ccvalSPI.getValue()));
		inputTF.addActionListener(e -> transmit(inputTF.getText()));
		sendBUT.addActionListener(e ->transmit(inputTF.getText()));
		return p;
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
			if (currentPort == null && port.getSystemPortName().contains("usbserial")) currentPort = port;
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

		tb.addSeparator();
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
		printStatus("Opening " + currentPort.getSystemPortName() + " : " + (success ? "ok" : "failed"));
		if (!success)
			return false;

		printConnectionStatus(true);
		currentPort.addDataListener(new DataListener());
		
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

	/**
	 * 
	 * @param s
	 */
	void transmit(String s) {
		
		inputTF.setText("");
		consoleTA.append("\n> " + s + "\n");
		
		int i=0;
		StringTokenizer st = new StringTokenizer(s, " ");
		while (st.hasMoreTokens()) {
			String token = st.nextToken();
			try {
				if (token.startsWith("0x")) i = Integer.parseInt(token.substring(2),16);
				else i = Integer.parseInt(token);
				System.out.println("Transmitting :\"0x" + String.format("%02X (%d)", i, i)+ "\"");
				send((byte)i);
			} catch (NumberFormatException nfe) {
				System.out.println("\"" + token + "\" isn't a valid integer, transmitting as a String");
				// transmit as an ascii string:
				send(token);
			}
		}				
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
		}
		else System.out.println("Connection not open");
	}
	
	/**
	 * Affiche la chaine dans la barre de status
	 */
	public void printStatus(String s) {
		System.out.println("[STATUS] " + s);
		//SwingUtilities.invokeLater(() -> statusLBL.setText("[STATUS] " + s));
		SwingUtilities.invokeLater(() -> consoleTA.append("[STATUS] " + s + "\n"));
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
	
	
	/**
	 * listends to incoming packets
	 */
	private class DataListener implements SerialPortDataListener {

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
			SwingUtilities.invokeLater(() -> consoleTA.append(new String(newData)));
		}
	}
	
	
	// -----------------------------------------------
	
	public static void main(String[] args) {
		
		if (args.length >= 2) new MainFrame(args[0], Integer.parseInt(args[1]));
		else new MainFrame();
		
		
	}
	

}
