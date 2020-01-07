/*
 * adsr.c
 *
 *  Created on: Dec 14, 2019
 *      Author: sydxrey
 *
 *
 * Generation of ADSR envelopes for VCA and VCF.
 *
 * ----------------- envelopes features ----------------
 *
 * - on most synths env's go from 1ms to 10" (see Moog Little phatty for instance).
 * - cutoff usually goes from 20Hz to 16kHz
 *
 * ----------------- vca env ----------------
 * VCA ADSR enveloppe : the tricky thing here is how velocity influences the
 * envelope amplitude. The following formula gives the global multiplicative factor:
 *
 *        tmp_velocity_mul_factor = (1-velocity_sensitivity) + (velocity/127.)*velocity_sensitivity;
 *
 * (we make use of a tmp var to speed up calculation, which gets updated once every NOTE ON event)
 *
 *
 * ----------------- vcf env ----------------
 * VCF ADSR enveloppe: here again things become quiet tricky when it comes to computing levels!!! (timing are easy to understand in comparison)
 * - env starts from initial value "vcf_cutoff" (see globa_synth_params_t)
 * - env raises during attack_time to a maximum value defined as
 *   max_level = env_amount * tmp_velocity_mul_factor
 * - env decays to sustain lvl = max_level * adsr.sustain_lvl
 *
 * In addition, the whole enveloppe is scaled up or down ( that is, including initial level)
 * depending on the current note being played AND according to kbd_tracking.
 * We thus define:
 *
 * tmp_kbdtracking_mul_factory = (note - 64)/64.0 *  kbd_tracking;
 *
 *   Note : tmp_velocity_mul_factor is defined like for the VCA env.
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "adsr.h"
#include "stm32f7xx_hal.h"
#include "mcp4822.h"
#include "midi.h"
#include "vcf.h"
#include "vca.h"
#include "stdio.h"

/* External variables --------------------------------------------------------*/

extern MidiNote midiNote;
extern GlobalFilterParams globalFilterParams;

/* Private variables ---------------------------------------------------------*/

#define LEGATO

AdsrParams vcaAdsr =
		{ .attackTimeMs = MAX_ATTACK_TIME_VCA * DEF_MIDICC_ATTACK_TIME_VCA
				/ 127.0, .decayTimeMs = MAX_DECAY_TIME_VCA
				* DEF_MIDICC_DECAY_TIME_VCA / 127.0, .releaseTimeMs =
				MAX_RELEASE_TIME_VCA * DEF_MIDICC_RELEASE_TIME_VCA / 127.0,
				.sustainLevel = MAX_SUSTAIN_LVL_VCA * DEF_MIDICC_SUSTAIN_LVL_VCA
						/ 127.0, };

StateMachineVca stateMachineVca = {
//.t = 0,
		.velocitySensitivity = MAX_VELOCITY_SENSITIVITY
				* DEF_MIDICC_VELOCITY_SENSITIVITY_VCA / 127.0,
		//.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_ATTACK_TIME),
		//.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_DECAY_TIME),
		//.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_RELEASE_TIME),
		.machineState = IDLE, .adsrParam = &vcaAdsr };

AdsrParams vcfAdsr =
		{ .attackTimeMs = MAX_ATTACK_TIME_VCF * DEF_MIDICC_ATTACK_TIME_VCF
				/ 127.0, .decayTimeMs = MAX_DECAY_TIME_VCF
				* DEF_MIDICC_DECAY_TIME_VCF / 127.0, .releaseTimeMs =
				MAX_RELEASE_TIME_VCF * DEF_MIDICC_RELEASE_TIME_VCF / 127.0,
				.sustainLevel = MAX_SUSTAIN_LVL_VCF * DEF_MIDICC_SUSTAIN_LVL_VCF
						/ 127.0, };

StateMachineVcf stateMachineVcf = { .t = 0, .tMax = 0, .velocitySensitivity =
		MAX_VELOCITY_SENSITIVITY * DEF_MIDICC_VELOCITY_SENSITIVITY_VCF / 127.0,
		.kbdTracking = MAX_KBD_TRACKING_VCF * DEF_MIDICC_KBD_TRACKING_VCF
				/ 127.0, .envAmount = MAX_ENV_AMOUNT_VCF
				* DEF_MIDICC_ENV_AMOUNT_VCF / 127.0,
		//.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.attackTimeMs),
		//.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.decayTimeMs),
		//.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.releaseTimeMs),
		.machineState = IDLE, .adsrParam = &vcfAdsr };

/* Private function prototypes -----------------------------------------------*/

/**
 * Init data for the Attack phase of the VCA envelope. This is triggered by a MIDI Note On.
 */
void prepareVcaEnvelopeNoteON() {

	// stateMachineVca.amplitude=0.0; // commented Dec 3 2019 SR : avoid clip when re-triggering note while the enveloppe is not finished
	stateMachineVca.tmpTargetLevel =
			((1.0 - stateMachineVca.velocitySensitivity)
					+ (midiNote.velocity / 127.)
							* stateMachineVca.velocitySensitivity);
	stateMachineVca.tmpDelta = ADSR_TIMER_PERIOD_MS
			* stateMachineVca.tmpTargetLevel / vcaAdsr.attackTimeMs; // prepare dx for the attack phase of x(t)

	stateMachineVca.machineState = ATTACK; // force vca machine state to ATTACK
}

/**
 * Init data for the Release phase of the VCA envelope. This is triggered by a MIDI Note Off.
 */
void prepareVcaEnvelopeNoteOFF() {

	stateMachineVca.tmpTargetLevel = 0.0;
	stateMachineVca.tmpDelta = - ADSR_TIMER_PERIOD_MS
			* stateMachineVca.amplitude / vcaAdsr.releaseTimeMs; // prepare dx for the R phase of x(t)

	stateMachineVca.machineState = RELEASE; // force vca machine state to RELEASE

}

/**
 * Updates the state machines associated with the generation of the VCA ADSR envelope,
 * then write "stateMachineVca.amplitude" to the appropriate DAC
 * This method should be called from the timer handler (every ms or so)
 */
void updateVcaEnvelope() {

	switch (stateMachineVca.machineState) {

	case IDLE:
		break;

	case ATTACK:
		dacVcaWrite(stateMachineVca.amplitude);
		stateMachineVca.amplitude += stateMachineVca.tmpDelta;
		if (stateMachineVca.amplitude >= stateMachineVca.tmpTargetLevel) {
			// prepare dyn params for DECAY phase:
			stateMachineVca.tmpTargetLevel *= vcaAdsr.sustainLevel; // modulate sustain level with velocity factor
			stateMachineVca.tmpDelta = ADSR_TIMER_PERIOD_MS
					* (stateMachineVca.tmpTargetLevel
							- stateMachineVca.amplitude)
					/ (vcaAdsr.decayTimeMs); // prepare dx for the decay phase of x(t)
			stateMachineVca.machineState = DECAY;
		}
		break;

	case DECAY:

		if (stateMachineVca.amplitude > stateMachineVca.tmpTargetLevel) {
			stateMachineVca.amplitude += stateMachineVca.tmpDelta;
			dacVcaWrite(stateMachineVca.amplitude);
		}
		// else stays on sustain plateau until NOTE OFF occurs
		break;

	case RELEASE:

		if (stateMachineVca.amplitude > 0.0) { // stateMachineVca.tmpTargetLevel) {
			stateMachineVca.amplitude += stateMachineVca.tmpDelta; // else stays on sustain plateau until NOTE OFF occurs
			dacVcaWrite(stateMachineVca.amplitude);
		} else
			stateMachineVca.machineState = IDLE;
		break;
	}

}

/**
 * Init data for the Attack phase of the VCF envelope. This is triggered by a MIDI Note On.
 */
void prepareVcfEnvelopeNoteON() {

	// ---------- VCF dyn parameters -----------
	/*
	 * Example using default parameters:
	 * stateMachineVcf.velocitySensitivity = 0
	 * stateMachineVcf.kbdTracking = 0
	 * stateMachineVcf.envAmount =  1.0
	 *
	 * vcfAdsr.attackTimeMs = 500ms
	 * vcfAdsr.decayTimeMs = 125ms
	 * vcfAdsr.releaseTimeMs = 300ms
	 * vcfAdsr.sustainLevel = 50%
	 *
	 * globalParams.vcfCutoff = 100/127
	 *
	 * => velocityMulFactor=1.0
	 */

	stateMachineVcf.tmpVelocityMulFactor = (1.0 - stateMachineVcf.velocitySensitivity) + (midiNote.velocity / 127.) * stateMachineVcf.velocitySensitivity;
	stateMachineVcf.t = 0;
	stateMachineVcf.tMax = (int) (vcfAdsr.attackTimeMs / (ADSR_TIMER_PERIOD_MS));
	stateMachineVcf.tmpKbdtrackingShiftFactor = (midiNote.note - 64) / 64.0* stateMachineVcf.kbdTracking * MAX_KBD_TRACKING_VCF;
#ifndef LEGATO
	stateMachineVcf.cutoffFrequency = globalFilterParams.vcfCutoff; // starts at global cutoff value (comment out in legato mode)
#endif

	stateMachineVcf.machineState = ATTACK; // force vcf machine state to ATTACK
}

/**
 * Init data for the Release phase of the VCA envelope. This is triggered by a MIDI Note Off.
 */
void prepareVcfEnvelopeNoteOFF() {

	stateMachineVcf.t = 0;
	stateMachineVcf.tMax = vcfAdsr.releaseTimeMs / (ADSR_TIMER_PERIOD_MS);
	//stateMachineVcf.tmpTargetLevel = globalFilterParams.vcfCutoff;
	//stateMachineVcf.tmpDelta = ADSR_TIMER_PERIOD_MS * (stateMachineVcf.tmpTargetLevel - stateMachineVcf.cutoffFrequency) / stateMachineVcf.tMax;

	stateMachineVcf.machineState = RELEASE; // force vcf machine state to RELEASE
}

/**
 * Updates the state machines associated with the generation of the VCF ADSR enveloppes,
 * then write it to the appropriate DAC
 */
void updateVcfEnvelope() {

	StateMachineVcf* s = &stateMachineVcf; // alias for easier reading

	switch (stateMachineVcf.machineState) {

	case IDLE:
		s->cutoffFrequency = globalFilterParams.vcfCutoff;
		break;

	case ATTACK:
		s->tmpTargetLevel = s->envAmount * s->tmpVelocityMulFactor;
		s->tmpDelta = (s->tmpTargetLevel - s->cutoffFrequency) / (s->tMax - s->t);
		s->cutoffFrequency += s->tmpDelta;
		s->t++;
		if (s->t >= s->tMax) {
			// prepare dyn params for DECAY phase:
			s->t = 0;
			s->tMax = vcfAdsr.decayTimeMs / (ADSR_TIMER_PERIOD_MS);
			s->machineState = DECAY;
		}
		break;

	case DECAY:
		if (s->t < s->tMax) {
			s->tmpTargetLevel = globalFilterParams.vcfCutoff + (s->envAmount * s->tmpVelocityMulFactor - globalFilterParams.vcfCutoff) * vcfAdsr.sustainLevel; // modulate sustain level with velocity factor
			s->tmpDelta = (s->tmpTargetLevel - s->cutoffFrequency) / (s->tMax  - s->t);
			s->cutoffFrequency += s->tmpDelta;
			s->t++;
		}
		else
			s->cutoffFrequency = globalFilterParams.vcfCutoff + (s->envAmount * s->tmpVelocityMulFactor - globalFilterParams.vcfCutoff) * vcfAdsr.sustainLevel;
		// else stays on sustain plateau until NOTE OFF occurs
		break;

	case RELEASE:
		if (s->t < s->tMax) {
			s->tmpTargetLevel = globalFilterParams.vcfCutoff; // allow for realtime editing of vcf cutoff!
			s->tmpDelta = ADSR_TIMER_PERIOD_MS * (s->tmpTargetLevel - s->cutoffFrequency) / (s->tMax - s->t);
			s->cutoffFrequency += s->tmpDelta; // else go back to IDLE state
			s->t++;
		}
		else {
			s->machineState = IDLE;
			s->cutoffFrequency = globalFilterParams.vcfCutoff;
		}
		break;
	}
	updateVcfCutoff();

}

void setVcaAdsrAttack(uint8_t value) {
	vcaAdsr.attackTimeMs = ((value + 1) / 127.) * MAX_ATTACK_TIME_VCA;
}

void setVcaAdsrDecay(uint8_t value) {
	vcaAdsr.decayTimeMs = ((value + 1) / 127.) * MAX_DECAY_TIME_VCA;
}

void setVcaAdsrSustain(uint8_t value) {
	vcaAdsr.sustainLevel = (value / 127.) * MAX_SUSTAIN_LVL_VCA;
}

void setVcaAdsrRelease(uint8_t value) {
	vcaAdsr.releaseTimeMs = ((value + 1) / 127.) * MAX_RELEASE_TIME_VCA;
}

void setVcaVelocitySensitivity(uint8_t value) {
	stateMachineVca.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * value / 127.;
}

void setVcfAdsrAttack(uint8_t value) {
	vcfAdsr.attackTimeMs = ((value + 1) / 127.) * MAX_ATTACK_TIME_VCF;
}

void setVcfAdsrDecay(uint8_t value) {
	vcfAdsr.decayTimeMs = ((value + 1) / 127.) * MAX_DECAY_TIME_VCF;
}

void setVcfAdsrSustain(uint8_t value) {
	vcfAdsr.sustainLevel = (value / 127.) * MAX_SUSTAIN_LVL_VCF;
}

void setVcfAdsrRelease(uint8_t value) {
	vcfAdsr.releaseTimeMs = ((value + 1) / 127.) * MAX_RELEASE_TIME_VCF;
}

void setVcfVelocitySensitivity(uint8_t value) {
	stateMachineVcf.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * value / 127.;
}

/* exponential enveloppes
 void updateVCAEnveloppeStateMachine(){

 double sustain;
 switch (stateMachineVca.machineState){

 case IDLE:
 break;

 case ATTACK:
 stateMachineVca.tmpExp *= stateMachineVca.mulFactorAttack;
 stateMachineVca.amplitude = (1.0 - stateMachineVca.tmpExp) * stateMachineVca.tmpVelocityMulFactor;
 dacVcaWrite(stateMachineVca.amplitude);
 stateMachineVca.t++;
 if (stateMachineVca.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state
 stateMachineVca.t = 0;
 stateMachineVca.tmpExp = 1.0;
 stateMachineVca.machineState = DECAY;
 }
 break;

 case DECAY:

 sustain = stateMachineVca.adsrParam->sustainLevel;
 stateMachineVca.tmpExp *= stateMachineVca.mulFactorDecay;
 stateMachineVca.amplitude = (stateMachineVca.tmpExp * (1 - sustain) + sustain)  * stateMachineVca.tmpVelocityMulFactor;
 dacVcaWrite(stateMachineVca.amplitude);
 stateMachineVca.t++;
 break;

 case RELEASE :

 stateMachineVca.tmpExp *= stateMachineVca.mulFactorRelease;
 dacVcaWrite(stateMachineVca.amplitude * stateMachineVca.tmpExp);
 stateMachineVca.t++;
 if (stateMachineVca.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state = IDLE
 stateMachineVca.t = 0;
 stateMachineVca.machineState = IDLE;
 }
 break;
 }
 }
 */

/*
 * void updateVCFEnveloppeStateMachine(){

 double sustain;

 switch (stateMachineVcf.machineState){

 case IDLE:
 break;

 case ATTACK:
 stateMachineVcf.tmpExp *= stateMachineVcf.mulFactorAttack; // = exp(-t/tau)
 stateMachineVcf.cutoffFrequency = globalParams.vcfCutoff + (1.0 - stateMachineVcf.tmpExp) * (stateMachineVcf.tmpMaxLevel - globalParams.vcfCutoff);
 dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency);
 stateMachineVcf.t++;
 if (stateMachineVcf.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state
 stateMachineVcf.t = 0;
 stateMachineVcf.tmpExp = 1.0;
 stateMachineVcf.machineState = DECAY;
 }
 break;

 case DECAY:

 sustain = (stateMachineVcf.adsrParam->sustainLevel) * stateMachineVcf.tmpMaxLevel;
 stateMachineVcf.tmpExp *= stateMachineVcf.mulFactorDecay;
 stateMachineVcf.cutoffFrequency = globalParams.vcfCutoff + (stateMachineVcf.tmpExp * (stateMachineVcf.tmpMaxLevel - sustain - globalParams.vcfCutoff) + sustain);
 dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency);
 stateMachineVcf.t++;
 break;

 case RELEASE :

 stateMachineVcf.tmpExp *= stateMachineVcf.mulFactorRelease;
 dacVcfCutoffWrite(stateMachineVcf.cutoffFrequency * stateMachineVcf.tmpExp);
 stateMachineVcf.t++;
 if (stateMachineVcf.tmpExp < 0.01){ // we consider exp(-t/tau) over -> automatically go to next state
 stateMachineVcf.t = 0;
 stateMachineVcf.machineState = IDLE;
 }
 break;
 }
 }*/

