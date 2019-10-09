package application.swing;
	
import java.awt.*;
import java.io.*;
import java.util.logging.Logger;

import javax.swing.*;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import application.ModuleFactory;
import controller.SynthControllerPane;
import view.touchscreen.TouchScreenViewFactory;

/**
 * This is the main application frame in simulator mode, i.e., when a desktop computer is the current platform.
 */	
public class SimulatorApp extends JFrame {
	
	private static final long serialVersionUID = 1L;
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	

	/**
	 * 
	 */
	public SimulatorApp(SynthControllerPane scp, TouchScreen ts, JMenuBar mb)  {
		
		super("Themis");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// JPanel p = createDecoratedPanel("Simulated RPi touchscreen");
		
		setLayout(new GridLayout(2,1,10,10));
		setBackground(Color.black); // #222
		
		add(ts);
		ts.setPreferredSize(new Dimension(1600,910)); // TODO => move to resource file
		
		setJMenuBar(mb);		
	    	    
		// debug add(new SynthControllerPaneSimulator(scp));

		pack();
		setVisible(true);
		
		LOGGER.info("Starting Swing Themis simulator");
	}	
	
	/**
	 * temporary alternate approach
	 */
	public SimulatorApp(SynthControllerPane scp, TabbedTouchScreen ts)  {
		
		super("Themis");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		// JPanel p = createDecoratedPanel("Simulated RPi touchscreen");
		
		//setLayout(new GridLayout(2,1,10,10));
		setLayout(new BorderLayout(10,10));
		setBackground(Color.black); // #222
		
		add(ts);
		ts.setPreferredSize(new Dimension(1600,910)); // TODO => move to resource file
	    	    
		// debug add(new SynthControllerPaneSimulator(scp));

		pack();
		setVisible(true);
		
		LOGGER.info("Starting Swing Themis simulator");
	}		
}