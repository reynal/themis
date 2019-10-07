package application.swing;
	
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.logging.Logger;

import javax.swing.*;
import javax.swing.border.*;
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import application.ModuleFactory;
import controller.SynthControllerPane;
import controller.component.*;
import model.*;
import model.serial.*;
import view.touchscreen.*;

/**
 * This is the main application frame in hardware mode, i.e., when a Raspberry is the current platform.
 */	
public class HardwareApp extends JFrame {
	
	private static final long serialVersionUID = 1L;
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	

	/**
	 * 
	 */
	public HardwareApp(TouchScreen ts, JMenuBar mb) {
		
		super("Themis");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		// TODO : remove JFrame decoration
		
		setBackground(Color.black); // #222
		
		setContentPane(ts);
		ts.setPreferredSize(new Dimension(1600,910));
		setJMenuBar(mb);		

		pack();
		setVisible(true);
		
		LOGGER.info("Starting Swing Themis application");
	}
	
	/**
	 *  temporary alternate approach 
	 */
	public HardwareApp(TabbedTouchScreen ts) {
		
		super("Themis");
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		// TODO : remove JFrame decoration
		
		setBackground(Color.black); // #222
		
		setContentPane(ts);
		ts.setPreferredSize(new Dimension(1600,910));

		pack();
		setVisible(true);
		
		LOGGER.info("Starting Swing Themis application");
	}
		
}
