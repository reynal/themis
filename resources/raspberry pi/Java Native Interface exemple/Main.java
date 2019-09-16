package pulsar;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class Main extends JFrame {
	
	enum Param {
		VCA_A,VCA_D, VCA_S, VCA_R, VCF_A,VCF_D, VCF_S, VCF_R;
	}
	
	Param active_param;
	JSlider jslider;
	JLabel activeParamLBL;
	
	public Main(){
		super("PULSAR");
		System.loadLibrary("pulsar"); // Load native library at runtime : libpulsar.so
		setLayout(new BorderLayout());
		add(new Pane(), BorderLayout.CENTER);
		jslider = new JSlider(SwingConstants.VERTICAL, 0, 100, 50);
		jslider.addChangeListener(e -> changeActiveParameter());
		add(jslider, BorderLayout.WEST);
		activeParamLBL = new JLabel();
		add(activeParamLBL, BorderLayout.NORTH);
		setActiveParam(Param.VCA_A);
		setSize(800,400);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setVisible(true);
	}
	
	private void changeActiveParameter() {
		if (active_param != null){
			System.out.println(active_param + " -> " + jslider.getValue());
			setParameter(active_param.ordinal(), jslider.getValue());
		}
	}

	public static void main(String[] args){
		new Main();
	}
	
	class Pane extends JPanel {
		
		Pane(){
			setLayout(new GridLayout(2,6));
			addButton(this, "Attack VCA", e -> setActiveParam(Param.VCA_A));
			addButton(this, "Decay VCA", e -> setActiveParam(Param.VCA_D));
			addButton(this, "Sustain VCA", e -> setActiveParam(Param.VCA_S));
			addButton(this, "Release VCA", e -> setActiveParam(Param.VCA_R));
			JToggleButton tb = new JToggleButton("ON");
			tb.addActionListener(e -> {if (((JToggleButton)e.getSource()).isSelected()) start(); else stop();});
			add(tb);
			
			addButton(this, "Attack VCF", e -> setActiveParam(Param.VCF_A));
			addButton(this, "Decay VCF", e -> setActiveParam(Param.VCF_D));
			addButton(this, "Sustain VCF", e -> setActiveParam(Param.VCF_S));
			addButton(this, "Release VCF", e -> setActiveParam(Param.VCF_R));
			addButton(this, "Calibrate", e -> calibrate());
		}

	}
	
	void setActiveParam(Param p){
		active_param = p;
		activeParamLBL.setText(active_param.toString());
	}
	
	void addButton(JPanel p, String label, ActionListener e){
		JButton b;
		p.add(b = new JButton(label));
		b.addActionListener(e);				
	}
	
	native void stop();

	native void start();

	native void setParameter(int paramNumber, int ratio);
	
	native void calibrate();
}
