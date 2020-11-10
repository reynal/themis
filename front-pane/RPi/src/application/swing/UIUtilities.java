package application.swing;

import java.awt.*;

import javax.swing.*;
import javax.swing.border.*;

import controller.component.*;


public class UIUtilities {

	/**
	 * Return an appropriate Swing component for the given physical control
	 * that may be used inside a UI simulator.
	 */
	public static JComponent createUIForControl(Control c) {
		
		if (c instanceof PushButton) {
			
			JButton b = new JButton(c.getLabel());
			// simply transmits events from JButton to PushButton:
			b.addActionListener(e -> ((PushButton)c).fireActionEvent(((JButton)e.getSource()).isSelected() ? PushButtonState.PRESSED : PushButtonState.RELEASED ));
			return b;

		}
		else if (c instanceof RotaryEncoder) {
			
			JPanel p = new JPanel();
			p.setBackground(Color.black);
			p.setLayout(new BorderLayout());
			JButton butMinus = new JButton("-");
			p.add(butMinus, BorderLayout.WEST);
			JLabel lbl = new JLabel(c.getLabel());
			lbl.setForeground(Color.white);
			p.add(lbl, BorderLayout.CENTER);
			JButton butPlus = new JButton("+");
			p.add(butPlus, BorderLayout.EAST);
			butPlus.addActionListener(e -> ((RotaryEncoder)c).fireChangeEvent(RotaryEncoder.Direction.UP));
			butMinus.addActionListener(e -> ((RotaryEncoder)c).fireChangeEvent(RotaryEncoder.Direction.DOWN));
			return p;			
			
		}
		else return null;
	}
	
	
			
	public static JPanel createDecoratedPanel(String title) {
		JPanel p = new JPanel();
        //pads.setPadding(new Insets(80));
        //pads.setHgap(10);
        //pads.setVgap(10);
		p.setBackground(Color.black);
		p.setBorder(BorderFactory.createTitledBorder(
				new LineBorder(Color.gray, 1), 
				title, 
				TitledBorder.CENTER, 
				TitledBorder.DEFAULT_POSITION, 
				new Font("SansSerif", Font.BOLD, 10), 
				Color.white ));
		return p;
	}
		
	
}
