import java.awt.*;

import javax.swing.*;

public class MidiCCPanel extends JFrame {
	
	private static final long serialVersionUID = 1L;
	
	SerialDebugger themisSerialDebugger;

	public MidiCCPanel(SerialDebugger themisSerialDebugger) {
		
		super("MidiCC Panel for Themis");
		this.themisSerialDebugger = themisSerialDebugger;
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		setLayout(new GridLayout(1,1));
		
		add(createMidiCCSlider("PWM_3340", 14));
		
		pack();
		setVisible(true);
	}
	
	JPanel createMidiCCSlider(String midiCCName, int midiCC) {
		
		JPanel p = new JPanel();
		p.setLayout(new BorderLayout());
		
		JLabel title = new JLabel(midiCCName, SwingConstants.CENTER);
		p.add(title, BorderLayout.NORTH);

		JSlider js = new JSlider(JSlider.VERTICAL, 0, 127, 64);
		p.add(js, BorderLayout.CENTER);
		
		JLabel lbl = new JLabel("??", SwingConstants.CENTER);
		p.add(lbl, BorderLayout.SOUTH);
		
		js.addChangeListener(e -> {
			//System.out.println(e);
			int v = ((JSlider)e.getSource()).getValue();
			lbl.setText(Integer.toString(v));
			send(midiCC, v);
			});
		return p;
	}
	
	void send(int midiCC, int midiValue) {
		String s = "176 " + midiCC + " " + midiValue;
		if (themisSerialDebugger != null)
			themisSerialDebugger.send(s);
		System.out.println("Sending " + s);
		
	}
	
	// -----------------------------------------------

	public static void main(String[] args) {

		new MidiCCPanel(null);


	}	

}

