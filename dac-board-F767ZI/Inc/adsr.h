/*
 * adsr.h
 *
 * defines and structs for ADSR enveloppes
 *
 *  Created on: Jan 30, 2019
 *      Author: reynal
 *
 *   NOTE : we use Java/C++ coding conventions for function, struct and variable names!
 */

#ifndef ADSR_H_
#define ADSR_H_

// defines for ADSR enveloppes default values (times are in ms)
#define DEF_ATTACK_TIME 10
#define DEF_DECAY_TIME 400
#define DEF_RELEASE_TIME 200

#define DEF_ATTACK_TIME_VCF 500
#define DEF_DECAY_TIME_VCF 200
#define DEF_RELEASE_TIME_VCF 200

#define MAX_ATTACK_TIME 1000
#define MAX_DECAY_TIME 1000
#define MAX_RELEASE_TIME 5000
#define MAX_SUSTAIN_LVL 1

#define MAX_VC_SENSI 1
#define MAX_MIXER 1
// sets the maximum permitted voltage shift in % :
#define MAX_KBD_TRACKING 0.3


#define DEF_SUSTAIN_LVL 0.5
#define DEF_SUSTAIN_LVL_VCF 0.5
#define DEF_VELOCITY_SENSITIVITY_VCA 0.1
#define DEF_VELOCITY_SENSITIVITY_VCF 0.0
#define DEF_KBD_TRACKING 0.0
#define DEF_ENV_AMOUNT 0.9
#define DEF_CUTOFF 0.8
#define DEF_RESONANCE 0.0

#define DEF_VCO_3340_PWM_DUTY 0.5


/*
 * ADSR env params
 */
typedef struct {
    double attackTimeMs;   //
    double decayTimeMs;    //
    double sustainLevel;    // b/w 0.0 et 1.0
    double releaseTimeMs;  //
} AdsrParams;

/*
 * enum for the various state machine states
 */
typedef enum {
  IDLE,       // 0
  ATTACK,     // 1 ... dure 5 tau
  DECAY,      // 2 ... dure jusqu'au note off
  RELEASE     // 3
} AdsrMachineState;

/* global parameters cutoff and Q influence the shape of the CVF adsr enveloppe */
typedef struct {
  double vcfCutoff; // between 0 and 100%
  double vcfResonance; // between 0 and 100%
} GlobalSynthParams;

/*
  parameters for the generation of the VCA enveloppes
*/
typedef struct {
  AdsrMachineState machineState; // current state of the state machine
  AdsrParams* adsrParam; // adsr env params
  double amplitude; // current CV value
  double velocitySensitivity; // 0-100% ; actual CV is modulated by velocity depending on this parameter (0= no mod, 1=full mod)

  /* linear enveloppes for V2140D (aka in dB) */
  double tmpDelta; // env value gets increased by this quantity at each time step (this is dx/dt * 1ms)
  double tmpTargetLevel; // target level inside each phase (ex: 1.0 for the A phase, sustain for the D phase, 0 for the R phase)

  /* Exponential enveloppes for LM13700 based VCA with no exp conv : */
  //double tmpExp; // stores exp(-t/tau) on a temporary basis to speed up computation for exponential enveloppes
  //double mulFactorAttack; // exp(-T/tau_a), where T=timer period (TIMER_PERIOD)
  //double mulFactorDecay; // exp(-T/tau_d)
  //double mulFactorRelease; // exp(-T/tau_r)
} StateMachineVca;

/*
  parameters for the generation of the VCF enveloppes
*/
typedef struct {
  AdsrMachineState machineState; // current state of the state machine
  AdsrParams* adsrParam; // adsr env params
  int t; // time, reset at the beginning of each phase
  int tMax; // max time for the current phase, if relevant (i.e. irrelevant for sustain)
  double cutoffFrequency; // current CV value
  double velocitySensitivity; // 0-100% ; filter sensitivity to velocity
  double kbdTracking; // 0-100%, filter sensitivity to current note freq
  double envAmount; // 0-100%, filter sensitivity to enveloppe
  double tmpDelta; // env value gets increased by this quantity at each time step (this is dx/dt * 1ms)
  double tmpTargetLevel; // target level at end of each phase, depends on env_amount, sustain and velocity_mul_factor
  double tmpKbdtrackingShiftFactor; // global shift (aka voltage addition) due to kbd_tracking

  /* exponential env generation:
  double tmpExp; // stores exp(-t/tau) on a temporary basis
  double mulFactorAttack; // exp(-T/tau_a), where T=timer period (TIMER_PERIOD)
  double mulFactorDecay; // exp(-T/tau_d)
  double mulFactorRelease; // exp(-T/tau_r)
  */
  double tmpIncrease; // env value gets increased by this quantity at each time step
} StateMachineVcf;

/*
 * struct for VCO parameters
 */
typedef struct {
	double detune; //-50%-50% of one tone
	int octave; // can be positive or negative
} VcoParameters;

/* struct for drums */
typedef struct {
	int bassdrumCounter;
	int rimshotCounter;
	int snareCounter;
	int lowtomCounter;
	int hightomCounter;
} DrumTriggers;

#define BASS_DRUM_NOTE 36
#define RIMSHOT_NOTE 37
#define SNARE_NOTE 38
#define LOWTOM_NOTE 41
#define HIGHTOM_NOTE 48


#endif /* ADSR_H_ */
