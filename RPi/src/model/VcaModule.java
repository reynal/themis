package model;

import java.util.logging.Logger;

/**
 * A model for a VCA (aka Voltage Controled Amplifier) module, whatever the underlying hardware (may be e.g. a LM13700 or a V2140 quad vca).
 * 
 * This model currently comprises:
 * - a velocity sensitivity parameter, ranging from 0% to 100%
 * - an ADSR enveloppe.  
 * 
 * @author Bastien Fratta
 */

public class VcaModule extends AbstractModule {
	
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	
	private MIDIParameter velocitySensitivityParameter;
	private ADSREnveloppe adsrEnveloppe;
	
	// list of label constant for use by clients:
	public static final String VEL_SENSITIVITY= "VCA Vel Sens";
	
	public VcaModule() {
		
		adsrEnveloppe = new ADSREnveloppe();
		parameterList.addAll(adsrEnveloppe.getParameters());
		
		velocitySensitivityParameter = new MIDIParameter(VEL_SENSITIVITY);
		parameterList.add(velocitySensitivityParameter);
		
		for (ModuleParameter<?> param : getParameters()) param.addModuleParameterChangeListener(e -> LOGGER.info(e.toString()));  // for debug purpose only
	}
	
	// ---- value getters and setters --- (write operating may fire change events)

	/**
	 * @return the sensitivity of the ADSR enveloppe to the MIDI note velocity, from 0% to 100%
	 */
	public double getVelocitySensitivity() {
		return velocitySensitivityParameter.getValue() / 127.0;
	}

	/**
	 * @param sensitivity sets the sensitivity of the ADSR enveloppe to the MIDI note velocity, from 0% to 100%
	 */
	public void setVelocitySensitivity(double sensitivity) {
		this.velocitySensitivityParameter.setValue((int)(127.0 * sensitivity));
	}
	
	// ---- SynthParameter getters ---- (write access is forbidden so as to listener mechanism integrity)
	

	public ADSREnveloppe getAdsrEnveloppe() {
		return adsrEnveloppe;
	}

	public MIDIParameter getVelocitySensitivityParameter() {
		return velocitySensitivityParameter;
	}
	

	
}