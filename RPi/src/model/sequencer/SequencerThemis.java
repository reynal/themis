package model.sequencer;

import java.util.*;
import javax.sound.midi.*;
import static model.sequencer.NoteDuration.*;

public class SequencerThemis {
	
	private ArrayList<Sequence> sequences;
	private Sequence activeSequence; // y o nécessairemin lo une sequince qu'est actsive meme tsi spovidzin lo
	
		
	public SequencerThemis() throws InvalidMidiDataException {
		sequences = new ArrayList<Sequence>();
		sequences.add(activeSequence=new Sequence(Sequence.PPQ, NoteDuration.TICKS_PER_QUARTER)); // DEFAULT_PPG ticks par noire 
		activeSequence.createTrack(); // add an initial track to the default seq
	}

	/**
	 * 
	 * @author alex_
	 * change the active sequence	 *
	 */
	public void setActiveSequence(int i) {
		// make sure i is inside bounds
		if (i >= sequences.size())
			i = sequences.size()-1;
		else if (i<0) i=0;
		activeSequence = sequences.get(i);
	}  
	
	public void addNote(int trackIndex, int tick, NoteDuration duration, int note, int velocity) throws InvalidMidiDataException {
		
		try {
			Track track = activeSequence.getTracks()[trackIndex];
			track.add(new MidiEvent(new ShortMessage(ShortMessage.NOTE_ON, note, velocity), tick));
			track.add(new MidiEvent(new ShortMessage(ShortMessage.NOTE_OFF, note, velocity), tick + duration.getTicks()));
		}
		catch (IndexOutOfBoundsException e) {
			throw new IllegalTrackException(e.getMessage());
		}		
	}
	
	/**
	 * 
	 * @author alex_
	 * SequencerThemis.State
	 *
	 */
	public enum State {
		ON(true),
		OFF(false);
		
	    public boolean statebool;

	    private State(boolean flag){
	    this.statebool = flag;
	    }  
	}
	
		
	/**
	 * methode de test pour verifier que la sequence joue bien comifo
	 * @throws MidiUnavailableException
	 * @throws InvalidMidiDataException
	 */
	public void testPlay() throws MidiUnavailableException, InvalidMidiDataException {
		Sequencer sequencer = MidiSystem.getSequencer();
		sequencer.open();
		sequencer.setSequence(activeSequence);
		System.out.println("sequencer = " +sequencer);

		if (! (sequencer instanceof Synthesizer)){
			
			System.out.println("sequencer n'implémente pas l'interface Synthetizer");

			Synthesizer synth = MidiSystem.getSynthesizer(); // renvoie un pointeur vers le synthé GM
			synth.open();
			Receiver synthReceiver = synth.getReceiver(); // midi OUT du synthé
			Transmitter seqTransmitter = sequencer.getTransmitter(); // midi IN du sequenceur
			seqTransmitter.setReceiver(synthReceiver); // midi IN du sequencer avec midi OUT du synth
		}
		else System.out.println("sequencer implémente l'interface Synthetizer");
		sequencer.setLoopStartPoint(0);
		sequencer.setLoopEndPoint(WHOLE.getTicks());
		sequencer.setLoopCount(5);
		sequencer.start(); // non-blocking method ! (thread)
		System.out.println("sequenceur started");		
	}
	
	/**
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args) throws Exception {
		
		SequencerThemis st = new SequencerThemis();
		st.addNote(0, 0, WHOLE, 40, 100);		
		st.testPlay();
	} 

}
