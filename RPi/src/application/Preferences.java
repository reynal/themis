package application;

import java.io.*;
import java.util.*;
import java.util.logging.Logger;

/**
 * 
 * @author sydxrey
 *
 */
public class Preferences {
	
	private static Preferences preferences;
	private Properties properties;
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	public static final String DEFAULT_FILE = "properties.txt";
	private static String fileName = DEFAULT_FILE;
	
	public static enum Key {
		MIDI_IN,
		DEFAULT_MIDI_CHANNEL,
		USB_DEV,
		MIDI_DEMO_FILE,
		SERIAL_OUT,
		PLATFORM, // one of PLATFORM enum
		OPEN_RPI_TOUCHSCREEN,
		OPEN_TABBED_TOUCHSCREEN,
		CREATE_SYNTH_CTRLPANE,
		OPEN_SYNTH_CTRLPANE_SIMULATION;
	}
	
	
	public static Preferences getPreferences() {
		if (preferences == null) preferences = new Preferences();
		return preferences;
	}
	
	public void setFile(String name) {
		fileName = name;
		try {
			properties.load(new BufferedReader(new FileReader(fileName)));
		} catch (IOException e1) {
			e1.printStackTrace();
			LOGGER.severe("properties.txt file not found!");
		}
	}
	
	private Preferences(){

		properties = new Properties();
		try {
			properties.load(new BufferedReader(new FileReader(fileName)));
		} catch (IOException e1) {
			e1.printStackTrace();
			LOGGER.severe("properties.txt file not found!");
		}

		//this.usbPort = getStringProperty(properties, "port_usb");
		//int xbee_count = getIntProperty(properties, "xbee_count");
	}

	public String getStringProperty(Key key) {
		String val = properties.getProperty(key.toString());
		if (val == null) {
			LOGGER.severe("[Reading "+fileName+"] key (String) \"" + key + "\" non definie !");
			System.exit(0);
		}

		return val;
	}

	public int getIntProperty(Key key) {
		String val = properties.getProperty(key.toString());
		if (val == null) {
			LOGGER.severe("[Reading "+fileName+"] key (int) \"" + key + "\" non definie !");
			System.exit(0);
		}

		return Integer.parseInt(val);
	}
}
