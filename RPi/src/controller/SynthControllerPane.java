package controller;

import java.awt.*;
import javax.swing.*;
import application.*;
import controller.component.Control;
import model.*;

/**
 * A subpart of the synth front pane dedicated to synth parameters (VCO, VCF, etc).
 * This class is essentially a UI builder.
 * 
 * @author sydxrey
 */
public class SynthControllerPane {
	
	/**
	 * Build the SouthWest pane
	 */
	public static JPanel createSWPane() {
		
		JPanel p = SwingMain.createDecoratedPanel("SW");
		p.setLayout(new GridLayout(5,2, 10,10)); // rows, cols, hgap, vgap
		
		VcoCEM3340 vco1 = new VcoCEM3340();
		add(p, vco1.getShapeParameter());
		add(p, vco1.getDutyParameter());
		add(p, vco1.getSyncFrom13700Parameter());
		
		return p;
	}
	
	// utility
	private static void add(JPanel p, SynthParameter<?> param) {
		Control c = param.getControl(); 
		p.add(c.getSwingView());
		JLabel label = new JLabel(param.getLabel());
		label.setForeground(Color.pink);
		p.add(label);
		param.addSynthParameterEditListener(e -> System.out.println(e));
	}
	
	// -------- test --------
	public static void main(String[] args) {
		
		JFrame f = new JFrame("test");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setContentPane(createSWPane());
		f.pack();
		f.setVisible(true);
	}
	

}
