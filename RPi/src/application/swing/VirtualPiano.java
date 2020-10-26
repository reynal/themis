package application.swing;

import java.awt.BorderLayout;
import java.io.File;
import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiMessage;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Sequence;
import javax.sound.midi.Sequencer;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Transmitter;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JToggleButton;
import javax.swing.SwingConstants;

import application.Preferences;
import misc.MusicNote;
import model.midi.MidiInHandler;
import model.serial.DebugTransmitter;

/**
 * 69 = A4 57 = A3 45 = A2 33 = A1 21 = A0 9 = A-1 0 = C-1 (octave=0 here)
 */
@SuppressWarnings("serial")
public class VirtualPiano extends JPanel {

	private int note;
	private int velocity = 100;
	private int octave = 5;
	private final MidiInHandler midiInHandler;
	private final int midiChannel = 0;
	
	private static final Logger LOGGER = Logger.getLogger("confLogger");

	// position of notes boundaries on the JPG image, from 0 to 1 = 100% (width, or
	// x coordinate)
	
	final static String[] KEY_BINDINGS = {"Q", "Z", "S", "E", "D","F", "T", "G", "Y", "H", "U", "J"};

	/**
	 * @param midiInHandler the MIDI message handler responsible for sending MIDI messages to the STM32 via the serial transmitter.  
	 * 
	 */
	public VirtualPiano(MidiInHandler midiInHandler) throws IOException {

		this.midiInHandler = midiInHandler;
		setLayout(new BorderLayout());
		add(new Keyboard(), BorderLayout.CENTER);
		add(createVelocitySlider(), BorderLayout.EAST);
		add(createOctaveSlider(), BorderLayout.WEST);
		try {
			add(new DemoPanel(), BorderLayout.SOUTH);
		} catch (InvalidMidiDataException | MidiUnavailableException e) {
			e.printStackTrace();
		}
		
		/*int i=0;
		for (String key: KEY_BINDINGS) {
			getInputMap().put(KeyStroke.getKeyStroke(key), "pressed"+key);
			getInputMap().put(KeyStroke.getKeyStroke("released "+key), "released"+key);
			getActionMap().put("pressed"+key, new NoteOnAction(i));
			getActionMap().put("released"+key, new NoteOffAction(i));
			i++;
		}*/
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
		noteOnOff(true);
	}

	// send a note off message
	private void noteOff() {
		noteOnOff(false);
	}
	
	// send a note on or note off message based on the current value of the note and octave fields.
	private void noteOnOff(boolean isOn) {
		if (midiInHandler == null) {
			LOGGER.warning("No Midi Handler!");
			return;
		}
		try {
			midiInHandler.send(new ShortMessage(
					isOn ? ShortMessage.NOTE_ON : ShortMessage.NOTE_OFF, 
							midiChannel, 
							octave * 12 + note, 
							isOn ? velocity : 0), 
					0);
		} catch (InvalidMidiDataException e) {
			e.printStackTrace();
		}
		
	}

	/*
	 * the piano keyboard itself
	 */
	private class Keyboard extends JPanel {

		
		Keyboard() {
			ButtonGroup g = new ButtonGroup();
			for (MusicNote musicNote : MusicNote.values()) {
				JToggleButton b = new JToggleButton(musicNote.toString());
				b.addActionListener(e -> {
					noteOff();
					note = musicNote.ordinal();
					noteOn();
				});
				g.add(b);
				add(b);
			}
			JToggleButton b = new JToggleButton("Note Off");
			g.add(b);
			add(b);
			b.addActionListener(e -> {
				noteOff();
			});
		}

	}

	/*
	 * sequencer and MIDI song start/stop panel for demo purpose
	 */
	private class DemoPanel extends JPanel {
		
		JToggleButton tb;
		JButton loadFileBut;
		//Track track;
		Sequencer seq;
		
		DemoPanel() throws InvalidMidiDataException, MidiUnavailableException{
			add(loadFileBut = new JButton("Load MIDI file"));
			loadFileBut.addActionListener(e -> loadMidiFile());
			add(tb=new JToggleButton("Play demo"));
			tb.addActionListener(e -> playDemo());
			
			//Sequence sequence = new Sequence(Sequence.PPQ, 200);
			//track = sequence.createTrack();
			

			seq = MidiSystem.getSequencer();
			seq.open();
			loadDefaultMidiFile();
			seq.setLoopCount(Sequencer.LOOP_CONTINUOUSLY);

			Transmitter	seqTransmitter = seq.getTransmitter(); 
			seqTransmitter.setReceiver(midiInHandler); 
		}
		
		void loadDefaultMidiFile() throws InvalidMidiDataException {
			
			try {
				String n = Preferences.getPreferences().getStringProperty(Preferences.Key.MIDI_DEMO_FILE);
				File f = new File(n);
				LOGGER.info("Loading MIDI file="+f);
				Sequence s = MidiSystem.getSequence(f);
				//System.out.println("Seq="+s);
				seq.setSequence(s);
			}
			catch (Exception e) { e.printStackTrace(); }
			
		}
		
		void loadMidiFile() {
			JFileChooser jfc = new JFileChooser();
			if (jfc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
				File midiFile = jfc.getSelectedFile();
				System.out.println(midiFile.getAbsolutePath());
				try {
					seq.setSequence(MidiSystem.getSequence(midiFile));
				}
				catch (Exception e) { e.printStackTrace(); }
			}
		}
		
		void addNoteOnEventToSequence(int time, int nnote, int vel) throws InvalidMidiDataException {
			MidiMessage msg = new ShortMessage(ShortMessage.NOTE_ON, midiChannel, nnote, vel);
			MidiEvent me = new MidiEvent(msg, time);
			//track.add(me);
		}
		
		void addNoteOffEventToSequence(int time, int nnote) throws InvalidMidiDataException {
			MidiMessage msg = new ShortMessage(ShortMessage.NOTE_OFF, midiChannel, nnote, 0);
			MidiEvent me = new MidiEvent(msg, time);
			//track.add(me);
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
		MidiInHandler midiInHandler = new MidiInHandler(new DebugTransmitter(), 0);
		//MidiInHandler  midiInHandler = new MidiInHandler(new UartTransmitter("/dev/tty.usbmodem1421203"), 0);
		f.setContentPane(new VirtualPiano(midiInHandler));
		// f.pack();
		f.setSize(700, 400);
		f.setVisible(true);
	}
}
