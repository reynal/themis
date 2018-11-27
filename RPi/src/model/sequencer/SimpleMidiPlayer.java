package model.sequencer;

import java.io.File;
import java.io.IOException;

import javax.sound.midi.*;
import javax.sound.midi.Sequencer;



public class SimpleMidiPlayer{

	public static void main(String[] args) throws Exception {
		
		playMidi("test.mid"); // "C:\Users\alex_\eclipse-workspace\src\test.mid"
		//listMidiDevices();
		
	} 
	
	static void playMidi(String file) throws Exception {

		//File midiFile = new File(file);

		//Sequence sequence = MidiSystem.getSequence(midiFile);
		// PPQ: The tempo-based timing type, for which the resolution is expressed in pulses (ticks) per quarter note.
		Sequence sequence = new Sequence(Sequence.PPQ, 200);
		Track track = sequence.createTrack();
		int sept = 7;
		int oct = 12;
		for (int i=0; i<40; i++){
			if ((i%2) == 0) {
			//MidiMessage msg = new ShortMessage(ShortMessage.NOTE_ON, (int)(Math.random()*100), 100);
			MidiMessage msg = new ShortMessage(ShortMessage.NOTE_ON, (int) 20 + oct, 100);
			MidiEvent me = new MidiEvent(msg, 199 * i);
			track.add(me);
			}
			else {
				MidiMessage msg = new ShortMessage(ShortMessage.NOTE_OFF, (int) 20 + oct, 100);
				sept += 7;
				oct += 12;
				if (sept > 50) {
					sept = 7;
				}
				if (oct > 80) {
					oct = 12;
				}
				MidiEvent me = new MidiEvent(msg, 1 * i);
				track.add(me);
			}
		}

		Sequencer seq = MidiSystem.getSequencer();
		seq.open();
		seq.setSequence(sequence);
		
		System.out.println("sequencer = " +seq);

		if (! (seq instanceof Synthesizer)){
			
			System.out.println("sequencer n'implémente pas l'interface Synthetizer");

			Synthesizer synth = MidiSystem.getSynthesizer(); // renvoie un pointeur vers le synthé GM
			synth.open();
			Receiver	synthReceiver = synth.getReceiver(); // midi OUT du synthé
			Transmitter	seqTransmitter = seq.getTransmitter(); // midi IN du sequenceur
			seqTransmitter.setReceiver(synthReceiver); // midi IN du sequencer avec midi OUT du synth
		}
		else System.out.println("sequencer implémente l'interface Synthetizer");
		seq.start(); // TODO : thread ?
		System.out.println("sequenceur started");
	}
	
	static void listMidiDevices() throws Exception {
		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();
		for (MidiDevice.Info info: infos){
			
			System.out.println("MidiDevice.Info:" + info);
			
			MidiDevice device = MidiSystem.getMidiDevice(info);
			for (Class i : device.getClass().getInterfaces())
				System.out.println("\t implémente : " + i);
			System.out.println("device correspondant : " + device);
		}
		
		
	}
}


