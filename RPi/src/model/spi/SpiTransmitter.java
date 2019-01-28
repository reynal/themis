package model.spi;

import model.event.SynthParameterEditEvent;
import model.event.SynthParameterEditListener;

/**
 * This class acts as a fast MIDI over SPI transmitter to any listening device, including the STM32.
 * @author sydxrey
 *
 */
public class SpiTransmitter implements SynthParameterEditListener<Object> {

	/**
	 * Initialize the SPI bus with the correct speed and parameter, using Pi4J library.
	 */
	public SpiTransmitter() {
		super();
		// TODO : @loic SPI initialization
		
	}

	@Override
	public void synthParameterEdited(SynthParameterEditEvent<Object> e) {
		// TODO Auto-generated method stub
		Object o = e.getValue();
		/*if (o instanceof Double) bla bla bla
		else if (o instance of Boolean) bla bla bla
		else bla bla bla*/
		// @loic : c'est ici que tu dois décortiquer l'event "e" et envoyer les données correspondantes sur le bus
	}


}
