package model.midi;

import java.io.IOException;

import javax.sound.midi.*;

import javafx.scene.control.Label;
import model.spi.SpiTransmitter;


/**
 * Capture midi input events, dispatching them to the SPI bus transmitter
 * SR TODO : need to implement a proper listener mechanism so that all interested objects may listen to incoming MIDI msg
 */
public class MidiInHandler implements Receiver {

	private MidiDevice device;
	private SpiTransmitter spiTransmitter;

	/**
	 * 
	 * @throws MidiUnavailableException
	 */
	public MidiInHandler(SpiTransmitter spiTransmitter) throws MidiUnavailableException {

		this.spiTransmitter = spiTransmitter;

		MidiDevice.Info[] infos = MidiSystem.getMidiDeviceInfo();

		for (MidiDevice.Info info : infos) {

			this.device = MidiSystem.getMidiDevice(info);
			System.out.println("Found: " + device);

			int maxTransmitters = device.getMaxTransmitters();
			System.out.println("  Max transmitters: " + maxTransmitters);

			if (maxTransmitters == -1 || maxTransmitters > 0) {
				Transmitter transmitter = device.getTransmitter();
				transmitter.setReceiver(this);
				// transmitter.setReceiver(new DumpReceiver()); // DEBUG
				System.out.println("Opening " + info);
				device.open();
				return; 
			}
		}
		throw new MidiUnavailableException("Could not find any midi input sources");
	}

	// the following method is called for every incoming MIDI message...
	@Override
	public void send(MidiMessage message, long timeStamp) {
		System.out.println(message + " received at time " + timeStamp);
		if (message instanceof ShortMessage) {
			ShortMessage sm = (ShortMessage)message;
			System.out.println("\tStatus=" + sm.getStatus() + ", data1=" + sm.getData1() + ", data2=" + sm.getData2());
			if (spiTransmitter != null)
				try {
					if (sm.getStatus() == ShortMessage.NOTE_ON || sm.getStatus() == ShortMessage.NOTE_OFF || sm.getStatus() == ShortMessage.CONTROL_CHANGE) {
						spiTransmitter.transmitMidiMessage(sm);						
						System.out.println("\tSend message " + sm + " over SPI bus to STM32");
						// TODO : SR, ici il faut ajouter les modeles pour qu'ils soient informes puisque 
						// les CONTROL_CHANGE s'adressent a eux !
					}
					
				} catch (IOException e) {
					e.printStackTrace();
				}
		}

	}

	@Override
	public void close() {
		System.out.println("Closing device " + device);
		device.close();
	}

	public static void main(String[] args) throws Exception {

		/*SpiTransmitter spi = new SpiTransmitter();
		MidiInHandler mih = new MidiInHandler(spi);*/
	}

}

