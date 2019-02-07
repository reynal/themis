package model.midi;

import java.io.IOException;

import javax.sound.midi.*;

import javafx.scene.control.Label;
import model.spi.SpiTransmitter;


/**
 * Capture midi input events, dispatching them to the SPI bus transmitter
 */
public class MidiInHandler implements Receiver {

	private MidiDevice device;
	private SpiTransmitter spiTransmitter;
	private Label label; // used to display information on a UI

	/**
	 * 
	 * @throws MidiUnavailableException
	 */
	public MidiInHandler(SpiTransmitter spiTransmitter, Label label) throws MidiUnavailableException {

		this.spiTransmitter = spiTransmitter;
		this.label = label;

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


	@Override
	public void send(MidiMessage message, long timeStamp) {
		System.out.println(message + " received at time " + timeStamp);
		if (message instanceof ShortMessage) {
			ShortMessage sm = (ShortMessage)message;
			System.out.println("\tStatus=" + sm.getStatus() + ", data1=" + sm.getData1() + ", data2=" + sm.getData2());
			if (label != null) label.setText("Status=" + sm.getStatus() + ", data1=" + sm.getData1() + ", data2=" + sm.getData2());
			if (spiTransmitter != null)
				try {
					if (sm.getStatus() == ShortMessage.NOTE_ON || sm.getStatus() == ShortMessage.NOTE_OFF || sm.getStatus() == ShortMessage.CONTROL_CHANGE) {
						spiTransmitter.transmitMidiMessage(sm);
						System.out.println("\tSend message " + sm + " over SPI bus to STM32");
					}
					
				} catch (IOException e) {
					// TODO Auto-generated catch block
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

