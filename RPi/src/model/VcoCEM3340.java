package model;

import java.util.List;

import controller.component.Control;
import javafx.scene.Node;

public class VcoCEM3340 extends Vco{

	enum WaveShape {
		
		SQUARE,
		TRIANGLE,
		PULSE,
		SAWTOOTH;
	}
	
	private WaveShape shape;
	private boolean syncFrom13700;

	// =========================================
	
	public static void main(String[] args) {
		
		/*VcoCEM3340 vco = new VcoCEM3340();
		List<SynthParameter> params = vco.getParameters();		
		for (SynthParameter p : params) {
			Control c = p.getControl();
			Node n = c.getJavaFXView();
			
		}*/
	}
	
}
