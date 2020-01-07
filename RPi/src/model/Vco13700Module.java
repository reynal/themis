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
	
	private MIDIParameter triLevelParameter, squLevelParameter;
	
	public static final String TRI_LEVEL = "Triangle";
	public static final String SQU_LEVEL = "Square";
	// TODO : subbass level
		
	public Vco13700Module() {
		super();
		parameterList.add(triLevelParameter = new MIDIParameter(TRI_LEVEL));
		parameterList.add(squLevelParameter = new MIDIParameter(SQU_LEVEL));
		
		// debug:
		triLevelParameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString()));
		squLevelParameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString()));
	}
	
	protected String getVcoName() {
		return "Vco13700";
	}
	
		
	// ---- value getters and setters --- (write operating may fire change events)
	
	public int getTriLevel() {
		return triLevelParameter.getValueAsMIDICode();
	}
	
	public void setTriLevel(int lvl) {
		triLevelParameter.setValue(lvl);
	}
	
	public int getSquLevel() {
		return squLevelParameter.getValueAsMIDICode();
	}
	
	public void setSquLevel(int lvl) {
		squLevelParameter.setValue(lvl);
	}
	
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	public MIDIParameter getTriLevelParameter() {
		return triLevelParameter;
	}

	public MIDIParameter getSquLevelParameter() {
		return squLevelParameter;
	}

}
