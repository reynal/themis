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
		
		DETUNE_3340A(14),
		OCTAVE_3340A(74),
		WAVE_3340A(15),
		PWM_3340A(16),
		LEVEL_3340A(17),
		SYNC_3340A(77),

		OCTAVE_3340B(75),
		DETUNE_3340B(18),
		LEVEL_TRI_3340B(19),
		LEVEL_PULSE_3340B(20),
		LEVEL_SAW_3340B(21),
		PWM_3340B(22),

		DETUNE_13700(13),
		OCTAVE_13700(76),
		LEVEL_TRI_13700(24),
		LEVEL_SQU_13700(25),

		VCF_CUTOFF(28),
		VCF_RESONANCE(30),
		VCF_KBDTRACKING(31),
		VCF_EG(36),

		VCF_ATTACK(32),
		VCF_DECAY(33),
		VCF_SUSTAIN(34),
		VCF_RELEASE(35),
		
		VCA_ATTACK(37),
		VCA_DECAY(38),
		VCA_SUSTAIN(39),
		VCA_RELEASE(40),
		
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


