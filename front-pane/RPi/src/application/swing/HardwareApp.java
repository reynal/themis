package application.swing;
	
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.util.logging.Logger;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenuBar;
import javax.swing.SwingUtilities;

import application.Preferences;

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
		setLayout(new BorderLayout());
		Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		 
		if ("N".equalsIgnoreCase(Preferences.getPreferences().getStringProperty(Preferences.Key.DECORATE_JFRAME))) {
			this.setUndecorated(true); // aka JWindow
			screenSize.height = screenSize.height-40;
		}
		else {
			screenSize.height = screenSize.height-80;
		}
		setPreferredSize(screenSize);
		
		setBackground(Color.black); // #222
		
		add(ts, BorderLayout.CENTER);

		pack();
		setVisible(true);
		
		LOGGER.info("Starting Swing Themis application");
	}
	
		
}
