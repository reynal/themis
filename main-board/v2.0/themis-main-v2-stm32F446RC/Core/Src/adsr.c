/*
 * adsr.c
 *
 *  Created on: Apr 26, 2020
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
#include "stm32f4xx_hal.h"
#include "midi.h"
#include "vcf.h"
#include "vca.h"
#include "stdio.h"

/* External variables --------------------------------------------------------*/

extern MidiNote_t midiNote;
extern GlobalFilterParams vcfGlobalParams;


/* Variables ---------------------------------------------------------*/

//#define LEGATO TODO


// the following table maps a Midi CC value (0-127) to a time constant in ms in an exponential way
static const int midiValueToTimeMs[128] = { 1, 8, 16, 25, 34, 44, 53, 63, 74, 85, 96, 108, 121, 134, 147, 161, \
		175, 191, 206, 223, 240, 257, 276, 295, 315, 335, 357, 379, 402, 426, \
		452, 478, 505, 533, 563, 593, 625, 658, 693, 729, 766, 805, 845, 887, \
		931, 976, 1023, 1072, 1124, 1177, 1232, 1289, 1349, 1412, 1476, 1544, \
		1614, 1686, 1762, 1841, 1923, 2008, 2097, 2189, 2285, 2385, 2489, \
		2596, 2709, 2826, 2947, 3073, 3205, 3342, 3484, 3632, 3786, 3946, \
		4112, 4285, 4465, 4653, 4848, 5050, 5261, 5480, 5708, 5946, 6193, \
		6449, 6716, 6994, 7283, 7583, 7896, 8221, 8559, 8911, 9277, 9657, \
		10053, 10465, 10893, 11338, 11802, 12284, 12785, 13306, 13849, 14413, \
		15000, 15610, 16245, 16905, 17592, 18306, 19050, 19823, 20627, 21463, \
		22333, 23238, 24179, 25158, 26176, 27235, 28337, 29483 };

static AdsrParams_t adsrVca;

VcaStateMachine_t vcaStateMachine = {
		//.t = 0,
		.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * DEF_MIDICC_VELOCITY_SENSITIVITY_VCA / 127.0,
		//.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_ATTACK_TIME),
		//.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_DECAY_TIME),
		//.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/DEF_RELEASE_TIME),
		.machineState = IDLE,
		.adsrParam = &adsrVca
};

static AdsrParams_t adsrVcf;

VcfStateMachine_t vcfStateMachine = {
		.t = 0,
		.tMax = 0,
		.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * DEF_MIDICC_VELOCITY_SENSITIVITY_VCF / 127.0,
		.kbdTracking = MAX_KBD_TRACKING_VCF * DEF_MIDICC_KBD_TRACKING_VCF / 127.0,
		.envAmount = MAX_EG_DEPTH_VCF * DEF_MIDICC_EG_DEPTH_VCF / 127.0,
		//.mulFactorAttack=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.attackTimeMs),
		//.mulFactorDecay=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.decayTimeMs),
		//.mulFactorRelease=exp(-1000.0*ADSR_TIMER_PERIOD/vcfAdsr.releaseTimeMs),
		.machineState = IDLE,
		.adsrParam = &adsrVcf
};

/* User code -----------------------------------------------*/

// ========================== VCA ==============================

/**
 * Init all ADSR enveloppes parameters.
 */
void adsrInitParameters(){

	adsrVca.attackTimeMs = midiValueToTimeMs[DEF_MIDICC_ATTACK_TIME_VCA];
	adsrVca.decayTimeMs = midiValueToTimeMs[DEF_MIDICC_DECAY_TIME_VCA];
	adsrVca.releaseTimeMs = midiValueToTimeMs[DEF_MIDICC_RELEASE_TIME_VCA];
	adsrVca.sustainLevel = DEF_MIDICC_SUSTAIN_LVL_VCA / 127.0;

	adsrVcf.attackTimeMs = midiValueToTimeMs[DEF_MIDICC_ATTACK_TIME_VCF];
	adsrVcf.decayTimeMs = midiValueToTimeMs[DEF_MIDICC_DECAY_TIME_VCF];
	adsrVcf.releaseTimeMs = midiValueToTimeMs[DEF_MIDICC_RELEASE_TIME_VCF];
	adsrVcf.sustainLevel = DEF_MIDICC_SUSTAIN_LVL_VCF / 127.0;

}

/**
 * Init data for the Attack phase of the VCA envelope. This is triggered by a MIDI Note On.
 */
void adsrPrepareVcaEnvelopeNoteON() {

	// stateMachineVca.amplitude=0.0; // commented Dec 3 2019 SR : avoid clip when re-triggering note while the enveloppe is not finished
	vcaStateMachine.tmpTargetLevel = ((1.0 - vcaStateMachine.velocitySensitivity)  + (midiNote.velocity / 127.) * vcaStateMachine.velocitySensitivity);

	vcaStateMachine.tmpDelta = ADSR_TIMER_PERIOD_MS * vcaStateMachine.tmpTargetLevel / adsrVca.attackTimeMs; // prepare dx for the attack phase of x(t)

	vcaStateMachine.machineState = ATTACK; // force vca machine state to ATTACK

	vcaStateMachine.amplitude = 0.0;
}

/**
 * Init data for the Release phase of the VCA envelope. This is triggered by a MIDI Note Off.
 */
void adsrPrepareVcaEnvelopeNoteOFF() {

	vcaStateMachine.tmpTargetLevel = 0.0;

	vcaStateMachine.tmpDelta = - ADSR_TIMER_PERIOD_MS * vcaStateMachine.amplitude / adsrVca.releaseTimeMs; // prepare dx for the R phase of x(t)

	vcaStateMachine.machineState = RELEASE; // force vca machine state to RELEASE

}

/**
 * Updates the state machines associated with the generation of the VCA ADSR envelope,
 * then write "stateMachineVca.amplitude" to the appropriate DAC
 * This method should be called from the timer handler (every ms or so)
 */
void adsrUpdateVcaEnvelope() {

	switch (vcaStateMachine.machineState) {

	case IDLE:
		break;

	case ATTACK:
		vcaStateMachine.amplitude += vcaStateMachine.tmpDelta;
		if (vcaStateMachine.amplitude >= vcaStateMachine.tmpTargetLevel) {
			// prepare dyn params for DECAY phase:
			vcaStateMachine.tmpTargetLevel *= adsrVca.sustainLevel; // modulate sustain level with velocity factor
			vcaStateMachine.tmpDelta = ADSR_TIMER_PERIOD_MS
					* (vcaStateMachine.tmpTargetLevel
							- vcaStateMachine.amplitude)
					/ (adsrVca.decayTimeMs); // prepare dx for the decay phase of x(t)
			vcaStateMachine.machineState = DECAY;
		}
		break;

	case DECAY:

		if (vcaStateMachine.amplitude > vcaStateMachine.tmpTargetLevel) {
			vcaStateMachine.amplitude += vcaStateMachine.tmpDelta;
		}
		// else stays on sustain plateau until NOTE OFF occurs
		break;

	case RELEASE:

		if (vcaStateMachine.amplitude > 0.0) { // stateMachineVca.tmpTargetLevel) {
			vcaStateMachine.amplitude += vcaStateMachine.tmpDelta; // else stays on sustain plateau until NOTE OFF occurs
		} else
			vcaStateMachine.machineState = IDLE;
		break;
	}
	vcaWriteAmplitudeToDac();

}

// =================== VCF ===================

/**
 * Init data for the Attack phase of the VCF envelope. This is triggered by a MIDI Note On.
 */
void adsrPrepareVcfEnvelopeNoteON() {

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

	vcfStateMachine.tmpVelocityMulFactor = (1.0 - vcfStateMachine.velocitySensitivity) + (midiNote.velocity / 127.) * vcfStateMachine.velocitySensitivity;
	vcfStateMachine.t = 0;
	vcfStateMachine.tMax = (int) (adsrVcf.attackTimeMs * ADSR_TIMER_FREQUENCY_KHZ);
	vcfStateMachine.tmpKbdtrackingShiftFactor = (midiNote.note - 64) / 64.0* vcfStateMachine.kbdTracking * MAX_KBD_TRACKING_VCF;
#ifndef LEGATO
	vcfStateMachine.cutoffFrequency = vcfGlobalParams.vcfCutoff; // starts at global cutoff value (comment out in legato mode)
#endif

	vcfStateMachine.machineState = ATTACK; // force vcf machine state to ATTACK
}

/**
 * Init data for the Release phase of the VCA envelope. This is triggered by a MIDI Note Off.
 */
void adsrPrepareVcfEnvelopeNoteOFF() {

	vcfStateMachine.t = 0;
	vcfStateMachine.tMax = adsrVcf.releaseTimeMs * ADSR_TIMER_FREQUENCY_KHZ;
	//stateMachineVcf.tmpTargetLevel = globalFilterParams.vcfCutoff;
	//stateMachineVcf.tmpDelta = ADSR_TIMER_PERIOD_MS * (stateMachineVcf.tmpTargetLevel - stateMachineVcf.cutoffFrequency) / stateMachineVcf.tMax;

	vcfStateMachine.machineState = RELEASE; // force vcf machine state to RELEASE
}

/**
 * Updates the state machines associated with the generation of the VCF ADSR enveloppes,
 * then write it to the appropriate DAC
 */
void adsrUpdateVcfEnvelope() {

	VcfStateMachine_t* s = &vcfStateMachine; // alias for easier reading

	switch (vcfStateMachine.machineState) {

	case IDLE:
		s->cutoffFrequency = vcfGlobalParams.vcfCutoff;
		break;

	case ATTACK:
		s->tmpTargetLevel = s->envAmount * s->tmpVelocityMulFactor;
		s->tmpDelta = (s->tmpTargetLevel - s->cutoffFrequency) / (s->tMax - s->t);
		s->cutoffFrequency += s->tmpDelta;
		s->t++;
		if (s->t >= s->tMax) {
			// prepare dyn params for DECAY phase:
			s->t = 0;
			s->tMax = adsrVcf.decayTimeMs * ADSR_TIMER_FREQUENCY_KHZ;
			s->machineState = DECAY;
		}
		break;

	case DECAY:
		if (s->t < s->tMax) { // while DECAY phase's still ongoing...
			s->tmpTargetLevel = vcfGlobalParams.vcfCutoff
								+ (s->envAmount * s->tmpVelocityMulFactor - vcfGlobalParams.vcfCutoff) * adsrVcf.sustainLevel; // modulate sustain level with velocity factor
			s->tmpDelta = (s->tmpTargetLevel - s->cutoffFrequency) / (s->tMax  - s->t);
			s->cutoffFrequency += s->tmpDelta;
			s->t++;
		}
		else // otherwise we stay on sustain plateau until a NOTE OFF event occurs
			s->cutoffFrequency = vcfGlobalParams.vcfCutoff
								+ (s->envAmount * s->tmpVelocityMulFactor - vcfGlobalParams.vcfCutoff) * adsrVcf.sustainLevel;

		break;

	case RELEASE:
		if (s->t < s->tMax) {
			s->tmpTargetLevel = vcfGlobalParams.vcfCutoff; // allow for realtime editing of vcf cutoff!
			s->tmpDelta = ADSR_TIMER_PERIOD_MS * (s->tmpTargetLevel - s->cutoffFrequency) / (s->tMax - s->t);
			s->cutoffFrequency += s->tmpDelta; // else go back to IDLE state
			s->t++;
		}
		else {
			s->machineState = IDLE;
			s->cutoffFrequency = vcfGlobalParams.vcfCutoff;
		}
		break;
	}
	vcfWriteCutoffToDac();

}

// ===================== setters =============================

void adsrSetVcaAttack(uint8_t value) {
	adsrVca.attackTimeMs = midiValueToTimeMs[value]; // ((value + 1) / 127.) * MAX_ATTACK_TIME_VCA;
}

void adsrSetVcaDecay(uint8_t value) {
	adsrVca.decayTimeMs =  midiValueToTimeMs[value]; // ((value + 1) / 127.) * MAX_DECAY_TIME_VCA;
}

void adsrSetVcaSustain(uint8_t value) {
	adsrVca.sustainLevel = (value / 127.); // * MAX_SUSTAIN_LVL_VCA;
}

void adsrSetVcaRelease(uint8_t value) {
	adsrVca.releaseTimeMs = midiValueToTimeMs[value]; //((value + 1) / 127.) * MAX_RELEASE_TIME_VCA;
}

void adsrSetVcaVelocitySensitivity(uint8_t value) {
	vcaStateMachine.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * value / 127.;
}

void adsrSetVcfAttack(uint8_t value) {
	adsrVcf.attackTimeMs = midiValueToTimeMs[value]; //((value + 1) / 127.) * MAX_ATTACK_TIME_VCF;
}

void adsrSetVcfDecay(uint8_t value) {
	adsrVcf.decayTimeMs = midiValueToTimeMs[value]; //((value + 1) / 127.) * MAX_DECAY_TIME_VCF;
}

void adsrSetVcfSustain(uint8_t value) {
	adsrVcf.sustainLevel = (value / 127.); // * MAX_SUSTAIN_LVL_VCF;
}

void adsrSetVcfRelease(uint8_t value) {
	adsrVcf.releaseTimeMs = midiValueToTimeMs[value]; //((value + 1) / 127.) * MAX_RELEASE_TIME_VCF;
}

void adsrSetVcfVelocitySensitivity(uint8_t value) {
	vcfStateMachine.velocitySensitivity = MAX_VELOCITY_SENSITIVITY * value / 127.;
}

void adsrSetVcfEgDepth(uint8_t value) {
	vcfStateMachine.envAmount = MAX_EG_DEPTH_VCF * value / 127.;
}

void adsrSetVcfKbdTracking(uint8_t value) {
	vcfStateMachine.kbdTracking = MAX_KBD_TRACKING_VCF * value / 127.;
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


