package application;

import java.awt.*;
import java.io.IOException;

import javax.swing.*;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

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
	
	private ControlFactory controlFactory;
	private ViewFactory viewFactory;
	private JPanel simulatorPane; // the global JPanel that hosts several subpane (one for each group of parameters) in UI simulator mode
	
	/**
	 * 
	 * @param mcpDevice a properly initialized MCP23017 bus expander, or null if only the simulator mode is to be used
	 * @param is31Device a properly initialized IS31FL3731 led driver, or null if only the simulator mode is to be used
	 * @param isSimulator if true, initializes a simulator Swing-based UI
	 * @throws IOException
	 * @throws UnsupportedBusNumberException 
	 */
	public SynthControllerPane(boolean isSimulator) throws IOException, UnsupportedBusNumberException {
		
		MCP23017 mcpDevice=null;
		IS31FL3731 is31Device=null;
		if (!isSimulator) {
			mcpDevice = new MCP23017();
			is31Device = new IS31FL3731();
		}
		this.controlFactory = new ControlFactory(mcpDevice);
		this.viewFactory = new ViewFactory(is31Device);
		
		if (isSimulator) {
			simulatorPane = SwingMain.createDecoratedPanel("Control Pane");
			simulatorPane.setLayout(new GridLayout(1,2, 10,10)); // rows, cols, hgap, vgap
		}

		createVCOPane(); 
		
		System.out.println("ViewFactory: " + viewFactory);
		
	}
	

	/**
	 * Creates the VCO Pane
	 * If simulatorPane is non null, also add UI components to it.
	 */
	private void createVCOPane() throws IOException {
		
		Vco3340 vco3340 = new Vco3340();
		Vco13700 vco13700 = new Vco13700();
		MCP23017.Port mcpPort;
		IS31FL3731.Matrix is31Matrix = IS31FL3731.Matrix.A;
		
				
		// ================================= LEFT COLUMN:3340 =================================
		mcpPort = MCP23017.Port.A;
		// encoder:
		Control c1 = controlFactory.createControl(vco3340.getDetuneParameter(), mcpPort, MCP23017.Pin.P0, MCP23017.Pin.P1);
		View v1 = viewFactory.createView(vco3340.getDetuneParameter(), is31Matrix, 0);
		
		// push:
		Control c2 = controlFactory.createControl(vco3340.getOctaveParameter(), mcpPort, MCP23017.Pin.P2);
		View v2 = viewFactory.createView(vco3340.getOctaveParameter(), is31Matrix, 6, 0);
		
		
		// encoder:
		Control c3 = controlFactory.createControl(vco3340.getWaveShapeParameter(), mcpPort, MCP23017.Pin.P3, MCP23017.Pin.P4);
		View v3 = viewFactory.createView(vco3340.getWaveShapeParameter(), is31Matrix, 1);
		
		// push:
		Control c4 = controlFactory.createControl(vco3340.getSyncFrom13700Parameter(), mcpPort, MCP23017.Pin.P5);
		View v4 = viewFactory.createView(vco3340.getSyncFrom13700Parameter(), is31Matrix, 6, 4);
		
		// encoder :
		Control c5 = controlFactory.createControl(vco3340.getDutyParameter(), mcpPort, MCP23017.Pin.P6, MCP23017.Pin.P7);
		View v5 = viewFactory.createView(vco3340.getDutyParameter(), is31Matrix, 2);

		if (simulatorPane != null) {
			JPanel panel = new JPanel();
			panel.setBackground(Color.black);
			panel.setLayout(new GridLayout(10, 1, 10,10));
			panel.add(v1.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c1));
			panel.add(v2.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c2));
			panel.add(v3.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c3));
			panel.add(v4.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c4));
			panel.add(v5.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c5));
			simulatorPane.add(panel);
		}
		
		// ================================= RIGHT COLUMN: 13700 =================================
		mcpPort = MCP23017.Port.B;
		// encoder:
		Control c6 = controlFactory.createControl(vco13700.getDetuneParameter(), mcpPort, MCP23017.Pin.P0, MCP23017.Pin.P1);
		View v6 = viewFactory.createView(vco13700.getDetuneParameter(), is31Matrix, 3);
		
		// push:
		Control c7 = controlFactory.createControl(vco13700.getOctaveParameter(), mcpPort, MCP23017.Pin.P2);
		View v7 = viewFactory.createView(vco13700.getOctaveParameter(), is31Matrix, 7, 0);
		
		
		// encoder:
		Control c8 = controlFactory.createControl(vco13700.getWaveShapeParameter(), mcpPort, MCP23017.Pin.P3, MCP23017.Pin.P4);
		View v8 = viewFactory.createView(vco13700.getWaveShapeParameter(), is31Matrix, 4);
		
		// push:
		Control c9 = controlFactory.createControl(null, mcpPort, MCP23017.Pin.P5);
		View v9 = viewFactory.createView(null, is31Matrix, 7, 4);
		
		// encoder :
		Control c10 = controlFactory.createControl(null, mcpPort, MCP23017.Pin.P6, MCP23017.Pin.P7);
		View v10 = viewFactory.createView(null, is31Matrix, 5);		// not used (reserved for FM level)
		
		if (simulatorPane != null) {
			JPanel panel = new JPanel();
			panel.setBackground(Color.black);
			panel.setLayout(new GridLayout(10, 1, 10,10));
			panel.add(v6.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c6));
			panel.add(v7.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c7));
			panel.add(v8.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c8));
			panel.add(v9.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c9));
			panel.add(v10.getUIForSimulator());
			panel.add(SwingMain.createUIForControl(c10));
			simulatorPane.add(panel);
		}
	}
	


	
	
	// -------- test --------
	public static void main(String[] args) throws Exception {
		
		SynthControllerPane scp = new SynthControllerPane(true);
		JFrame f = new JFrame("test");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setContentPane(scp.simulatorPane);
		f.pack();
		f.setVisible(true);
	}
	

}
