package model.midi;

import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.*;

import application.ModuleFactory;
import application.Preferences;
import model.ModuleParameter;
import model.serial.*;


/**
 * Capture MIDI input events, dispatching them to interested listeners, e.g., a serial bus transmitter
 * 
 */
public class MidiInHandler implements Receiver {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MidiDevice device;
	private int midiChannel;
	private AbstractSerialTransmitter serialTransmitter;

	/**
	 * Creates a Midi IN handler that listens to incoming MIDI events on the given midiChannel.
	 * Note ON/OFF MIDI messages are directly forwarded to the given AbstractSerialTransmitter (e.g., USB port, etc)
	 * 
	 * Midi CC messages are transmitted to appropriate ModuleParameter's.
	 * 
	 * @throws MidiUnavailableException
	 */
	public MidiInHandler(AbstractSerialTransmitter spiTransmitter, int midiChannel) throws MidiUnavailableException {

		this.midiChannel = midiChannel;
		this.serialTransmitter = spiTransmitter;
		
		listMidiTransmitters();

		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();
		
		String expectedMidiDevice = Preferences.getPreferences().getStringProperty(Preferences.Key.MIDI_IN);
		
		for (MidiDevice.Info info : infos) {
			
			if (!info.getDescription().contains(expectedMidiDevice)) continue; 

			device = MidiSystem.getMidiDevice(info);
			int maxTransmitters = device.getMaxTransmitters();
			if (maxTransmitters == 0 || device instanceof Sequencer) continue; // not a MIDI OUT port
			
			Transmitter transmitter = device.getTransmitter();
			transmitter.setReceiver(this);
			// transmitter.setReceiver(new DumpReceiver()); // DEBUG
			LOGGER.info("Opening MIDI device \"" + info.getDescription() + "\", listening on channel " + midiChannel);
			device.open();
			return; 
		}
		LOGGER.warning("Could not find any input MIDI source! Please plug a MIDI keyboard!");
	}
	
	public static void listMidiTransmitters() {
		
		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();
		
		String str = "Listing Midi Devices with transmitters: (out of " + infos.length + " MIDI devices found)\n";

		for (MidiDevice.Info info : infos) {

			try {
				MidiDevice device;
				device = MidiSystem.getMidiDevice(info);
				int maxTransmitters = device.getMaxTransmitters();
				if (maxTransmitters == 0 || device instanceof Sequencer) continue; // not a MIDI OUT port
				str += "\t- \"" + info.getDescription() + "\"" + (maxTransmitters==-1 ? "" : (maxTransmitters + " transmitters"));
			} catch (MidiUnavailableException e) {
				e.printStackTrace();
			}
		}
		LOGGER.info(str);
	}

	// the following method is called for every incoming MIDI message...
	@Override
	public void send(MidiMessage message, long timeStamp) {
		
		//System.out.println(message + " received at time " + timeStamp);
		
		if (message instanceof ShortMessage) {
			ShortMessage sm = (ShortMessage)message;
			if (sm.getChannel() != this.midiChannel) {
				LOGGER.info("Incoming MIDI Message on channel " + sm.getChannel() + " while we are listening on channel " + midiChannel + " (Cmd=" + sm.getCommand() + " data1=" + sm.getData1() + " data2=" + sm.getData2() + ")");
				return;
			}
			// from now on, this message is for us
			// forward Note ON and OFF to Serial Transmitter, and CC directly to module parameters !
			if (serialTransmitter != null) {
				try {
					if (sm.getCommand() == ShortMessage.NOTE_ON) { 
						if(sm.getData2() == 0) // bug fix for old keyboard that do now understand NOTE OFF, but a NOTE ON with 0 vel instead
							try {
								sm = new ShortMessage(ShortMessage.NOTE_OFF, sm.getData1(), 0);
							} catch (InvalidMidiDataException e) {
								e.printStackTrace();
							}
						serialTransmitter.transmitMidiMessage(sm);						
						LOGGER.info("\tSending Note-ON to STM32: note="+sm.getData1()+" vel="+sm.getData2());
					}
					else if (sm.getCommand() == ShortMessage.NOTE_OFF) { 
						serialTransmitter.transmitMidiMessage(sm);						
						LOGGER.info("\tSending Note-OFF to STM32: note="+sm.getData1());
					}
					else if (sm.getCommand() == ShortMessage.CONTROL_CHANGE) {
						ModuleParameter<?> parameter = ModuleFactory.getDefault().getModuleParameter(sm.getData1());
						if (parameter != null) {
							parameter.setValueFromMIDICode(sm.getData2());
							LOGGER.info("\tForwarding MIDI CC"+sm.getData1()+" message to " + parameter + " with value " + sm.getData2());
						}
						else LOGGER.warning("No ModuleParameter associated with MIDI CC" + sm.getData1());
					}
					
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
			else LOGGER.warning("No SERIAL transmitter plugged into MidiInHandler");
			
		}

	}

	@Override
	public void close() {
		if (device == null) return; 
		LOGGER.info("Closing MIDI device " + device);
		device.close();
	}

	public static void main(String[] args) throws Exception {

		//UartTransmitter trans = new UartTransmitter();
		//new MidiInHandler(null, 0);
		
		listMidiTransmitters();
		
		//listMidiOutDevices();
	}

}

