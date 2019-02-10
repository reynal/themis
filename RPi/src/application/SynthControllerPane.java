package application;

import java.awt.*;
import java.io.IOException;

import javax.swing.*;
import controller.component.*;
import device.*;
import model.*;

/**
 * A subpart of the synth front pane dedicated to synth parameters (VCO, VCF, etc).
 * This class is essentially a UI builder.
 * 
 * @author sydxrey
 */
public class SynthControllerPane {
	
	Control[][] swControls= new Control[4][5]; // 4 cols, 5 rows, where each col = {rotary+push, rotary+push, rotary}
	MCP23017 mcpDevice;
	JPanel swPane;
	
	public SynthControllerPane(MCP23017 mcpDevice, boolean isSimulator) throws IOException {
		
		this.mcpDevice = mcpDevice;
		
		// instantiate models 
		Vco3340 vco3340 = new Vco3340();
		for (SynthParameter<?> param : vco3340.getParameters())
			param.addSynthParameterEditListener(e -> System.out.println(e)); // SR TODO : should be a BarGraph

		Vcf3320 vcf3320 = new Vcf3320();
		for (SynthParameter<?> param : vcf3320.getParameters())
			param.addSynthParameterEditListener(e -> System.out.println(e)); 

		// instantiate encoders
		if (isSimulator) {
			swPane = SwingMain.createDecoratedPanel("SW");
			swPane.setLayout(new GridLayout(1,4, 10,10)); // rows, cols, hgap, vgap
		}

		JPanel panel = null;
		
		panel = initPaneColumn(isSimulator, 0, vco3340.getDetuneParameter(), vco3340.getSyncFrom13700Parameter(), vco3340.getWaveShapeParameter(), vco3340.getOctaveParameter(), vco3340.getDutyParameter());
		if (isSimulator) swPane.add(panel);
			
		panel = initPaneColumn(isSimulator, 0, vcf3320.getCutoffParameter(), vcf3320.getEgDepthParameter(),vcf3320.getKbdTrackingParameter(),vcf3320.getFilterOrderParameter(),vcf3320.getResonanceParameter());
		if (isSimulator) swPane.add(panel);
	}
	
	/**
	 * 
	 * @param col from 0 to 11
	 * @param param1 a SynthParameter for the first rotary encoder
	 * @param param2 a SynthParameter for the push button of the first rotary encoder 
	 * @param param3 a SynthParameter for the second rotary encoder
	 * @param param4 a SynthParameter for the push button of the second rotary encoder
	 * @param param5 a SynthParameter for the third rotary encoder (which has no push capability)
	 * @param isSimulator if true, returns a JPanel corresponding for the UI simulator, otherwise returns null
	 * @throws IOException
	 */
	JPanel initPaneColumn(boolean isSimulator, int col, SynthParameter<?> param1, SynthParameter<?> param2, SynthParameter<?> param3, SynthParameter<?> param4, SynthParameter<?> param5) throws IOException {
		
		RotaryEncoder re;
		PushButton pb;
		JPanel panel = null;
		
		if (isSimulator) {
			panel = new JPanel();
			panel.setBackground(Color.black);
			panel.setLayout(new GridLayout(5, 1, 10,10));
		}
		
		re = new RotaryEncoder(param1.getLabel(), mcpDevice, MCP23017.Pin.P0, MCP23017.Pin.P1);
		re.addRotaryEncoderChangeListener(param1);
		if (panel != null) panel.add(SwingMain.createUIForControl(re));
		swControls[col][0]=re;
				
		pb = new PushButton(param2.getLabel(), mcpDevice, MCP23017.Pin.P2);
		pb.addPushButtonActionListener(param2);
		if (panel != null) panel.add(SwingMain.createUIForControl(pb));
		swControls[col][1]=pb;
		
		re = new RotaryEncoder(param3.getLabel(), mcpDevice, MCP23017.Pin.P3, MCP23017.Pin.P4);
		re.addRotaryEncoderChangeListener(param3);
		if (panel != null) panel.add(SwingMain.createUIForControl(re));
		swControls[col][2]=re;
		
		pb = new PushButton(param4.getLabel(), mcpDevice, MCP23017.Pin.P5);
		pb.addPushButtonActionListener(param4);
		if (panel != null) panel.add(SwingMain.createUIForControl(pb));
		swControls[col][3]=pb;
		
		re = new RotaryEncoder(param5.getLabel(), mcpDevice, MCP23017.Pin.P6, MCP23017.Pin.P7);
		re.addRotaryEncoderChangeListener(param5);
		if (panel != null) panel.add(SwingMain.createUIForControl(re));
		swControls[col][4]=re;
		
		return panel;
	}
	


	
	
	// -------- test --------
	public static void main(String[] args) throws IOException {
		
		SynthControllerPane scp = new SynthControllerPane(null, true);
		JFrame f = new JFrame("test");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setContentPane(scp.swPane);
		f.pack();
		f.setVisible(true);
	}
	

}
