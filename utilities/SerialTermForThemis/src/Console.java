import java.awt.*;
import java.util.StringTokenizer;

import javax.swing.*;
import com.fazecast.jSerialComm.*; // cf jSerialComm-1.3.11.jar



/**
 *
 * @author sydxrey
 *
 */
public class Console extends JFrame implements SerialPortDataListener {

	private static final long serialVersionUID = 1L;

	private JSpinner noteSPI, velocitySPI, ccvalSPI, ccSPI;
	private JScrollPane consoleSP;
	private JTextArea consoleTA;
	private JTextField inputTF;
	private JButton noteOnBUT, noteOffBUT, midiccBUT, sendBUT, clearBUT;

	SerialDebugger themisSerialDebugger;


	public Console(SerialDebugger themisSerialDebugger){

		super("Serial console for Themis/STM32");
		this.themisSerialDebugger = themisSerialDebugger;
		setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		setLayout(new BorderLayout());
		add(createToolBar(), BorderLayout.NORTH);
		add(createTextArea(), BorderLayout.CENTER);
		//f.setSize(400,400);
		pack();
		setVisible(true);

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

		tb.add(clearBUT = new JButton("CLEAR"));
		clearBUT.addActionListener(e -> clearTextArea());

		return tb;
	}

	void clearTextArea(){

		SwingUtilities.invokeLater(() -> consoleTA.setText(""));
	}



	void printMessage(String s) {
		SwingUtilities.invokeLater(() -> consoleTA.append("[STATUS] " + s + "\n"));
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
				themisSerialDebugger.send((byte)i);
			} catch (NumberFormatException nfe) {
				System.out.println("\"" + token + "\" isn't a valid integer, transmitting as a String");
				// transmit as an ascii string:
				themisSerialDebugger.send(token);
			}
		}
	}



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
