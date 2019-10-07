package model;

/**
 * An enum for all possible Midi Control Change (aka MidiCC) supported by the underlying hardware.
 * 
 * @see dac_board.c file in the STM32 project
 * 
 * @author reynal
 *
 */
public enum MidiCCImplementation {
		
		WAVE_3340(9),
		DETUNE_13700(10),
		WAVE_13700(11),
		PWM_3340(14),
		LEVEL_3340(15),
		LEVEL_13700(16),
		VCF_CUTOFF(19),
		VCF_RESONANCE(21),
		VCF_KBDTRACKING(22),
		VCF_ATTACK(23),
		VCF_DECAY(24),
		VCF_SUSTAIN(25),
		VCF_RELEASE(26),
		VCF_EG(27),
		VCA_ATTACK(28),
		VCA_DECAY(29),
		VCA_SUSTAIN(30),
		VCA_RELEASE(31),
		OCTAVE_3340(74),
		OCTAVE_13700(75),
		SYNC_3340(77),
		VCA_VELOCITY_SENSITIVITY(104),
		VCF_ORDER(109),
		VCF_VELOCITY_SENSITIVITY(110),
		CALIBRATE(127);
		
		int code;

		/**
		 * @return the MIDI code for this Control Change
		 */
		public int getCode() {
			return code;
		}

		/**
		 * @param code
		 */
		private MidiCCImplementation(int code) {
			this.code = code;
		}

}


