
public class Registres {
	
	byte 	IS31FL3731_BUS1 = (byte)0x74; //Adress of the first Bus I²C
	byte 	IS31FL3731_BUS2 = (byte)0x75; //Adress of the second Bus I²C
	byte 	IS31FL3731_BUS3 = (byte)0x76; //Adress of the third Bus I²C
	
	byte	IS31FL3731_CMD_REG =         (byte) 0xFD; // write to this register to select current frame register (or the Function Register, aka Page 9, which sets general parameters)
	byte	IS31FL3731_PAGE_FRAME1 =     (byte) 0x00; // page 1
	byte	IS31FL3731_PAGE_FRAME2 =	 (byte) 0x01; // page 2
	byte	IS31FL3731_PAGE_FRAME3	=    (byte) 0x02; // page 3
	byte	IS31FL3731_PAGE_FRAME4	=	 (byte) 0x03; // page 4
	byte	IS31FL3731_PAGE_FRAME5	=	 (byte) 0x04; // page 5
	byte	IS31FL3731_PAGE_FRAME6	=	 (byte) 0x05; // page 6
	byte	IS31FL3731_PAGE_FRAME7	=	 (byte) 0x06; // page 7
	byte	IS31FL3731_PAGE_FRAME8	=	 (byte) 0x07; // page 8
	byte	IS31FL3731_PAGE_FUNCTIONREG	=(byte) 0x0B; // Function register

	// LED coordinates: we must consider matrices A and B as making up a single 16 column wide matrix!
	// so 0x00 = first row, left part (matrix A)
	// and 0x01 = first row, right part (matrix B)
	// etc
	int	IS31FL3731_ONOFF_REG_BASE_ADDR = (byte) 0x00; // there are 18 such registers, two for each row (A then B)
	int	IS31FL3731_BLINK_REG_BASE_ADDR = (byte) 0x12; // ibid (remember to configure IS31FL3731_DISP_OPTION first by setting bit BE to one)
	int	IS31FL3731_PWM_REG_BASE_ADDR =   (byte) 0x24; // there are 144 such registers, one for each LED (from left to right)

	// Function register : ("PAGE9")
	byte	IS31FL3731_CONFIG_REG	=	       (byte) 0x00; // static (aka "picture") vs animation vs audio modulation
	byte	IS31FL3731_PICTURE_DISPLAY =	   (byte) 0x01; // in picture mode, chooses with picture to display (frame 1 by default)
	byte	IS31FL3731_AUTOPLAY1 =         	   (byte) 0x02; // for animations (number of loops, number of frames)
	byte	IS31FL3731_AUTOPLAY2 =         	   (byte) 0x03; // for animations (frame delay time)
	// 0x04 reserved
	byte	IS31FL3731_DISP_OPTION =           (byte) 0x05; // useful parameters for blinking (enable, global blinking period and duty cycle)
	byte	IS31FL3731_AUDIO_SYNC =    		   (byte) 0x06; // enable or disable audio
	byte	IS31FL3731_FRAME_STATE =   		   (byte) 0x07; // enable interrupt when movie is finished ; a read operation provides currently displayed frame number
	byte	IS31FL3731_BREATH_CTRL1 = 	       (byte) 0x08; // sets fade in and out times
	byte	IS31FL3731_BREATH_CTRL2 =   	   (byte) 0x09; // enables breathing
	byte	IS31FL3731_SHUTDOWN =      		   (byte) 0x0A; // Software shutdown (not hardware shutdown with pin SDB) ; write 0x01 to leave shutdown
	byte	IS31FL3731_AGC =           		   (byte) 0x0B; // slow/fast AGC, enables AGC, audio gain
	byte	IS31FL3731_AUDIO_ADC_RATE =		   (byte) 0x0C; // sets audio sample rate

	
	protected int rowRegisterOfStateLED(int row) {
		
		int adr = IS31FL3731_ONOFF_REG_BASE_ADDR + 2*row;
		if (row%2 == 0) System.out.println("You select State Mode for A" + row +"row");
		else System.out.println("You select State Mode for B" + row+1 +"row");
		return adr;
	}
	
	protected int rowRegisterOfPWMLED(int row) {
		
		int adr = IS31FL3731_PWM_REG_BASE_ADDR+(int)0xFF*row;
		if (row%2 == 0) System.out.println("You select PWM Mode for A" + row +"row");
		else System.out.println("You select PWM Mode for B" + row+1 +"row");
		return adr;
	}
}
