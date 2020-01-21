package application.swing;

import java.awt.Color;
import java.awt.Font;
import java.awt.GridLayout;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.LineBorder;
import javax.swing.border.TitledBorder;

import controller.SynthControllerPane;
import controller.component.ControlFactory;
import view.component.View;
import view.component.ViewFactory;

/**
 * Constructs a Swing based simulator UI for a SynthControllerPane, for example
 * if no hardware is currently plugged to the Raspberry, or for debugging and
 * testing purpose.
 * 
 * This is practically implemented as a JFrame that can hosts several subpanes,
 * one for each group of parameters.
 * 
 * @author reynal
 *
 */
@SuppressWarnings("serial")
public class SynthControllerPaneSimulator extends JFrame {

	public SynthControllerPaneSimulator(SynthControllerPane synthControllerPane) {
		
		super("ThEmIS SynthControllerPaneSimulator");
		setDefaultCloseOperation(EXIT_ON_CLOSE);

		JPanel mainPanel = new JPanel();
		mainPanel.setBackground(Color.black);
		mainPanel.setBorder(BorderFactory.createTitledBorder(
				new LineBorder(Color.gray, 1), "Synth controller pane simulator",
					TitledBorder.CENTER, TitledBorder.DEFAULT_POSITION, new Font("SansSerif", Font.BOLD, 10), Color.white));

		mainPanel.setLayout(new GridLayout(1, synthControllerPane.getColumnCount(), 10, 10)); // rows, cols, hgap, vgap

		for (int col = 0; col < synthControllerPane.getColumnCount(); col++) {
			JPanel panel = new JPanel();
			panel.setBackground(Color.black);
			panel.setLayout(new GridLayout(2 * synthControllerPane.getRowCount(), 1, 10, 10));
			for (int row = 0; row < synthControllerPane.getRowCount(); row++) {
				View v = synthControllerPane.getView(col,row);
				panel.add(v != null ? v.getUIForSimulator() : new JLabel("???"));
				panel.add(UIUtilities.createUIForControl(synthControllerPane.getControl(col,row)));
			}
			mainPanel.add(panel);
		}
		
		setContentPane(mainPanel);
		setLocation(0,450);
		pack();
		setVisible(true);
	}

	// -------- test --------
	
	public static void main(String[] args) throws Exception {

		ControlFactory controlFactoryLeft = new ControlFactory(null); // one factory for each MCP device
		ControlFactory controlFactoryRight = new ControlFactory(null);
		ViewFactory viewFactory = new ViewFactory(null);		
		SynthControllerPane scp = new SynthControllerPane(controlFactoryLeft, controlFactoryRight, viewFactory);
		SynthControllerPaneSimulator scps = new SynthControllerPaneSimulator(scp);
		scps.setDefaultCloseOperation(EXIT_ON_CLOSE);
	}
}
