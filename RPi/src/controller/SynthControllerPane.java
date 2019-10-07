package controller;

import java.io.IOException;
import java.util.logging.Logger;

import javax.swing.JFrame;

import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import application.ModuleFactory;
import controller.component.Control;
import controller.component.ControlFactory;
import device.IS31FL3731;
import device.MCP23017;
import view.component.View;
import view.component.ViewFactory;

/**
 * A subpart of the synth front pane dedicated to synth parameters (VCO, VCF, etc).
 * This class is essentially a builder able to link hardware together so as to create
 * a coherent chain, plus the ability to create a Simulator UI for design investigation
 * or testing purpose. 
 * 
 * @author SR
 */
public class SynthControllerPane {
	
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private final static int PANEL_COUNT = 4; // nb of panels in control pane
	private final static int ROW_COUNT = 5; // nb of row in each panel
	private Control[][] controls; // col, row
	private View[][] views; // col, row
	


	/**
	 * Creates a controller pane using the given hardware factories.  
	 */
	public SynthControllerPane(ControlFactory controlFactoryLeft, ControlFactory controlFactoryRight, ViewFactory viewFactory) {

		controls = new Control[PANEL_COUNT][ROW_COUNT];
		views = new View[PANEL_COUNT][ROW_COUNT];

		IS31FL3731.Matrix is31Matrix;
		
		ModuleFactory models = ModuleFactory.getDefault();
		
		try {
		// ================================= COLUMN #0 :3340 =================================
		is31Matrix = IS31FL3731.Matrix.A;
		// encoder:
		controls[0][0] = controlFactoryLeft.createControl(models.getVco3340().getDetuneParameter(), MCP23017.Pin.P0A, MCP23017.Pin.P1A);
		views[0][0] = viewFactory.createView(models.getVco3340().getDetuneParameter(), is31Matrix, 0);
		
		// push:.
		controls[0][1] = controlFactoryLeft.createControl(models.getVco3340().getOctaveParameter(), MCP23017.Pin.P2A);
		views[0][1] = viewFactory.createView(models.getVco3340().getOctaveParameter(), is31Matrix, 6, 0);
		
		
		// encoder:
		controls[0][2] = controlFactoryLeft.createControl(models.getVco3340().getWaveShapeParameter(), MCP23017.Pin.P3A, MCP23017.Pin.P4A);
		views[0][2] = viewFactory.createView(models.getVco3340().getWaveShapeParameter(), is31Matrix, 1, 0);
		
		// push:
		controls[0][3] = controlFactoryLeft.createControl(models.getVco3340().getSyncFrom13700Parameter(), MCP23017.Pin.P5A);
		views[0][3] = viewFactory.createView(models.getVco3340().getSyncFrom13700Parameter(), is31Matrix, 6, 4);
		
		// encoder :
		controls[0][4] = controlFactoryLeft.createControl(models.getVco3340().getDutyParameter(), MCP23017.Pin.P6A, MCP23017.Pin.P7A);
		views[0][4] = viewFactory.createView(models.getVco3340().getDutyParameter(), is31Matrix, 2);

		
		// ================================= COLUMN #1: 13700 =================================
		is31Matrix = IS31FL3731.Matrix.A;
		// encoder:
		controls[1][0] = controlFactoryLeft.createControl(models.getVco13700().getDetuneParameter(), MCP23017.Pin.P0B, MCP23017.Pin.P1B);
		views[1][0] = viewFactory.createView(models.getVco13700().getDetuneParameter(), is31Matrix, 3);
		
		// push:
		controls[1][1] = controlFactoryLeft.createControl(models.getVco13700().getOctaveParameter(), MCP23017.Pin.P2B);
		views[1][1] = viewFactory.createView(models.getVco13700().getOctaveParameter(), is31Matrix, 7, 0);
		
		
		// encoder:
		controls[1][2] = controlFactoryLeft.createControl(models.getVco13700().getWaveShapeParameter(), MCP23017.Pin.P3B, MCP23017.Pin.P4B);
		views[1][2] = viewFactory.createView(models.getVco13700().getWaveShapeParameter(), is31Matrix, 4);
		
		// push:
		controls[1][3] = controlFactoryLeft.createControl(null, MCP23017.Pin.P5B);
		views[1][3] = viewFactory.createView(null, is31Matrix, 7, 4);
		
		// encoder :
		controls[1][4] = controlFactoryLeft.createControl(null, MCP23017.Pin.P6B, MCP23017.Pin.P7B);
		views[1][4] = viewFactory.createView(null, is31Matrix, 5);		// not used (reserved for FM level)		
		System.out.println("ViewFactory: " + viewFactory);
		
		// ================================= COLUMN #2 : VCA ? =================================
		is31Matrix = IS31FL3731.Matrix.B;
		// encoder:
		controls[2][0] = controlFactoryRight.createControl(null, MCP23017.Pin.P0A, MCP23017.Pin.P1A);
		views[2][0] = viewFactory.createView(null, is31Matrix, 0);
		
		// push:
		controls[2][1] = controlFactoryRight.createControl(null, MCP23017.Pin.P2A);
		views[2][1] = viewFactory.createView(null, is31Matrix, 6, 0);
		
		
		// encoder:
		controls[2][2] = controlFactoryRight.createControl(null, MCP23017.Pin.P3A, MCP23017.Pin.P4A);
		views[2][2] = viewFactory.createView(null, is31Matrix, 1);
		
		// push:
		controls[2][3] = controlFactoryRight.createControl(null, MCP23017.Pin.P5A);
		views[2][3] = viewFactory.createView(null, is31Matrix, 6, 4);
		
		// encoder :
		controls[2][4] = controlFactoryRight.createControl(null, MCP23017.Pin.P6A, MCP23017.Pin.P7A);
		views[2][4] = viewFactory.createView(null, is31Matrix, 2);

		
		// ================================= COLUMN #3: VCF ? =================================
		is31Matrix = IS31FL3731.Matrix.B;
		// encoder:
		controls[3][0] = controlFactoryRight.createControl(null, MCP23017.Pin.P0B, MCP23017.Pin.P1B);
		views[3][0] = viewFactory.createView(null, is31Matrix, 3);
		
		// push:
		controls[3][1] = controlFactoryRight.createControl(null, MCP23017.Pin.P2B);
		views[3][1] = viewFactory.createView(null, is31Matrix, 7, 0);
		
		
		// encoder:
		controls[3][2] = controlFactoryRight.createControl(null, MCP23017.Pin.P3B, MCP23017.Pin.P4B);
		views[3][2] = viewFactory.createView(null, is31Matrix, 4);
		
		// push:
		controls[3][3] = controlFactoryRight.createControl(null, MCP23017.Pin.P5B);
		views[3][3] = viewFactory.createView(null, is31Matrix, 7, 4);
		
		// encoder :
		controls[3][4] = controlFactoryRight.createControl(null, MCP23017.Pin.P6B, MCP23017.Pin.P7B);
		views[3][4] = viewFactory.createView(null, is31Matrix, 5);		// not used (reserved for FM level)		
		System.out.println("ViewFactory: " + viewFactory);		
		
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Return the control (aka rotary encoder or pushbutton) at column "col" and row "row"
	 */
	public Control getControl(int col, int row) {
		return controls[col][row];
	}
	
	/**
	 * Return the view (aka bargraphs) at column "col" and row "row"
	 */
	public View getView(int col, int row) {
		return views[col][row];
	}
	
	/**
	 * @return the number of control and view columns in this pane
	 */
	public int getColumnCount() {
		return PANEL_COUNT;
	}

	/**
	 * @return the number of control and view columns in this pane
	 */
	public int getRowCount() {
		return ROW_COUNT;
	}	

	
	

}
