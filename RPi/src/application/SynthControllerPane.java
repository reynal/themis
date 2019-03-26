package application;

import java.awt.*;
import java.io.IOException;

import javax.swing.*;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import controller.component.*;
import device.*;
import model.*;
import view.component.*;
import model.spi.*;

/**
 * A subpart of the synth front pane dedicated to synth parameters (VCO, VCF, etc).
 * This class is essentially a builder able to link hardware together so as to create
 * a coherent chain, plus the ability to create a Simulator UI for design investigation
 * or testing purpose.
 * 
 * @author SR
 */
public class SynthControllerPane {
	
	private ControlFactory controlFactory1,controlFactory2;
	private ViewFactory viewFactory;
	private JPanel simulatorPane; // the global JPanel that hosts several subpane (one for each group of parameters) in UI simulator mode
	private final static int columnCount = 4; // nb of panels in control pane
	private Control[][] cArray;
	private View[][] vArray;
	
	/**
	 * Creates the controller pane. This class is supposed to be a singleton. 
	 * @param isSimulator if true, initializes a simulator Swing-based UI
	 */
	public SynthControllerPane(boolean isSimulator, SpiTransmitter spiTransmitter) throws IOException, UnsupportedBusNumberException {
		
		MCP23017 mcpDevice1=null;
		MCP23017 mcpDevice2=null;
		IS31FL3731 is31Device=null;
		if (!isSimulator) {
			mcpDevice1 = new MCP23017();
			mcpDevice2 = new MCP23017();
			is31Device = new IS31FL3731();
		}
		this.controlFactory1 = new ControlFactory(mcpDevice1);
		this.controlFactory2 = new ControlFactory(mcpDevice2);
		this.viewFactory = new ViewFactory(is31Device);
		cArray = new Control[columnCount][5];
		vArray = new View[columnCount][5];

		Vco3340 vco3340 = new Vco3340();
		Vco13700 vco13700 = new Vco13700();
		//spiTransmitter.initParameterIdHashMap(vco3340, vco13700);
		MCP23017.Port mcpPort;
		IS31FL3731.Matrix is31Matrix;
		
				
		// ================================= COLUMN #0 :3340 =================================
		is31Matrix = IS31FL3731.Matrix.A;
		mcpPort = MCP23017.Port.A;
		// encoder:
		cArray[0][0] = controlFactory1.createControl(vco3340.getDetuneParameter(), mcpPort, MCP23017.Pin.P0, MCP23017.Pin.P1);
		vArray[0][0] = viewFactory.createView(vco3340.getDetuneParameter(), is31Matrix, 0);
		
		// push:
		cArray[0][1] = controlFactory1.createControl(vco3340.getOctaveParameter(), mcpPort, MCP23017.Pin.P2);
		vArray[0][1] = viewFactory.createView(vco3340.getOctaveParameter(), is31Matrix, 6, 0);
		
		
		// encoder:
		cArray[0][2] = controlFactory1.createControl(vco3340.getWaveShapeParameter(), mcpPort, MCP23017.Pin.P3, MCP23017.Pin.P4);
		vArray[0][2] = viewFactory.createView(vco3340.getWaveShapeParameter(), is31Matrix, 1);
		
		// push:
		cArray[0][3] = controlFactory1.createControl(vco3340.getSyncFrom13700Parameter(), mcpPort, MCP23017.Pin.P5);
		vArray[0][3] = viewFactory.createView(vco3340.getSyncFrom13700Parameter(), is31Matrix, 6, 4);
		
		// encoder :
		cArray[0][4] = controlFactory1.createControl(vco3340.getDutyParameter(), mcpPort, MCP23017.Pin.P6, MCP23017.Pin.P7);
		vArray[0][4] = viewFactory.createView(vco3340.getDutyParameter(), is31Matrix, 2);

		
		// ================================= COLUMN #1: 13700 =================================
		is31Matrix = IS31FL3731.Matrix.A;
		mcpPort = MCP23017.Port.B;
		// encoder:
		cArray[1][0] = controlFactory1.createControl(vco13700.getDetuneParameter(), mcpPort, MCP23017.Pin.P0, MCP23017.Pin.P1);
		vArray[1][0] = viewFactory.createView(vco13700.getDetuneParameter(), is31Matrix, 3);
		
		// push:
		cArray[1][1] = controlFactory1.createControl(vco13700.getOctaveParameter(), mcpPort, MCP23017.Pin.P2);
		vArray[1][1] = viewFactory.createView(vco13700.getOctaveParameter(), is31Matrix, 7, 0);
		
		
		// encoder:
		cArray[1][2] = controlFactory1.createControl(vco13700.getWaveShapeParameter(), mcpPort, MCP23017.Pin.P3, MCP23017.Pin.P4);
		vArray[1][2] = viewFactory.createView(vco13700.getWaveShapeParameter(), is31Matrix, 4);
		
		// push:
		cArray[1][3] = controlFactory1.createControl(null, mcpPort, MCP23017.Pin.P5);
		vArray[1][3] = viewFactory.createView(null, is31Matrix, 7, 4);
		
		// encoder :
		cArray[1][4] = controlFactory1.createControl(null, mcpPort, MCP23017.Pin.P6, MCP23017.Pin.P7);
		vArray[1][4] = viewFactory.createView(null, is31Matrix, 5);		// not used (reserved for FM level)		
		System.out.println("ViewFactory: " + viewFactory);
		
		// ================================= COLUMN #2 : VCA ? =================================
		is31Matrix = IS31FL3731.Matrix.B;
		mcpPort = MCP23017.Port.A;
		// encoder:
		cArray[2][0] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P0, MCP23017.Pin.P1);
		vArray[2][0] = viewFactory.createView(null, is31Matrix, 0);
		
		// push:
		cArray[2][1] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P2);
		vArray[2][1] = viewFactory.createView(null, is31Matrix, 6, 0);
		
		
		// encoder:
		cArray[2][2] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P3, MCP23017.Pin.P4);
		vArray[2][2] = viewFactory.createView(null, is31Matrix, 1);
		
		// push:
		cArray[2][3] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P5);
		vArray[2][3] = viewFactory.createView(null, is31Matrix, 6, 4);
		
		// encoder :
		cArray[2][4] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P6, MCP23017.Pin.P7);
		vArray[2][4] = viewFactory.createView(null, is31Matrix, 2);

		
		// ================================= COLUMN #3: VCF ? =================================
		is31Matrix = IS31FL3731.Matrix.B;
		mcpPort = MCP23017.Port.B;
		// encoder:
		cArray[3][0] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P0, MCP23017.Pin.P1);
		vArray[3][0] = viewFactory.createView(null, is31Matrix, 3);
		
		// push:
		cArray[3][1] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P2);
		vArray[3][1] = viewFactory.createView(null, is31Matrix, 7, 0);
		
		
		// encoder:
		cArray[3][2] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P3, MCP23017.Pin.P4);
		vArray[3][2] = viewFactory.createView(null, is31Matrix, 4);
		
		// push:
		cArray[3][3] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P5);
		vArray[3][3] = viewFactory.createView(null, is31Matrix, 7, 4);
		
		// encoder :
		cArray[3][4] = controlFactory2.createControl(null, mcpPort, MCP23017.Pin.P6, MCP23017.Pin.P7);
		vArray[3][4] = viewFactory.createView(null, is31Matrix, 5);		// not used (reserved for FM level)		
		System.out.println("ViewFactory: " + viewFactory);		
	}
	
	/**
	 * 
	 */
	public JPanel getSimulatorPane() {
		
		if (simulatorPane != null) return simulatorPane;
		
		simulatorPane = SwingMain.createDecoratedPanel("Control Pane");
		simulatorPane.setLayout(new GridLayout(1,columnCount, 10,10)); // rows, cols, hgap, vgap
		
		JPanel panel; 
		
		for (int col=0; col < columnCount; col++) {
			panel = new JPanel();
			panel.setBackground(Color.black);
			panel.setLayout(new GridLayout(10, 1, 10,10));
			for (int row=0; row < 5; row++) {
				panel.add(vArray[col][row].getUIForSimulator());
				panel.add(SwingMain.createUIForControl(cArray[col][row]));
			}
			simulatorPane.add(panel);
		}
		
		return simulatorPane;
	}

	
	// -------- test --------
	public static void main(String[] args) throws Exception {
		
		SynthControllerPane scp = new SynthControllerPane(true, null);
		JFrame f = new JFrame("test");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setContentPane(scp.getSimulatorPane());
		f.pack();
		f.setVisible(true);
	}
	

}
