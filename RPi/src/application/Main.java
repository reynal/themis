package application;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.LogManager;
import java.util.logging.Logger;


/**
 * The main entry point to the Themis application.
 * 
 * There are two available modes: - simulator: the whole hardware interface
 * (push buttons, LEDs, etc) is simulated on a computer through a UI graphic
 * interface based on Swing - hardware (on a Raspberry Pi) : the graphic UI
 * interface is limited to the embedded touchscreen ; the hardware must be
 * connected to a Raspberry.
 * 
 * @author reynal
 *
 */
public class Main {

	// logging:
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	static {
		try {
			LogManager.getLogManager().readConfiguration(new FileInputStream("./logger.properties"));
		} catch (IOException exception) {
			LOGGER.log(Level.SEVERE, "Error in loading configuration", exception);
		}
	}

	// ---------------------------------------------------------------------------

	public static void main(String[] args) throws Exception {

		HardwareManager.start();
	}



	private static void testLogger() throws SecurityException, FileNotFoundException, IOException {

		Main.LOGGER.setLevel(Level.SEVERE);
		Main.LOGGER.info("une information");
		Main.LOGGER.warning("un warning");
		Main.LOGGER.severe("un truc grave");
		Main.LOGGER.fine("un truc fin");

		LogManager.getLogManager().readConfiguration(new FileInputStream("./logger.properties"));

		System.out.println("-----------------");

		Main.LOGGER.info("une information");
		Main.LOGGER.warning("un warning");
		Main.LOGGER.severe("un truc grave");
		Main.LOGGER.fine("un truc fin");

	}

}
