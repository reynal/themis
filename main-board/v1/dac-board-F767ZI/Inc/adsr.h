/*
 * adsr.h
 *
 * defines and structs for ADSR enveloppes and related parameters (PWM, cutoff, resonance, etc)
 *
 *  Created on: Jan 30, 2019
 *      Author: reynal
 *
 *   NOTE : we use Java/C++ coding conventions for function, struct and variable names!
 */

#ifndef ADSR_H_
#define ADSR_H_

#include "stm32f7xx_hal.h"
#include "dac_board.h"

#define	ADSR_TIMER_PERIOD_FACTOR 20
#define	ADSR_TIMER_PERIOD (ADSR_TIMER_PERIOD_FACTOR * TIMDAC_PERIOD_SEC) // 1ms = 0.001
#define	ADSR_TIMER_PERIOD_MS (1000.0 * ADSR_TIMER_PERIOD) // around 1ms

/**
 *  following is a list of default MIDI CC values (from 0 to 127)
 *  and the associated maximum values:
 *  - from 0 to 4095 for DAC voltages (e.g., mixer control, PWM duty, etc)
 *  - from 0 to the maximum physical value for others (e.g., ADSR envelopes)
 *  The actual default DAC value is thus given by, e.g., for the VCA envelope attack time,
 *
 *       DEF_MIDICC_ATTACK_TIME_VCA * MAX_ATTACK_TIME_VCA / 127.0
 *
 *  and for, e.g. the PWM duty cycle (DAC #2):
 *
 *  	DEF_MIDICC_VCO3340_PWM_DUTY * MAX_VCO3340_PWM_DUTY
 */
// VCA ADSR (times in ms)
#define DEF_MIDICC_ATTACK_TIME_VCA 2 // A
//#define MAX_ATTACK_TIME_VCA 1000

#define DEF_MIDICC_DECAY_TIME_VCA 8 // D
//#define MAX_DECAY_TIME_VCA 1000

#define DEF_MIDICC_SUSTAIN_LVL_VCA 110 // S
//#define MAX_SUSTAIN_LVL_VCA 1

#define DEF_MIDICC_RELEASE_TIME_VCA 64 // R
//#define MAX_RELEASE_TIME_VCA 5000

// VCF ADSR (times in ms)
#define DEF_MIDICC_ATTACK_TIME_VCF 48 // A
//#define MAX_ATTACK_TIME_VCF 1000

#define DEF_MIDICC_DECAY_TIME_VCF 48 // D
//#define MAX_DECAY_TIME_VCF 1000

#define DEF_MIDICC_SUSTAIN_LVL_VCF 64 // S
//#define MAX_SUSTAIN_LVL_VCF 1

#define DEF_MIDICC_RELEASE_TIME_VCF 32 // R
//#define MAX_RELEASE_TIME_VCF 5000

// VCF EG etc
#define DEF_MIDICC_KBD_TRACKING_VCF 0 // kbd tracking (maximum permitted voltage shift in percents)
#define MAX_KBD_TRACKING_VCF 0 // TODO

#define DEF_MIDICC_EG_DEPTH_VCF 127 // env amount
#define MAX_EG_DEPTH_VCF 1.0 // TODO

// Velocity
#define DEF_MIDICC_VELOCITY_SENSITIVITY_VCA 13
#define DEF_MIDICC_VELOCITY_SENSITIVITY_VCF 0
#define MAX_VELOCITY_SENSITIVITY 1






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


/*
  parameters for the generation of the VCA enveloppes
*/
typedef struct {
  AdsrMachineState machineState; // current state of the state machine
  AdsrParams* adsrParam; // adsr env params
  double amplitude; // current CV value
  double velocitySensitivity; // 0-100% ; actual CV is modulated by velocity depending on this parameter (0= no mod, 1=full mod)
  double tmpDelta; // env value gets increased by this quantity at each time step (this is dx/dt * 1ms)
  double tmpTargetLevel; // target level inside each phase (ex: 1.0 for the A phase, sustain for the D phase, 0 for the R phase)
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
  double tmpVelocityMulFactor;
  double tmpDelta; // env value gets increased by this quantity at each time step (this is dx/dt * 1ms)
  double tmpTargetLevel; // target level at end of each phase, depends on env_amount, sustain and velocity_mul_factor
  double tmpKbdtrackingShiftFactor; // global shift (aka voltage addition) due to kbd_tracking
  //double tmpIncrease; // env value gets increased by this quantity at each time step
} StateMachineVcf;


/* Private function prototypes -----------------------------------------------*/

void initAdsrParameters();

void prepareVcaEnvelopeNoteON();
void prepareVcaEnvelopeNoteOFF();
void updateVcaEnvelope();

void prepareVcfEnvelopeNoteON();
void prepareVcfEnvelopeNoteOFF();
void updateVcfEnvelope();

void setVcfCutoffGlobal(uint8_t value);
void setVcfResonanceGlobal(uint8_t value);

void setVcfEgDepth(uint8_t value);
void setVcfKbdTracking(uint8_t value);


void setVcaAdsrAttack(uint8_t value);
void setVcaAdsrDecay(uint8_t value);
void setVcaAdsrSustain(uint8_t value);
void setVcaAdsrRelease(uint8_t value);
void setVcaVelocitySensitivity(uint8_t value);

void setVcfAdsrAttack(uint8_t value);
void setVcfAdsrDecay(uint8_t value);
void setVcfAdsrSustain(uint8_t value);
void setVcfAdsrRelease(uint8_t value);
void setVcfVelocitySensitivity(uint8_t value);

#endif /* ADSR_H_ */
