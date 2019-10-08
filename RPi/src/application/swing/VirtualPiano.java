package application.swing;

import java.awt.BorderLayout;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.event.ActionEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.geom.AffineTransform;
import java.io.IOException;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.ShortMessage;
import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;

import model.midi.MidiInHandler;
import model.serial.DebugTransmitter;

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
	private final int channel = 1;

	// position of notes boundaries on the JPG image, from 0 to 1 = 100% (width, or
	// x coordinate)
	final static double[] NOTE_BOUNDARIES = new double[] { 0.081, 0.167, 0.264, 0.3457, 0.42857, 0.50857, 0.6, 0.671,
			0.771, 0.8428, 0.92857, 1.0 };
	
	final static String[] KEY_BINDINGS = {"Q", "Z", "S", "E", "D","F", "T", "G", "Y", "H", "U", "J"};

	/**
	 * 
	 */
	public VirtualPiano(MidiInHandler midiHandler) throws IOException {

		this.midiHandler = midiHandler;
		setLayout(new BorderLayout());
		add(new Keyboard(), BorderLayout.CENTER);
		add(createVelocitySlider(), BorderLayout.EAST);
		add(createOctaveSlider(), BorderLayout.WEST);
		
		int i=0;
		for (String key: KEY_BINDINGS) {
			getInputMap().put(KeyStroke.getKeyStroke(key), "pressed"+key);
			getInputMap().put(KeyStroke.getKeyStroke("released "+key), "released"+key);
			getActionMap().put("pressed"+key, new NoteOnAction(i));
			getActionMap().put("released"+key, new NoteOffAction(i));
			i++;
		}
	}

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
			midiHandler.send(new ShortMessage(ShortMessage.NOTE_ON, channel, octave * 12 + note, velocity), 0);
		} catch (InvalidMidiDataException e) {
			e.printStackTrace();
		}
	}

	// send a note off message
	private void noteOff() {
		if (midiHandler == null)
			return;
		try {
			midiHandler.send(new ShortMessage(ShortMessage.NOTE_OFF, channel, octave * 12 + note, 0), 0);
		} catch (InvalidMidiDataException e) {
			e.printStackTrace();
		}
	}

	/*
	 * 
	 */
	private class Keyboard extends JPanel implements MouseListener, MouseMotionListener {

		private final Image backgroundImage;

		Keyboard() {
			backgroundImage = new ImageIcon(this.getClass().getResource("/resources/img/keyboard.png")).getImage();
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

	// ------------------------------------------------------------------------------------

	public static void main(String[] args) throws IOException, MidiUnavailableException {
		JFrame f = new JFrame("test piano kbd");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		MidiInHandler midiInHandler = new MidiInHandler(new DebugTransmitter(), 1);
		f.setContentPane(new VirtualPiano(midiInHandler));
		// f.pack();
		f.setSize(700, 400);
		f.setVisible(true);
	}
}
