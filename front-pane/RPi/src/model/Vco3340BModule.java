package model;

import java.util.List;
import java.util.logging.Logger;

/** 
 * A model for a VCO based on the CEM or AS3340 device.
 * 
 * @author Bastien Fratta
 * @author S. Reynal
 */
public class Vco3340BModule extends VcoModule {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MIDIParameter triLevelParameter;
	private MIDIParameter sawLevelParameter;
	private MIDIParameter pulseLevelParameter;
	private MIDIParameter dutyParameter;
	
	public static final String DUTY = "Duty";
	public static final String TRI_LEVEL = "TriangleLvl";
	public static final String SAW_LEVEL = "SawLvl";
	public static final String PULSE_LEVEL = "PulseLvl";
	
	
	public Vco3340BModule() {
		super();
		parameterList.add(triLevelParameter = new MIDIParameter(getVcoName() + TRI_LEVEL));
		parameterList.add(sawLevelParameter = new MIDIParameter(getVcoName() + SAW_LEVEL));
		parameterList.add(pulseLevelParameter = new MIDIParameter(getVcoName() + PULSE_LEVEL));
		parameterList.add(dutyParameter = new MIDIParameter(getVcoName() + DUTY));
		
		// debug:
		triLevelParameter.addChangeListener(e -> LOGGER.info(e.toString()));
		sawLevelParameter.addChangeListener(e -> LOGGER.info(e.toString()));
		pulseLevelParameter.addChangeListener(e -> LOGGER.info(e.toString()));
		dutyParameter.addChangeListener(e -> LOGGER.info(e.toString())); 
	}
	
	protected String getVcoName() {
		return "Vco3340B";
	}

	// ---- value getters and setters --- (write operating may fire change events)
	
	public int getTriLevel() {
		return triLevelParameter.getValueAsMIDICode();
	}
	
	public void setTriLevel(int lvl) {
		triLevelParameter.setValue(lvl);
	}
	
	public int getSawLevel() {
		return sawLevelParameter.getValueAsMIDICode();
	}
	
	public void setSawLevel(int lvl) {
		sawLevelParameter.setValue(lvl);
	}

	public int getPulseLevel() {
		return pulseLevelParameter.getValueAsMIDICode();
	}
	
	public void setPulseLevel(int lvl) {
		pulseLevelParameter.setValue(lvl);
	}

	
	public double getDuty() {
		return dutyParameter.getValue();
	}

	public void setDuty(int duty) {
		this.dutyParameter.setValue(duty);
	}
		
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
		
	public MIDIParameter getTriLevelParameter() {
		return triLevelParameter;
	}

	public MIDIParameter getSawLevelParameter() {
		return sawLevelParameter;
	}

	public MIDIParameter getPulseLevelParameter() {
		return pulseLevelParameter;
	}

	public MIDIParameter getDutyParameter() {
		return dutyParameter;
	}




	
	// ------------ test -------------
	public static void main(String[] args) {
	
		Vco3340BModule vco1 = new Vco3340BModule();
		List<ModuleParameter<?>> paramsVCO1 = vco1.getParameters();
		for (ModuleParameter<?> p : paramsVCO1) {
			System.out.println(p);
		}
	}

}


