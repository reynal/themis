package model;

import java.util.logging.Logger;

/**
 * A model for a VCO module based on the LM13700 OTA integrated circuit.
 * 
 * @author Bastien Fratta
 * @author Reynal
 *
 */
public class Vco13700Module extends VcoModule {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MIDIParameter waveShapeParameter;
	
	public static final String WAVE = "Shape";
	
	public Vco13700Module() {
		super();
		parameterList.add(waveShapeParameter = new MIDIParameter(WAVE));
		
		// debug:
		waveShapeParameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString())); 
	}
	
	protected String getVcoName() {
		return "Vco13700";
	}
	
		
	enum WaveShape {
		
		SQUARE,
		TRIANGLE;
	}

	// ---- value getters and setters --- (write operating may fire change events)
	
	public int getWaveShape() {
		return waveShapeParameter.getValueAsMIDICode();
	}
	
	public void setWaveShape(int waveshape) {
		waveShapeParameter.setValue(waveshape);
	}
	
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	public MIDIParameter getWaveShapeParameter() {
		return waveShapeParameter;
	}

}
