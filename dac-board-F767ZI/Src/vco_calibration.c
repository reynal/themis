/*
 * vco_calibration.c
 *
 *  Created on: 20 nov. 2018
 *      Author: Morgan PRIOTON / Reynal
 *
 *
 * Features:
 * - the lowest note on the piano is a 27.5Hz A.
 * - C-1 	~8.18Hz		#0
 * - A-1		13.75Hz		#9
 * - A0 		27.5Hz		#21
 * - A1 		55Hz			#33
 * - A2 		110Hz		#45
 * - A3 		220Hz		#57
 * - C4 		~261.62Hz 	#60 		(middle C)
 * - A4 		440Hz 		#69		ref diapason
 * -	 A5		880Hz		#81
 * - A6		1760Hz		#93
 * - A7		3520Hz		#105
 * - A8		7040Hz		#117
 * - G9 		~12543Hz		#127
 *
 * VCO 3340 is able to produce a 17Hz a the lowest note though it can hardly heard (CV at 0mV).
 * The highest note is close to 1kHz (TODO: check exactly) (= 1562mV on the CV input, i.e, 3200 on the dac with gain=2)
 *
 * 	VCO13700 ... ??? TODO @Nathan
 *
 *
 */
/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
//#include "tim.h"
#include "vco_calibration.h"
#include "dac_board.h"

/* Private variables ---------------------------------------------------------*/
extern TIM_HandleTypeDef* htimCalib;
//char VCOnumberGeneral = 0;
int calibrationVcoCV; // VCO Control Voltage, i.e., DAC output !
int calibrationVcoCyclesCount = -1; // we compute the vco frequency over many signal cycles and we need to count'em
int calibrationCaptureValue = 0; // stores the Capture register value
//int voltageToPeriod[4096] = { 0 };
double voltageToFreq[4096] = { 0. }; // debugging only

double midiToFreqArray[128] = { 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.250, 12.978, 13.750, 14.568, 15.434, 16.352,  // #0
		17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.500, 25.957, 27.500, 29.135, 30.868, 32.703,  // #1
		34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735, 65.406,  // #2
		69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110.000, 116.541, 123.471, 130.813,  // #3
		138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220.000, 233.082, 246.942, 261.626,  // #4
		277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440.000, 466.164, 493.883, 523.251,  // #5
		554.365, 587.330, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880.000, 932.328, 987.767, 1046.502,  // #6
		1108.731, 1174.659, 1244.508, 1318.510, 1396.913, 1479.978, 1567.982, 1661.219, 1760.000, 1864.655, 1975.533, 2093.005, // #7
		2217.461, 2349.318, 2489.016, 2637.020, 2793.826, 2959.955, 3135.963, 3322.438, 3520.000, 3729.310, 3951.066, 4186.009,  // #8
		4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040.000, 7458.620, 7902.133, 8372.018,  // #9
		8869.844, 9397.273, 9956.063, 10548.082, 11175.303, 11839.822, 12543.854}; // #10

vcoCalib_t calibrationCurrentVco;

int maxVcoCV = 4096; // permitted CV range for each VCO
int minVcoCV = 0;

int tmpmidiToVCO13700CV[128] = { 0 }; // TODO preremplir avec une pre-calib

int tmpmidiToVCO3340CV[128] = { 0 };


/**
 * Launch the calibration process
 */
void launchVcoCalibration(){

		//initMidiToFreqArray();

	  calibrationCurrentVco = CALIB_VCO_3340;
	  maxVcoCV = VCO3340_MAX_INPUT_CV;
	  calibrationVcoCV = minVcoCV = VCO3340_MIN_INPUT_CV; // don't start too low... stability issues !
	  dacWrite(calibrationVcoCV, DAC_VCO_3340_FREQ);
	  HAL_TIM_IC_Start_IT(htimCalib, TIM_CHANNEL_CALIB_VCO3340); // Activation du calibrage du VCO branche sur PA0

	  while(calibrationCurrentVco != CALIB_COMPLETED){
		  HAL_Delay(100); // 100ms blinks faster!
		  //HAL_GPIO_TogglePin(GPIOB, LD3_Pin);
	  }

	  // proceeds with main() code

}

/**
 * Build a table of the periods T of a signal corresponding to all possible midi notes, according to
 * the formula given by the tempered range.
 *
 * T(n) = T127 * 2^(n/12)
 *
 * where n is the midi note, and T127 the period of the highest note
 *
 * 2^(1/12)=1.0594630943592953
 */
void initMidiToFreqArray(){

	int i=24; // first two octaves useless...
	midiToFreqArray[i] = BASE_MIDI_FREQ;

	while (i < 96){
		i++;
		midiToFreqArray[i] = midiToFreqArray[i-1] * 1.0594630943592953;
	}

}

/**
 * interrupt callback that gets triggered whenever ... TODO
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
  //if(htim->Instance == TIM2){ // plus rapide de faire le test htim==htimCalib
	if (htim == htimCalib){
		calibrateVcoCallbackShort();
		/*diff[diff_idx++]= __HAL_TIM_GetCompare(htimCalib, TIM_CHANNEL_CALIB_VCO3340); // or HAL_TIM_ReadCapturedValue(htimCalib, TIM_CHANNEL_CALIB_VCO3340)
		if (diff_idx >= 10) {
			//for (int i=0; i<9; i++) diff[i]=diff[i+1]-diff[i];
			diff_idx=0;
			__HAL_TIM_SetCounter(htimCalib, 0);
		}*/
	}
}

/**
 * Called by HAL_TIM_IC_CaptureCallback.
 */
void calibrateVcoCallbackShort() {

	switch (calibrationCurrentVco) {
	case CALIB_VCO_3340: // ch 1
		calibrationCaptureValue =__HAL_TIM_GetCompare(htimCalib, TIM_CHANNEL_CALIB_VCO3340);
		break;
	case CALIB_VCO_13700: // ch 2
		calibrationCaptureValue =__HAL_TIM_GetCompare(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
		break;
	case CALIB_COMPLETED:
		return;
	}

	// initial step of given freq:
	if (calibrationVcoCyclesCount == -1) {
		__HAL_TIM_SetCounter(htimCalib, 0); // SR TODO : peut etre c'est mieux de calculer la difference entre deux capture successives ?
		calibrationVcoCyclesCount=0;
	}

	// ultimate step: stores measure in table
	else {

		//voltageToPeriod[calibrationVcoCV] = calibrationCaptureValue;
		voltageToFreq[calibrationVcoCV] = TIM2_FREQ * TIM2_IC_PRESCALER / calibrationCaptureValue;

		calibrationVcoCyclesCount = -1; // reinit for next freq
		calibrationVcoCV ++; // validate current measure and go to next
		HAL_GPIO_TogglePin(GPIOB, LD3_Pin);

		if (calibrationVcoCV < maxVcoCV) {
			dacWrite(calibrationVcoCV, DAC_VCO_3340_FREQ);
		}
		else { // finish and switch to next vco or terminate calib process
			switch (calibrationCurrentVco) {
			case CALIB_VCO_3340:
				HAL_TIM_IC_Stop_IT(htimCalib, TIM_CHANNEL_CALIB_VCO3340);
				fillMidiToCVArray(tmpmidiToVCO3340CV);

				calibrationCurrentVco=CALIB_COMPLETED;

				//calibrationCurrentVco=CALIB_VCO_13700; // jump to next vco
				calibrationVcoCV = 20; // check starting value that guarantees stability
				//HAL_TIM_IC_Start_IT(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
				break;
			case CALIB_VCO_13700:
				HAL_TIM_IC_Stop_IT(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
				calibrationCurrentVco=CALIB_COMPLETED; // ok over
				break;
			case CALIB_COMPLETED:
				return;
			}
		}
	}

}

int checkMeasure(int i){

	if (i <= minVcoCV + 1) return 1;
	double x2 = voltageToFreq[i-2];
	double x3 = voltageToFreq[i-1];
	double x4 = voltageToFreq[i];
	double alpha = x3*x3/(x2*x4);

	if (alpha  < 1.1 && alpha > 0.9) return 1;
	else return 0;

}


/**
 * Called by HAL_TIM_IC_CaptureCallback.
 * BROKEN CODE !
 */
void calibrateVcoCallbackLong() {

	switch (calibrationCurrentVco) {
	case CALIB_VCO_3340: // ch 1
		calibrationCaptureValue =__HAL_TIM_GetCompare(htimCalib, TIM_CHANNEL_CALIB_VCO3340);
		break;
	case CALIB_VCO_13700: // ch 2
		calibrationCaptureValue =__HAL_TIM_GetCompare(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
		break;
	case CALIB_COMPLETED:
		return;
	}

	// initial step:
	if (calibrationVcoCyclesCount == -1) {
		__HAL_TIM_SetCounter(htimCalib, 0);
		calibrationVcoCyclesCount++;
	}

	// ultimate step: stores measure in table
	else if (calibrationVcoCyclesCount == CALIBRATION_PERIODS - 1) {

		switch (calibrationCurrentVco) {
		case CALIB_VCO_3340:
			//voltageToPeriod[calibrationVcoCV] = calibrationCaptureValue; DEBUG
			break;
		case CALIB_VCO_13700:
			//voltageToPeriod[calibrationVcoCV] = calibrationCaptureValue; DEBUG
			break;
		case CALIB_COMPLETED:
			return;
		}

		calibrationVcoCyclesCount = -1; // reinit for next freq

		if (calibrationVcoCV < 4096) {
			calibrationVcoCV += 16;
			dacWrite(calibrationVcoCV, DAC_VCO_3340_FREQ);
		} else {
			switch (calibrationCurrentVco) {
			case CALIB_VCO_3340:
				HAL_TIM_IC_Stop_IT(htimCalib, TIM_CHANNEL_CALIB_VCO3340);
				calibrationVcoCV = 0;
				//calibrationCurrentVco=CALIB_VCO_13700; // jump to next vco
				calibrationCurrentVco=CALIB_COMPLETED;
				//HAL_TIM_IC_Start_IT(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
				break;
			case CALIB_VCO_13700:
				HAL_TIM_IC_Stop_IT(htimCalib, TIM_CHANNEL_CALIB_VCO13700);
				calibrationCurrentVco=CALIB_COMPLETED; // ok over
				break;
			case CALIB_COMPLETED:
				return;
			}
		}
	}

	// intermediate steps:
	else {
		calibrationVcoCyclesCount++;
	}
}

/**
 * @param midiToVCO13700CV or midiToVCO3340CV
 */
void fillMidiToCVArray(int* midiToVcoCV){

	int dacLevel=0;
	float f;
	for (int midiNote=0; midiNote<128; midiNote++){
		f = midiToFreqArray[midiNote]; // tempered range
		if (f == 0) continue; // invalid midi note
		while (dacLevel < 4096 && voltageToFreq[dacLevel] < f){
			dacLevel++;
		}
		midiToVcoCV[midiNote] = dacLevel;
	}



}


