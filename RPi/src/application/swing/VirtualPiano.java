package application.swing;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.io.*;

import javax.sound.midi.*;
import javax.swing.*;

import model.midi.MidiInHandler;
import model.serial.DebugTransmitter;
import model.serial.UartTransmitter;

/**
 * 69 = A4 57 = A3 45 = A2 33 = A1 21 = A0 9 = A-1 0 = C-1 (octave=0 here)
 */
@SuppressWarnings("serial")
public class VirtualPiano extends JPanel {

	private double width, height;
	private int note;
	private int velocity = 100;
	private int octave = 5;
	private final MidiInHandler midiHandler;
	private final int midiChannel = 0;

	// position of notes boundaries on the JPG image, from 0 to 1 = 100% (width, or
	// x coordinate)
	final static double[] NOTE_BOUNDARIES = new double[] { 0.081, 0.167, 0.264, 0.3457, 0.42857, 0.50857, 0.6, 0.671,
			0.771, 0.8428, 0.92857, 1.0 };
	
	final static String[] KEY_BINDINGS = {"Q", "Z", "S", "E", "D","F", "T", "G", "Y", "H", "U", "J"};

	/**
	 * @throws InvalidMidiDataException 
	 * @throws MidiUnavailableException 
	 * 
	 */
	public VirtualPiano(MidiInHandler midiHandler) throws IOException, InvalidMidiDataException, MidiUnavailableException {

		this.midiHandler = midiHandler;
		setLayout(new BorderLayout());
		add(new Keyboard(), BorderLayout.CENTER);
		add(createVelocitySlider(), BorderLayout.EAST);
		add(createOctaveSlider(), BorderLayout.WEST);
		add(new DemoPanel(), BorderLayout.SOUTH);
		
		int i=0;
		for (String key: KEY_BINDINGS) {
			getInputMap().put(KeyStroke.getKeyStroke(key), "pressed"+key);
			getInputMap().put(KeyStroke.getKeyStroke("released "+key), "released"+key);
			getActionMap().put("pressed"+key, new NoteOnAction(i));
			getActionMap().put("released"+key, new NoteOffAction(i));
			i++;
		}
	}

	/*
	 * An action that triggers a Note On MIDI event
	 */
	private class NoteOnAction extends AbstractAction {

		int n;

		NoteOnAction(int note) {

			this.n = note;
		}

		@Override
		public void actionPerformed(ActionEvent e) {

			note = n;
			noteOn();

		}
	}

	/*
	 * An action that triggers a Note Off MIDI event
	 */
	private class NoteOffAction extends AbstractAction {

		int n;

		NoteOffAction(int note) {

			this.n = note;
		}

		@Override
		public void actionPerformed(ActionEvent e) {

			note = n;
			noteOff();

		}
	}

	/*
	 * Utility that creates a slider that can control the given parameter.
	 */
	private JPanel createVelocitySlider() {

		JPanel p = new JPanel();
		p.setLayout(new BorderLayout());

		JLabel title = new JLabel("Velocity", SwingConstants.CENTER);
		p.add(title, BorderLayout.NORTH);

		JSlider js = new JSlider(JSlider.VERTICAL, 0, 127, velocity);
		p.add(js, BorderLayout.CENTER);

		JLabel lbl = new JLabel("0", SwingConstants.CENTER);
		p.add(lbl, BorderLayout.SOUTH);

		js.addChangeListener(e -> {
			// System.out.println(e);
			int midiValue = ((JSlider) e.getSource()).getValue();
			lbl.setText(Integer.toString(midiValue));
			velocity = midiValue;
		});
		return p;
	}

	/*
	 * Utility that creates a slider that can control the given parameter.
	 */
	private JPanel createOctaveSlider() {

		JPanel p = new JPanel();
		p.setLayout(new BorderLayout());

		JLabel title = new JLabel("Octave", SwingConstants.CENTER);
		p.add(title, BorderLayout.NORTH);

		JSlider js = new JSlider(JSlider.VERTICAL, 2, 8, octave);
		p.add(js, BorderLayout.CENTER);

		JLabel lbl = new JLabel(Integer.toString(octave), SwingConstants.CENTER);
		p.add(lbl, BorderLayout.SOUTH);

		js.addChangeListener(e -> {
			// System.out.println(e);
			int v = ((JSlider) e.getSource()).getValue();
			lbl.setText(Integer.toString(v));
			octave = v;
		});
		return p;
	}
	


	// send a note on message
	private void noteOn() {
		if (midiHandler == null)
			return;
		try {
			midiHandler.send(new ShortMessage(ShortMessage.NOTE_ON, midiChannel, octave * 12 + note, velocity), 0);
		} catch (InvalidMidiDataException e) {
			e.printStackTrace();
		}
	}

	// send a note off message
	private void noteOff() {
		if (midiHandler == null)
			return;
		try {
			midiHandler.send(new ShortMessage(ShortMessage.NOTE_OFF, midiChannel, octave * 12 + note, 0), 0);
		} catch (InvalidMidiDataException e) {
			e.printStackTrace();
		}
	}

	/*
	 * the piano keyboard itself
	 */
	private class Keyboard extends JPanel implements MouseListener, MouseMotionListener {

		private final Image backgroundImage;

		Keyboard() {
			backgroundImage = new ImageIcon(this.getClass().getResource("/res/keyboard-themis.png")).getImage();
			addMouseListener(this);
			addMouseMotionListener(this);
		}

		@Override
		public void paintComponent(Graphics g) {

			super.paintComponent(g);

			Graphics2D g2 = (Graphics2D) g;
			g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
			g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
			g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);

			width = getWidth();
			height = getHeight();
			// System.out.println("width=" + width + " height=" + height);
			double sx = width / (double) backgroundImage.getWidth(this);
			double sy = height / (double) backgroundImage.getHeight(this);
			AffineTransform at = AffineTransform.getScaleInstance(sx, sy); // let's scale down the image so that it
																			// exactly fits in the frame
			// at = AffineTransform.getScaleInstance(0.5, 0.5);
			g2.drawImage(backgroundImage, at, this);

		}

		/*
		 * Update the note value depending on the current mouse position
		 */
		private void updateNote(MouseEvent e) {
			int x = e.getX();
			int y = e.getY();

			if (y > 0.64 * height) { // mouse is on white notes only
				int toucheBlanche = (int) (7.0 * x / width);
				switch (toucheBlanche) {
				case 0:
					note = 0;
					break; // DO
				case 1:
					note = 2;
					break; // RE
				case 2:
					note = 4;
					break; // MI
				case 3:
					note = 5;
					break; // FA
				case 4:
					note = 7;
					break; // SOL
				case 5:
					note = 9;
					break; // LA
				case 6:
					note = 11;
					break; // SI
				default:
					note = -1;
				}
			} else { // both white and black notes:
				double xx = x / width;
				note = -1;
				for (int i = 0; i < NOTE_BOUNDARIES.length; i++) {
					if (xx < NOTE_BOUNDARIES[i]) {
						note = i;
						break;
					}
				}
			}
			// System.out.println("note = " + note + " x=" + e.getX() + " y=" + e.getY());
		}

		@Override
		public void mouseClicked(MouseEvent e) {

		}

		@Override
		public void mousePressed(MouseEvent e) {
			updateNote(e);
			noteOn();
		}

		@Override
		public void mouseEntered(MouseEvent e) {

		}

		@Override
		public void mouseExited(MouseEvent e) {

		}

		@Override
		public void mouseDragged(MouseEvent e) {

		}

		@Override
		public void mouseMoved(MouseEvent e) {
			updateNote(e);
		}

		@Override
		public void mouseReleased(MouseEvent e) {
			noteOff();
		}
	}

	/*
	 * 
	 */
	private class DemoPanel extends JPanel {
		
		JToggleButton tb;
		Track track;
		Sequencer seq;
		
		DemoPanel() throws InvalidMidiDataException, MidiUnavailableException{
			add(tb=new JToggleButton("Play demo"));
			tb.addActionListener(e -> playDemo());
			
			Sequence sequence = new Sequence(Sequence.PPQ, 200);
			track = sequence.createTrack();
			buildTrack();

			seq = MidiSystem.getSequencer();
			seq.open();
			seq.setSequence(sequence);
			seq.setLoopCount(Sequencer.LOOP_CONTINUOUSLY);

			Transmitter	seqTransmitter = seq.getTransmitter(); 
			seqTransmitter.setReceiver(midiHandler); 
		}
		
		void buildTrack() throws InvalidMidiDataException {
			
			for (int i=0; i<10; i++) {
				if ((i%2) == 0) addNoteOnEventToSequence(200*i, 39, 100);
				else addNoteOffEventToSequence(200*i, 39);
			}
			
		}
		
		void addNoteOnEventToSequence(int time, int nnote, int vel) throws InvalidMidiDataException {
			MidiMessage msg = new ShortMessage(ShortMessage.NOTE_ON, midiChannel, nnote, vel);
			MidiEvent me = new MidiEvent(msg, time);
			track.add(me);
		}
		
		void addNoteOffEventToSequence(int time, int nnote) throws InvalidMidiDataException {
			MidiMessage msg = new ShortMessage(ShortMessage.NOTE_OFF, midiChannel, nnote, 0);
			MidiEvent me = new MidiEvent(msg, time);
			track.add(me);
		}
		
		void playDemo() {
			if (tb.isSelected()) seq.start();
			else seq.stop();
			System.out.println("sequenceur running " + seq.isRunning());	
		}
	}	
	// ------------------------------------------------------------------------------------

	public static void main(String[] args) throws IOException, MidiUnavailableException, InvalidMidiDataException {
		JFrame f = new JFrame("test piano kbd");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		//MidiInHandler midiInHandler = new MidiInHandler(new DebugTransmitter(), 0);
		MidiInHandler  midiInHandler = new MidiInHandler(new UartTransmitter("/dev/tty.usbmodem413"), 0);
		f.setContentPane(new VirtualPiano(midiInHandler));
		// f.pack();
		f.setSize(700, 400);
		f.setVisible(true);
	}
}
