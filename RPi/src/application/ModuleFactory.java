package application;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.logging.Logger;

import model.*;
import model.serial.AbstractSerialTransmitter;
import view.touchscreen.VcaView;
import view.touchscreen.VcfView;
import view.touchscreen.Vco13700View;
import view.touchscreen.Vco3340View;

import static model.MidiCCImplementation.*;

/**
 * This class is responsible for creating all the hardware, i.e., all the modules that represent
 * a real hardware implementation of the Themis synthetizer.
 * This is implemented as a singleton (TODO: best choice? add a "DefaultFactory" instead?)
 * 
 * @author bastien
 * @author reynal
 *
 */
public class ModuleFactory {
	
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private HashMap<ModuleParameter<?>, MidiCCImplementation> midiCcIdTable;
	private ModuleParameter<?>[] midiCCToModuleParameter;
	
	private Vco3340Module vco3340;
	private Vco13700Module vco13700;
	private VcaModule vca;
	private VcfModule vcf;
	

	// --------------------- singleton ---------------------
	
	private static ModuleFactory modules =  null;
	
	public static ModuleFactory getDefault() {
		// lazily creates factory:
		if (modules == null)
			modules = new ModuleFactory();
		return modules;
	}
	
	// --------------------- constructor ---------------------
	
	private ModuleFactory(){
		
		vco3340 = new Vco3340Module();
		vco13700 = new Vco13700Module();
		vca = new VcaModule();
		vcf = new VcfModule();
		
		initMidiCCHashMap();
		initMidiCCToModuleParameterTable();
		

		
	}
	
	// --------------------- module getters ---------------------
	
	public Vco3340Module getVco3340() {
		return vco3340;
	}

	public Vco13700Module getVco13700() {
		return vco13700;
	}

	public VcaModule getVca() {
		return vca;
	}

	public VcfModule getVcf() {
		return vcf;
	}
	
	/**
	 * @return a list containing all the module parameters handled by this factory.
	 */
	public List<ModuleParameter<?>> getAllModuleParameters(){
		
		ArrayList<ModuleParameter<?>> list = new ArrayList<ModuleParameter<?>>();
		
		list.addAll(getVco3340().getParameters());
		list.addAll(getVco13700().getParameters());
		list.addAll(getVca().getParameters());
		list.addAll(getVcf().getParameters());
		
		return list;
	}
	
	
	
	// ------------------- other methods --------------
	
	/**
	 * @return the Midi CC code for the given parameter, if it's in the table.
	 * Otherwise return -1.
	 */
	public int getMidiCC(ModuleParameter<?> p) {
		MidiCCImplementation midiCC = midiCcIdTable.get(p);
		if (midiCC != null) return midiCC.getCode();
		else return -1;
	}
	
	public ModuleParameter<?> getModuleParameter(int midiCC){
		return midiCCToModuleParameter[midiCC];
	}
	
	/**
	 * Attach the given AbstractSerialTransmitter as a listener to changes in this module parameters.
	 * @param serialTransmitter
	 */
	public void attachSerialTransmitter(AbstractSerialTransmitter serialTransmitter) {
		
		for (ModuleParameter<?> parameter : getAllModuleParameters()) {
			
			parameter.addModuleParameterChangeListener(serialTransmitter);
		}
		
	}
	
	
	// --------------------- private methods ---------------------
	
	/**
	 * Init the MIDI parameter ID table 
	 * (used for example by a serial transmitter when writing to the STM32 over the serial bus)
	 */
	private void initMidiCCHashMap(){
		
		midiCcIdTable =  new HashMap<ModuleParameter<?>, MidiCCImplementation> ();
		
		// 3340
		//midiCcIdHashMap.put(getVco3340().getDetuneParameter(), ???); TODO
		midiCcIdTable.put(getVco3340().getOctaveParameter(), OCTAVE_3340);
		midiCcIdTable.put(getVco3340().getSyncFrom13700Parameter(), SYNC_3340);
		midiCcIdTable.put(getVco3340().getWaveShapeParameter(), WAVE_3340);
		midiCcIdTable.put(getVco3340().getDutyParameter(), PWM_3340);

		// 13700
		midiCcIdTable.put(getVco13700().getDetuneParameter(), DETUNE_13700);
		midiCcIdTable.put(getVco13700().getOctaveParameter(), OCTAVE_13700);
		midiCcIdTable.put(getVco13700().getWaveShapeParameter(), WAVE_13700);

		// 3320 vcf
		midiCcIdTable.put(getVcf().getCutoffParameter(), VCF_CUTOFF);
		midiCcIdTable.put(getVcf().getResonanceParameter(), VCF_RESONANCE);
		midiCcIdTable.put(getVcf().getFilterOrderParameter(), VCF_ORDER);
		midiCcIdTable.put(getVcf().getKbdTrackingParameter(), VCF_KBDTRACKING);
		midiCcIdTable.put(getVcf().getEgDepthParameter(), VCF_EG);
		midiCcIdTable.put(getVcf().getVelocitySensitivityParameter(), VCF_VELOCITY_SENSITIVITY);
		midiCcIdTable.put(getVcf().getAdsrEnveloppe().getAttackMsParameter(), VCF_ATTACK);
		midiCcIdTable.put(getVcf().getAdsrEnveloppe().getDecayMsParameter(), VCF_DECAY);
		midiCcIdTable.put(getVcf().getAdsrEnveloppe().getSustainLevelParameter(), VCF_SUSTAIN);
		midiCcIdTable.put(getVcf().getAdsrEnveloppe().getReleaseMsParameter(), VCF_RELEASE);

		// VCA
		midiCcIdTable.put(getVca().getVelocitySensitivityParameter(), VCA_VELOCITY_SENSITIVITY);
		midiCcIdTable.put(getVca().getAdsrEnveloppe().getAttackMsParameter(), VCA_ATTACK);
		midiCcIdTable.put(getVca().getAdsrEnveloppe().getDecayMsParameter(), VCA_DECAY);
		midiCcIdTable.put(getVca().getAdsrEnveloppe().getSustainLevelParameter(), VCA_SUSTAIN);
		midiCcIdTable.put(getVca().getAdsrEnveloppe().getReleaseMsParameter(), VCA_RELEASE);
		
		LOGGER.info("midiCcIdTable initialized");
	}
	
	private void initMidiCCToModuleParameterTable(){
		
		midiCCToModuleParameter =  new ModuleParameter[128];
		
		// 3340
		//midiCcIdHashMap.put(getVco3340().getDetuneParameter(), ???); TODO
		midiCCToModuleParameter[OCTAVE_3340.getCode()]	= getVco3340().getOctaveParameter();
		midiCCToModuleParameter[SYNC_3340.getCode()]	= getVco3340().getSyncFrom13700Parameter();
		midiCCToModuleParameter[WAVE_3340.getCode()]	= getVco3340().getWaveShapeParameter();
		midiCCToModuleParameter[PWM_3340.getCode()]		= getVco3340().getDutyParameter();

		// 13700
		midiCCToModuleParameter[DETUNE_13700.getCode()]	= getVco13700().getDetuneParameter();
		midiCCToModuleParameter[OCTAVE_13700.getCode()]	= getVco13700().getOctaveParameter();
		midiCCToModuleParameter[WAVE_13700.getCode()]	= getVco13700().getWaveShapeParameter();

		// 3320 vcf
		midiCCToModuleParameter[VCF_CUTOFF.getCode()]	= getVcf().getCutoffParameter();
		midiCCToModuleParameter[VCF_RESONANCE.getCode()]= getVcf().getResonanceParameter();
		midiCCToModuleParameter[VCF_ORDER.getCode()]	= getVcf().getFilterOrderParameter();
		midiCCToModuleParameter[VCF_KBDTRACKING.getCode()]=getVcf().getKbdTrackingParameter();
		midiCCToModuleParameter[VCF_EG.getCode()]		= getVcf().getEgDepthParameter();
		midiCCToModuleParameter[VCF_VELOCITY_SENSITIVITY.getCode()]=getVcf().getVelocitySensitivityParameter();
		midiCCToModuleParameter[VCF_ATTACK.getCode()]	= getVcf().getAdsrEnveloppe().getAttackMsParameter();
		midiCCToModuleParameter[VCF_DECAY.getCode()]	= getVcf().getAdsrEnveloppe().getDecayMsParameter();
		midiCCToModuleParameter[VCF_SUSTAIN.getCode()]	= getVcf().getAdsrEnveloppe().getSustainLevelParameter();
		midiCCToModuleParameter[VCF_RELEASE.getCode()]	= getVcf().getAdsrEnveloppe().getReleaseMsParameter();

		// VCA
		midiCCToModuleParameter[VCA_VELOCITY_SENSITIVITY.getCode()]=getVca().getVelocitySensitivityParameter();
		midiCCToModuleParameter[VCA_ATTACK.getCode()]=getVca().getAdsrEnveloppe().getAttackMsParameter();
		midiCCToModuleParameter[VCA_DECAY.getCode()]=getVca().getAdsrEnveloppe().getDecayMsParameter();
		midiCCToModuleParameter[VCA_SUSTAIN.getCode()]=getVca().getAdsrEnveloppe().getSustainLevelParameter();
		midiCCToModuleParameter[VCA_RELEASE.getCode()]=getVca().getAdsrEnveloppe().getReleaseMsParameter();
		
		LOGGER.info("midiCCToModuleParameter initialized");
	}
			
}
