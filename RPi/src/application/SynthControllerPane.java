package application;

import java.awt.*;
import java.io.IOException;

import javax.swing.*;
import controller.component.*;
import device.*;
import model.*;
import view.component.*;

/**
 * A subpart of the synth front pane dedicated to synth parameters (VCO, VCF, etc).
 * This class is essentially a UI builder.
 * 
 * @author sydxrey
 */
public class SynthControllerPane {
	
	//private Control[][] swControls= new Control[4][5]; // 4 cols, 5 rows, where each col = {rotary+push, rotary+push, rotary}
	private ControlFactory controlFactory;
	private ViewFactory viewFactory;
	private JPanel swPane; // the global JPanel that hosts several subpane (one for each group of parameters) in UI simulator mode
	
	/**
	 * 
	 * @param mcpDevice a properly initialized MCP23017 bus expander, or null if only the simulator mode is to be used
	 * @param is31Device a properly initialized IS31FL3731 led driver, or null if only the simulator mode is to be used
	 * @param isSimulator if true, initializes a simulator Swing-based UI
	 * @throws IOException
	 */
	public SynthControllerPane(boolean isSimulator) throws IOException {
		
		MCP23017 mcpDevice;
		IS31FL3731 is31Device;
		if (!isSimulator) {
			mcpDevice = new MCP23017();
			is31Device = new IS31FL3731();
		}
		this.controlFactory = new ControlFactory(mcpDevice);
		this.viewFactory = new ViewFactory(is31Device);
		
		// instantiate models 
		Vco3340 vco3340 = new Vco3340();		
		Vcf3320 vcf3320 = new Vcf3320();

		// instantiate encoders
		if (isSimulator) {
			swPane = SwingMain.createDecoratedPanel("SW");
			swPane.setLayout(new GridLayout(1,4, 10,10)); // rows, cols, hgap, vgap
		}

		JPanel panel = null;
		
		panel = initPaneColumn(isSimulator, 0, 
				vco3340.getDetuneParameter(), 
				vco3340.getSyncFrom13700Parameter(), 
				vco3340.getWaveShapeParameter(), 
				vco3340.getOctaveParameter(), 
				vco3340.getDutyParameter());
		if (isSimulator) swPane.add(panel);
			
		panel = initPaneColumn(isSimulator, 0, 
				vcf3320.getCutoffParameter(), 
				vcf3320.getEgDepthParameter(),
				vcf3320.getKbdTrackingParameter(),
				vcf3320.getFilterOrderParameter(),
				vcf3320.getResonanceParameter());
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
	 * @param isSimulator if true, returns a JPanel corresponding for the UI simulator, otherwise returns null and simply performs the hw component initialization
	 * @throws IOException if there's an error at the hardware (aka I2C) level
	 */
	JPanel initPaneColumn(boolean isSimulator, int col, MIDIParameter param1, SynthParameter<?> param2, MIDIParameter param3, SynthParameter<?> param4, MIDIParameter param5) throws IOException {
		
		Control c1, c2, c3, c4, c5;
		View v1, v2, v3, v4, v5;
		JPanel panel = null;
		
		if (isSimulator) {
			panel = new JPanel();
			panel.setBackground(Color.black);
			panel.setLayout(new GridLayout(5, 2, 10,10));
		}
		
		c1 = controlFactory.createControl(param1, MCP23017.Pin.P0, MCP23017.Pin.P1);				
		c2 = controlFactory.createControl(param2, MCP23017.Pin.P2);
		c3 = controlFactory.createControl(param3, MCP23017.Pin.P3, MCP23017.Pin.P4);				
		c4 = controlFactory.createControl(param4, MCP23017.Pin.P5);
		c5 = controlFactory.createControl(param5, MCP23017.Pin.P6, MCP23017.Pin.P7);
		
		v1 = viewFactory.createView(param1, IS31FL3731.Matrix.A, 0);
		v2 = viewFactory.createView(param2, IS31FL3731.Matrix.A, 0);
		v3 = viewFactory.createView(param3, IS31FL3731.Matrix.A, 1);
		
		v5 = viewFactory.createView(param5, IS31FL3731.Matrix.A, 2);

		if (panel != null) {
			panel.add(SwingMain.createUIForControl(c1));
			panel.add(v1.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c2));
			panel.add(v2.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c3));
			panel.add(v3.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c4));
			panel.add(v4.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c5));
			panel.add(v5.getUIForSimulator());
		}
		
		return panel;
	}
	
	// UI simul


	
	
	// -------- test --------
	public static void main(String[] args) throws IOException {
		
		SynthControllerPane scp = new SynthControllerPane(null, null, true);
		JFrame f = new JFrame("test");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setContentPane(scp.swPane);
		f.pack();
		f.setVisible(true);
	}
	

}
