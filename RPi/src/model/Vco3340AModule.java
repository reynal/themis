package model;

import java.util.List;
import java.util.logging.Logger;

/** 
 * A model for a VCO based on the CEM or AS3340 device.
 * 
 * @author Bastien Fratta
 * @author S. Reynal
 */
public class Vco3340AModule extends VcoModule {

	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private EnumParameter<WaveShape> waveShapeParameter;
	private MIDIParameter dutyParameter;
	private BooleanParameter syncFrom13700Parameter;
	private MIDIParameter levelParameter;
	
	// list of label constant for use by clients:
	public static final String WAVE = "Shape";
	public static final String DUTY = "Duty";
	public static final String SYNC = "Sync";
	public static final String LEVEL = "Level";
	
	public Vco3340AModule() {
		super();
		parameterList.add(waveShapeParameter = new EnumParameter<WaveShape>(WaveShape.class, WAVE));
		parameterList.add(dutyParameter = new MIDIParameter(DUTY));
		parameterList.add(syncFrom13700Parameter = new BooleanParameter(SYNC));
		parameterList.add(levelParameter = new MIDIParameter(LEVEL));
		
		// debug:
		waveShapeParameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString())); 
		dutyParameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString())); 
		syncFrom13700Parameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString()));
		levelParameter.addModuleParameterChangeListener(e -> LOGGER.info(e.toString()));
	}
	
	protected String getVcoName() {
		return "Vco3340A";
	}

	// ---- value getters and setters --- (write operating may fire change events)
	
	public WaveShape getWaveShape() {
		return waveShapeParameter.getValue();
	}
	
	public void setWaveShape(WaveShape waveshape) {
		this.waveShapeParameter.setValue(waveshape);
	}
	

	public boolean isSyncFrom13700() {
		return syncFrom13700Parameter.getValue();
	}


	public void setSyncFrom13700(boolean syncFrom13700) {
		this.syncFrom13700Parameter.setValue(syncFrom13700);
	}
	
	public double getDuty() {
		return dutyParameter.getValue();
	}

	public void setDuty(int duty) {
		this.dutyParameter.setValue(duty);
	}
	
	public int getLevel() {
		return levelParameter.getValue();
	}
	
	public void setLevel(int v){
		levelParameter.setValue(v);
	}
		
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	
	
	public BooleanParameter getSyncFrom13700Parameter() {
		return syncFrom13700Parameter;
	}
	
	public EnumParameter<WaveShape> getWaveShapeParameter() {
		return waveShapeParameter;
	}

	public MIDIParameter getDutyParameter() {
		return dutyParameter;
	}

	public MIDIParameter getLevelParameter() {
		return levelParameter;
	}

	// ----------- enum -------------
	
	public static enum WaveShape {
		
		TRIANGLE,
		SAWTOOTH,
		PULSE;
	}


	
	// ------------ test -------------
	public static void main(String[] args) {
	
		Vco3340AModule vco1 = new Vco3340AModule();
		List<ModuleParameter<?>> paramsVCO1 = vco1.getParameters();
		for (ModuleParameter<?> p : paramsVCO1) {
			System.out.println(p);
		}
	}

}


