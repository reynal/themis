package model.midi;

import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.*;

import application.ModuleFactory;
import model.ModuleParameter;
import model.serial.AbstractSerialTransmitter;


/**
 * Capture midi input events, dispatching them to a serial bus transmitter
 * 
 */
public class MidiInHandler implements Receiver {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MidiDevice device;
	private int midiChannel;
	private AbstractSerialTransmitter serialTransmitter;

	/**
	 * Creates a Midi IN handler that listens to incomindg MIDI events on the given midiChannel.
	 * Note ON/OFF MIDI messages are forwarded to the given AbstractSerialTransmitter
	 * CC messages are transmitted to appropriate ModuleParameter's.
	 * @throws MidiUnavailableException
	 */
	public MidiInHandler(AbstractSerialTransmitter spiTransmitter, int midiChannel) throws MidiUnavailableException {

		this.midiChannel = midiChannel;
		this.serialTransmitter = spiTransmitter;

		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

		for (MidiDevice.Info info : infos) {

			this.device = MidiSystem.getMidiDevice(info);
			int maxTransmitters = device.getMaxTransmitters();
			LOGGER.info("Found: " + device + " with "+ maxTransmitters +" transmitters (aka Midi Out)");

			if (maxTransmitters == -1 || maxTransmitters > 0) {
				Transmitter transmitter = device.getTransmitter();
				transmitter.setReceiver(this);
				// transmitter.setReceiver(new DumpReceiver()); // DEBUG
				LOGGER.info("Opening " + info);
				device.open();
				LOGGER.info("Listening on channel " + midiChannel);
				return; 
			}
		}
		throw new MidiUnavailableException("Could not find any midi input sources");
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
			LOGGER.info("Status=" + sm.getStatus() + " data1=" + sm.getData1() + " data2=" + sm.getData2());
			
			// forward Note ON and OFF to Serial Transmitter, and CC directly to module parameters !
			if (serialTransmitter != null)
				try {
					if (sm.getCommand() == ShortMessage.NOTE_ON || sm.getCommand() == ShortMessage.NOTE_OFF) { 
						serialTransmitter.transmitMidiMessage(sm);						
						System.out.println("\tTransmitting Note ON/OFF message " + sm);
					}
					else if (sm.getCommand() == ShortMessage.CONTROL_CHANGE) {
						ModuleParameter<?> parameter = ModuleFactory.getDefault().getModuleParameter(sm.getData1());
						if (parameter != null) parameter.setValueFromMIDICode(sm.getData2());
						else LOGGER.warning("No module parameter associated to MIDI CC" + sm.getData1());
					}
					
				} catch (IOException e) {
					e.printStackTrace();
				}
		}

	}

	@Override
	public void close() {
		LOGGER.info("Closing MIDI device " + device);
		device.close();
	}

	public static void main(String[] args) throws Exception {

		/*SpiTransmitter spi = new SpiTransmitter();
		MidiInHandler mih = new MidiInHandler(spi);*/
	}

}

