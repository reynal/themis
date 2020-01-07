package application.swing;
	
import java.awt.Color;
import java.awt.Dimension;
import java.util.logging.Logger;

import javax.swing.JFrame;
import javax.swing.JMenuBar;

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
