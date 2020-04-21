package application;

import java.awt.HeadlessException;
import java.io.IOException;
import java.util.logging.Logger;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiUnavailableException;

import com.fazecast.jSerialComm.SerialPort;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;
import com.pi4j.system.SystemInfo;

import application.swing.HardwareApp;
import application.swing.SynthControllerPaneSimulator;
import application.swing.TabbedTouchScreen;
import application.swing.TouchScreen;
import controller.SynthControllerPane;
import controller.component.ControlFactory;
import device.IS31FL3731;
import device.MCP23017;
import model.midi.MidiDumpReceiver;
import model.midi.MidiInHandler;
import model.serial.AbstractSerialTransmitter;
import model.serial.DebugTransmitter;
import model.serial.SpiTransmitter;
import model.serial.UartTransmitter;
import view.component.ViewFactory;

/**
 * This singleton class is responsible for configuring the underlying hardware and providing hardware information on-demand.
 * 
 * A brief summary of the various event dispatching mechanisms implemented in this software:
 * 
 *                                                                          Java MIDI Keyboard 
 *                                                                                             \
 *      Midi In (midi kbd + CC controllers) ->  Note ON/OFF are directly forwarded (faster)  ->   Serial Transmitter to STM32
 *                                          \ (midi CC)                                         /
 *     controller.Control (PushButton, etc)  -> model.ModuleParameter (Attack, VCO detune, etc) -> view.component.BarGraph (hardware)
 *                                          /                                                   \
 *   view.touchscreen.VcaView (mouse clicks)                                                     -> view.touchscreen.VcaView (raspberry touchscreen)
 *                                        /
 *                   controller.MidiCCPad
 * 
 * 
 * 
 * - incoming MIDI message (e.g. from a MIDI keyboard) are forwarded to the current serialTransmitter (e.g. UART on a Mac/PC or SPI on a Raspberry) ;
 *   those that are Midi CC are also forwarded to listening ModuleParameter
 *   
 * - ModuleParameter's thus listen to both incoming MIDI messages, changes of controller positions (pushbutton, rotary encoder) and actions on the touchscreen.
 * 
 * - View's (both BarGraph and touchscreenView) listen to change
 * 
 * @author reynal
 *
 */
public class HardwareManager {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	public static final boolean DUMP_MIDI = false;
	private static final int DEFAULT_MIDI_CHANNEL = 0; // TODO corresponds to MIDI channel 1 ???
	
	private static final boolean CREATE_SYNTH_CTRLPANE = false;
	private static final boolean OPEN_SYNTH_CTRLPANE_SIMULATION = false;
	private static final boolean OPEN_TABBED_TOUCHSCREEN = true; 
	private static final boolean OPEN_RPI_TOUCHSCREEN = false;
	
	enum Platform {
		RASPBERRYPI, // => SPI, possibly UART, simulator depends on available screen TODO: check screen size
		DESKTOP // => UART, simulator
	}
	
	private static HardwareManager singleton;
	
	private boolean isSynthControlPaneHWConnected; // if true, means the expected hardware (HW) devices are visible on the I2C bus => can start HardwareApp
	private Platform platform;
	private AbstractSerialTransmitter serialTransmitter;
	private MidiInHandler midiInHandler;
	private SynthControllerPane synthControllerPane;
	
	/**
	 * TODO: handle command line options
	 * @throws UnsupportedBusNumberException I2C related errors
	 * @throws IOException UART errors and others
	 * @throws HeadlessException when a screen is needed but not connected
	 * @throws MidiUnavailableException 
	 * @throws InvalidMidiDataException 
	 */
	private HardwareManager() throws HeadlessException, IOException, UnsupportedBusNumberException, InvalidMidiDataException, MidiUnavailableException {
		
		checkPlatform(); // RPi or desktop ?
		
		createSerialTransmitter(); // try SPI or UART
		
		createMidiInSystem(); // Midi in handler
		
		if (CREATE_SYNTH_CTRLPANE) createSynthControllerPane(); // based on MCP23017 and IS31FL3137 led driver
		
		// debug createTouchScreen();
		
		initShutdownHook(); // closes resource before exiting

		switch (platform) {
		
		// simulator
		case DESKTOP:
			if (OPEN_SYNTH_CTRLPANE_SIMULATION) new SynthControllerPaneSimulator(synthControllerPane); // open front pane simulator
			if (OPEN_TABBED_TOUCHSCREEN) new TabbedTouchScreen(midiInHandler).openJFrame();
			if (OPEN_RPI_TOUCHSCREEN) new TouchScreen().openJFrame();
			break;
			
		// hardware may be connected
		case RASPBERRYPI:
			if (isSynthControlPaneHWConnected) {
				if (OPEN_TABBED_TOUCHSCREEN) new HardwareApp(new TabbedTouchScreen(midiInHandler));
				//else new HardwareApp(touchScreen, touchScreenMenuBar);
			}
			else {
				//if (USE_TABBED_TOUCHSCREEN) new SimulatorApp(new TabbedTouchScreen(midiInHandler));
				//else new SimulatorApp(touchScreen, touchScreenMenuBar); 
			}
			break;
		default:
			break;
		}
	}
	
	/**
	 * Start the hardware.
	 * @throws MidiUnavailableException 
	 * @throws InvalidMidiDataException 
	 */
	public static void start() {
		
		if (singleton == null) // prevents more than one instanciation
			try {
				singleton = new HardwareManager();
			} catch (Exception e) {
				e.printStackTrace();
				System.exit(1);
			} 
		
	}
	
	/**
	 * @return the default HardwareManager ; TODO allow multiple hardware implementations with the same software.
	 */
	public HardwareManager getDefaultHardwareManager() {
		
		if (singleton == null) start();
		return singleton;
		
	}
	
	/*
	 * 
	 */
	private void checkPlatform() {
		
		platform = Platform.RASPBERRYPI;
		
		// hack to know if we're running on a RPi or a desktop computer: if not on RPi, following P4J code should trigger an exception
		// of type FileNotFoundException (coz it looks for /proc/cpuinfo, which does not exist on OS X or Windows)
		try {
			SystemInfo.BoardType boardType = com.pi4j.system.SystemInfoFactory.getProvider().getBoardType();
			LOGGER.info("boardType = " + boardType);
			if (boardType != SystemInfo.BoardType.RaspberryPi_3B) platform = Platform.DESKTOP;
			
		} catch (IOException | UnsupportedOperationException | InterruptedException e) {
			LOGGER.info(e.toString() + " => probably not running on a RPi, assuming Platform.DESKTOP");
			platform = Platform.DESKTOP;
		}
		
		
	}
	
	/*
	 * Init a serial transmitter based on hardware guess, then makes it a listener to 
	 * module parameter changes.
	 */
	private void createSerialTransmitter() {
		
		serialTransmitter = null;

		try {
			if (platform == Platform.RASPBERRYPI)
				serialTransmitter = new SpiTransmitter();
			else { // let's try to see if there's a serial port available on the host station:
				serialTransmitter = new UartTransmitter();
			}
		} catch (IOException e) {
			e.printStackTrace();
			serialTransmitter = new DebugTransmitter();
		}

		ModuleFactory.getDefault().attachSerialTransmitter(serialTransmitter);

	}
	
	/*
	 * Initializes the Midi IN system so that incoming MIDI message (e.g. from a MIDI keyboard)
	 * are forwarded to the current serialTransmitter (e.g. UART on a Mac/PC or SPI on a Raspberry)
	 */
	private void createMidiInSystem() {
		
		try {
			midiInHandler = new MidiInHandler(serialTransmitter, DEFAULT_MIDI_CHANNEL);
		} 
		catch (MidiUnavailableException e) {
				e.printStackTrace();
		}
		
		if (DUMP_MIDI) 
			new MidiDumpReceiver(System.out);
		
		
	}
	
	/*
	 * Try to create a SynthControllerPane if a front pane hardware (MCP23017 etc) is connected. 
	 * Otherwise creates a graphic simulator.
	 */
	private void createSynthControllerPane() {

		MCP23017 mcpDevice1=null;
		MCP23017 mcpDevice2=null;
		IS31FL3731 is31Device=null;
		
		isSynthControlPaneHWConnected = true;
		
		// let's try to create hardware instances:
		try {
			mcpDevice1 = new MCP23017(); // TODO : I2C adress must not be the same for both devices!
			// mcpDevice2 = new MCP23017(); // TODO PENDING
			is31Device = new IS31FL3731();
		} catch (IOException | UnsupportedBusNumberException | UnsatisfiedLinkError e) {
			//e.printStackTrace();
			isSynthControlPaneHWConnected = false;
			LOGGER.warning("Frontpane hardware probably not connected!");
		}
		
		ControlFactory controlFactoryLeft = new ControlFactory(mcpDevice1); // one factory for each MCP device
		ControlFactory controlFactoryRight = new ControlFactory(mcpDevice2);
		ViewFactory viewFactory = new ViewFactory(is31Device);
		synthControllerPane = new SynthControllerPane(controlFactoryLeft, controlFactoryRight, viewFactory);
	}
			

	/*
	 * 
	 */
	private void initShutdownHook() {

		Runtime.getRuntime().addShutdownHook(
				new Thread() {
					public void run() {
						LOGGER.info("Shutdown Hook is running !");
						closeHardware();
					}
				}
		);
	}
	
	/*
	 * Release all hardware resource (UART, Midi, etc)
	 */
	private void closeHardware() {
		
		if (serialTransmitter != null) serialTransmitter.close();
		if (midiInHandler != null) midiInHandler.close();
	}
}
