package view.component;
import java.io.IOException;

import com.pi4j.io.i2c.*; // pi4j-core.jar must be in the project build path! [SR]


/**
 * The IS31FL3731 is a device (also available as an Adafruit module, see https://www.adafruit.com/product/2946) 
 * that can drive up to 144 leds in PWM mode using a dedicated 9-line matrix multiplexing. 
 * It uses I2C for communications, see component <a href="http://www.issi.com/WW/pdf/31FL3731.pdf">datasheet</a>.
 * 
 * This Java class provides a wrapper to its low level functions based on the jPigpio library.
 * 
 * IS31FL3731 Registers:
 * 
 * the device is able to drive 2 x 72 = 144 leds sorted as two 9x8 submatrices
 * 
 * There are 8 frames for displaying animations + one special frame called "function register" (aka general configuration parameters)
 * Every frame is associated with the same set of LED registers (LED on/off, PWM, etc)
 * Special frame "function register" is associated with general configuration parameters
 * 
 * General Configuration  :
 * 1) i2cWrite (CMD_REG, PAGE_FUNCTIONREG) selects frame "function register" for general configuration
 * 2) i2cWrite (CONFIG, 0xFF) writes into configuration register
 * 
 * LED Configuration  :
 * 1) i2cWrite (CMD_REG, PAGE_FRAME1) selects frame "one"
 * 2) i2cWrite (ONOFF_REG_BASE_ADDR + 5, 0xFF) switches on the whole 3rd row of matrix B
 * 3) i2cWrite (BLINK_REG_BASE_ADDR + 5, 0xFF) same for blinking
 * 4) i2cWrite (PWM_REG_BASE_ADDR + 21, 0x80) set the PWM ratio of LED number "21" to 50% (=5th led of 2nd row)
 * 
 * @author S. Reynal July 2018
 * @author Lucien Manza Nov 2018
 * 
 */
public class IS31FL3731 {
	
	// -------------- fields --------------
	
	/* write to this register to select current frame register (or the Function Register, aka Page 9, which sets general parameters) */
	private static final int COMMAND_REGISTER = 0xFD; 
	private static final int FUNCTION_REGISTER = 0x0B;
	protected static I2CDevice i2cDevice; 
	
	// -------------- constructors --------------
	
	/**
	 * @throws Exception 
	 * 
	 */
	
	
	public IS31FL3731() throws Exception {
		//TODO - DONE
		
		// - init I2C bus, create device using given address
	
		i2cDevice = I2CFactory.getInstance(I2CBus.BUS_1).getDevice(DeviceAddress.AD_GND.getValue());
		
		
		// - select function register
		selectFunctionRegister();
		
		// - write appropriate parameter values to function register
		DisplayMode displayMode = DisplayMode.PICTURE_MODE;//TODO select the mode to use
		
		setDisplayMode(displayMode, 0);
		setDisplayedFrame(0);
		setAutoPlayLoopingParameters(1, 1);
		setAutoPlayFrameDelayTime(23);
		setDisplayOptions(false, false, 0);
		setAudioSynchronization(false);
		setBreathControl(false, 0, 0, 0);
		setShutdown(true);
		setAutoGainControl(false, false, 0);
		setAudioSampleRate(14);
		
		// - then selects default frame 1
		
		selectFrameRegister(0);
		
		
	}
	
	
	// -------------- public and protected methods --------------
		
	/**
	 * Configure the current display mode in the Configuration Register (00h)
	 * @param startFrame in Auto Frame Play Mode, sets the initial frame from which the animation starts
	 * @author SR
	 * @throws Exception 
	 */
	public void setDisplayMode(DisplayMode displayMode, int startFrame) throws Exception {
		
		startFrame %= 8; // must be lower than 8
		configure(FunctionRegister.CONFIG_REG, displayMode.getValue() | startFrame);
		
	}
	
	/**
	 * sets the currently displayed frame when in Picture Mode (01h)
	 * @author SR
	 * @throws Exception 
	 */
	public void setDisplayedFrame(int frame) throws Exception {
		
		configure(FunctionRegister.PICTURE_DISPLAY, frame % 8);
		
	}
	
	/**
	 * Sets looping parameter when in Auto Play Mode
	 * @param loopCount the number of loops playing, from 1 to 7 ; 0 for endless looping
	 * @param frameCount the number of frames playing, from 1 to 7 ; 0 for all frames
	 * @author SR
	 * @throws Exception 
	 */
	public void setAutoPlayLoopingParameters(int loopCount, int frameCount) throws Exception {
		
		loopCount %= 8;
		frameCount %= 8;
		configure(FunctionRegister.AUTOPLAY1, frameCount % 8);
		
		
	}
	
	/**
	 * Set the delay in MS between frames in Auto Play Mode. The methods picks the closest register parameter value.
	 * @param delayMs
	 * @author SR
	 * @throws Exception 
	 */
	public void setAutoPlayFrameDelayTime(int delayMs) throws Exception {
		
		// tau = 11ms (typ., see datasheet page 12)
		int fdt = (int)Math.round(0.09090909 * delayMs); // i.e. divided by 11ms
		if (fdt == 0) fdt = 1; // because fdt=0 means fdt=64, see datasheet
		else if (fdt > 63) fdt = 0;
		configure(FunctionRegister.AUTOPLAY2, fdt);
		
	}
	
	/**
	 * Set intensity and blink related parameters (05h)
	 * @param useFrame1IntensityForAllFrames if true, frame intensity use that of frame 1, otherwise each frame has its own intensity 
	 * @param blinkEnable enable led blinking
	 * @param blinkPeriodTimeSec sets the blinking period in seconds (picks the closest permitted value ; max is 2 seconds)
	 * @author SR
	 * @throws Exception 
	 */
	public void setDisplayOptions(boolean useFrame1IntensityForAllFrames, boolean enableBlink, double blinkPeriodTimeSec) throws Exception {
		
		// tau = 0.27s, see datasheet page 13
		int value = (int)Math.round(3.7037037 * blinkPeriodTimeSec);
		if (value > 7) value = 7;
		if (enableBlink) value |= 0x08;
		if (useFrame1IntensityForAllFrames) value |= 0x20;
		configure(FunctionRegister.DISP_OPTION, value);
	}
	
	/**
	 * @param enableSync enable audio signal to modulate the intensity of the matrix
	 * @throws Exception 
	 */
	public void setAudioSynchronization(boolean enableSync) throws Exception {
		
		if (enableSync)
			configure(FunctionRegister.AUDIO_SYNC, 0x01);
		else
			configure(FunctionRegister.AUDIO_SYNC, 0x00);
		
	}
	
	
	/**
	 * Set breathing parameters (registers 08h and 09h)
	 * @param enableBreathing
	 * @param fadeOutTimeMs
	 * @param fadeIntTimeMs
	 * @param extinguishTimeMs
	 * @throws Exception 
	 */
	public void setBreathControl(boolean enableBreathing, int fadeOutTimeMs, int fadeInTimeMs, int extinguishTimeMs) throws Exception {
		
		// TODO - DONE
		byte val1 = 0;
		byte val2 = 0;
		
		if (enableBreathing == false){
			
			val2 = val2 |= (byte) (1 << 4);
			configure(FunctionRegister.BREATH_CTRL2, val2);			
		}
		
		else {
			
			if (fadeOutTimeMs <8 && fadeInTimeMs <8) {
			
				val1 = val1 |= (byte) fadeInTimeMs;
				val1 = val1 |= (byte) (fadeOutTimeMs << 4);
			}
			
			if (extinguishTimeMs <8) {
			
				val2 = val2 |= (byte) extinguishTimeMs;
				val2 = val2 |= (byte) (1 << 4);
			
			}
		
			configure(FunctionRegister.BREATH_CTRL1, val1);
			configure(FunctionRegister.BREATH_CTRL2, val2);
			
		}
		
		
		
	}
	
	/**
	 * Shutdown register.
	 * @param normal if true, sets the device to normal mode, otherwise shut it down (reduces energy)
	 * @throws Exception 
	 */
	public void setShutdown(boolean normal) throws Exception {
		
		// TODO - DONE
		
		if (normal == false) {
			
			configure(FunctionRegister.SHUTDOWN, 0x0);
			
		}
		
		else {
			
			configure(FunctionRegister.SHUTDOWN, 0x01);
			
		}
		
	}
	
	/**
	 * AGC Control Register (0Bh)
	 * @param enableAGC
	 * @param fastMode
	 * @param audioGain from 0dB to 21dB
	 * @throws Exception 
	 */
	public void setAutoGainControl(boolean enableAGC, boolean fastMode, int audioGain) throws Exception {
		
		int ags = audioGain / 3;
		if (ags < 0) ags = 0;
		else if (ags > 7) ags = 7;
		
		// TODO - DONE
		
		byte val = 0;
		
		val = val |= ags;
		
		if (enableAGC == false) { val |= 1 << 4;}
		
		else { val |= 1 << 4;}
		
		if (fastMode == false) { val |= 0 << 5;}
		
		else { val |= 1 << 5;}
		
		configure(FunctionRegister.AGC, val);
	}
	
	/**
	 * Sets the audio sammple rate of the input signal when in Audio Frame Play Mode.
	 * @param sampleRateMs
	 * @throws Exception 
	 */
	public void setAudioSampleRate(int sampleRateMs) throws Exception {
		
		// TODO - DONE
		if (sampleRateMs == 0) {
			configure(FunctionRegister.AUDIO_ADC_RATE, 256);
		}
		
		else if (sampleRateMs>0 && sampleRateMs<256) {
			configure(FunctionRegister.AUDIO_ADC_RATE, sampleRateMs);
		}
	}
	
	// TODO : - DONE write a method that reads the Frame State Register (07h)
	
	public int readFrameStateRegister () throws Exception {
		
		selectFunctionRegister();
		int address = FunctionRegister.FRAME_STATE.getAddress(); 
		int val = i2cRead(address);
		
		val &= 0b111;
		
		return val;
		
	}
	
	/**
	 * Switch the given LED on or off
	 * @param row
	 * @param col 0 <= col <= 7 : matrix A ; 8 <= col <= 15 : matrix B
	 * @param state true for the "on" state, false otherwise
	 * @throws IOException in case byte cannot be written to the i2c device or i2c bus
	 */
	static void switchLED(int row, int col, int state) throws IOException{
		
		  int reg, bit;
		  col &= 0xF; // restrains to 0-15
		  if (col < 8) { // matrix A
		      reg = FrameRegister.ONOFF_REG_BASE_ADDR.getAddress() + 2*row;
		      bit  = 1 << (col & 7) ;
		  }
		  else { // matrix B
		      reg = FrameRegister.ONOFF_REG_BASE_ADDR.getAddress() + 2*row + 1;
		      col -= 8;
		      bit  = 1 << (col & 7) ;
		  }

		  int old = i2cDevice.read(reg);
		  if (state == 0)
		    old &= (~bit) ;
		  else
		    old |=   bit ;
		  i2cDevice.write(reg, (byte)old);		
	}
	
	/**
	 * Sets the intensity of the given LED.
	 * @param row
	 * @param col 0 <= col <= 7 : matrix A ; 8 <= col <= 15 : matrix B
	 * @param pwm 0-255
	 * @throws IOException in case byte cannot be written to the i2c device or i2c bus
	 */
	static void setLEDpwm(int row, int col, int pwm) throws IOException {

		col &= 0xF; // make sure it's inside 0-15
		int reg = FrameRegister.PWM_REG_BASE_ADDR.getAddress() + 16*row + col;
		i2cDevice.write(reg, (byte)(pwm & 0xFF));
		
	}
	
	/**
	 * Display a bargraph-like picture from the given "val"
	 * @throws IOException in case byte cannot be written to the i2c device or i2c bus 
	 */
	void bargraph(int val) throws IOException {
		
	  switch(val & 0xFF){ // make sure val is  lower than 16
	    case 0:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x00);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 1:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x01);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 2:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x03);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 3:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x07);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 4:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x0F);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 5:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x1F);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 6:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x3F);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 7:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0x7F);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 8:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x00);
	            break;
	    case 9:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x01);
	            break;
	    case 10:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x03);
	            break;
	    case 11:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x07);
	            break;
	    case 12:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x0F);
	            break;
	    case 13:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x1F);
	            break;
	    case 14:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x3F);
	            break;
	    case 15:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0x7F);
	            break;
	    case 16:
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+1, (byte)0xFF);
	            i2cDevice.write(FrameRegister.ONOFF_REG_BASE_ADDR.getAddress()+3, (byte)0xFF);
	            break;
	    }	
	}
	// -------------- private or package methods --------------

	/**
	 * Selects one of 8 possible frames (aka pages) for further configuration. 
	 * Every frame is a picture with independent LED configurations.
	 * Further writing to registers will be directed to the active frame, 
	 * though it doesn't mean this frame is the currently displayed one (both things are independent).
	 * This method write to the special 0xFD command register.
	 * @author SR
	 * @throws Exception 
	 */
	private void selectFrameRegister(int frame) throws Exception {
		
		if (frame < 0 || frame > 8) throw new IllegalArgumentException("Valid page number ranges from 0 to 7 : " + frame);
		
		// TODO : - DONE write "frame" to COMMAND_REGISTER
		
		i2cWrite (COMMAND_REGISTER, frame);
		
	}
	
	/**
	 * Selects the special "Function register page" for further configuration.
	 * Further writing to registers will be directed to this special page. 
	 * This method write to the special 0xFD command register.
	 * @author SR
	 * @throws Exception 
	 */
	private void selectFunctionRegister() throws Exception {
	
		// TODO - DONE : write FUNCTION_REGISTER to COMMAND_REGISTER
		
		i2cWrite(COMMAND_REGISTER, 0b1011);
	}
	
	/**
	 * Write the given value to the given FunctionRegister
	 * @param register
	 * @param value may be the result of FunctionRegisterMask or'ed together
	 */
	private void configure(FunctionRegister register, int value) throws Exception {
		
		// TODO - DONE : read Command Register to know what is the currently active page so that we can go back to it later
		
		int current = i2cRead(COMMAND_REGISTER);
		
		selectFunctionRegister();

		// TODO - DONE: write value to register.getAddress()
		
		i2cWrite (register.getAddress(),value);
		
		// TODO - DONE : make previously active PAGE active again
		
		i2cWrite (COMMAND_REGISTER, current);
	}
	
	
	// Set Mode of a row
	// As one entire row has two encoder (column A and B), return state LED register address of 1 row among the 18 
	protected static int registerOfStateLED(int row) { //@Lucien : private to protected for BarGraph class
		
		int addr = FrameRegister.ONOFF_REG_BASE_ADDR.getAddress() + 2*row;
		if (row%2 == 0) System.out.println("You select State Mode for A" + row +"row");
		else System.out.println("You select State Mode for B" + row +"row");
		return addr;
	}
	
	// As one entire row has two encoder (column A and B), return PWM LED register address of 1 row among the 18 
	protected int registerOfPWMLED(int row) { //@Lucien : private to protected for BarGraph class
		
		int addr = FrameRegister.PWM_REG_BASE_ADDR.getAddress() + 255*row;
		if (row%2 == 0) System.out.println("You select PWM Mode for A" + row +"row");
		else System.out.println("You select PWM Mode for B" + row +"row");
		return addr;
		
	}	
	
	// -------------- enums and inner classes --------------
	
	/**
	 * An enumeration of possible I2C device addresses depending on the connection of the "AD" pin
	 * @author SR
	 */
	public static enum DeviceAddress {
		
		/** device I2C address with AD connected to GND */
		AD_GND(0x74), 
		/** device I2C address with AD connected to SCL */
		AD_SCL(0x75),
		/** device I2C address with AD connected to SDA */
		AD_SDA(0x76),
		/** device I2C address with AD connected to VCC */
		AD_VCC(0x77);

		private int address; 
		
		DeviceAddress(int address){
			this.address = address;
		}
		
		public int getValue() {
			return address;
		}
	}
		
	/**
	 * an enumeration of registers that can be used when a "frame" page is currently active
	 * When the "function register" is active, use a FunctionRegister instead.
	 * @author SR
	 */
	private static enum FrameRegister {
		
		
		// LED coordinates: we must consider matrices A and B as making up a single 16 column wide matrix!
		// so 0x00 = first row, left part (matrix A)
		// and 0x01 = first row, right part (matrix B)
		// etc
		ONOFF_REG_BASE_ADDR(0x00), // there are 18 such registers, two for each row (A then B)
		BLINK_REG_BASE_ADDR(0x12), // ibid (remember to configure DISP_OPTION first by setting bit BE to one)
		PWM_REG_BASE_ADDR(0x24); // there are 144 such registers, one for each LED (from left to right)

	
		private int address; // the register address or the constant data field
		
		FrameRegister(int address){
			this.address = address;
		}
		
		public int getAddress() {
			return address;
		}

	}
	
	/**
	 * An enumeration of registers that can be used when the "function register" (aka Page 9) is currently active.
	 * When a "frame" page is currently active, use a FrameRegister instead. 
	 * @author SR
	 */
	private static enum FunctionRegister {
		
		/** configure the operation mode: static (aka "picture") vs animation vs audio modulation */
		CONFIG_REG(0x00), // 
		/** in picture mode, chooses with picture to display (frame 1 by default) */
		PICTURE_DISPLAY(0x01),
		/** for animations (number of loops, number of frames) */
		AUTOPLAY1(0x02), 
		/** for animations (frame delay time) */
		AUTOPLAY2(0x03), 
		// 0x04 reserved
		/** useful parameters for blinking (enable, global blinking period and duty cycle)  */
		DISP_OPTION(0x05),  
		/** enable or disable audio */
		AUDIO_SYNC(0x06),  
		/** enable interrupt when movie is finished ; a read operation provides currently displayed frame number */
		FRAME_STATE(0x07),  
		/**  sets fade in and out times */
		BREATH_CTRL1(0x08),  
		/** enables breathing */
		BREATH_CTRL2(0x09),  
		/** Software shutdown (not hardware shutdown with pin SDB) ; write 0x01 to leave shutdown */
		SHUTDOWN(0x0A),  
		/** slow/fast AGC, enables AGC, audio gain */
		AGC(0x0B), 
		/** sets audio sample rate */
		AUDIO_ADC_RATE(0x0C); 

		private int address; 
		
		FunctionRegister(int address){
			this.address = address;
		}
		
		public int getAddress() {
			return address;
		}
	}	

	/**
	 * an enumeration of available parameters for the Display Mode field in the "Configuration Register (00h)"
	 * @author SR
	 */
	public static enum DisplayMode {
		
		PICTURE_MODE(0b00000000),
		AUTO_FRAME_PLAY_MODE(0b00001000),
		AUDIO_FRAME_PLAY_MODE(0b00010000);
		
		private int mask; 
		
		DisplayMode(int mask){
			this.mask = mask;
		}
		
		public int getValue() {
			return mask;
		}		
	}
	
	public void i2cWrite(int adress, int value) throws Exception{
		
		//INTIALISATION DU BUS1
		I2CBus bus1 = I2CFactory.getInstance(I2CBus.BUS_1);
		I2CDevice IS31FL3731 = bus1.getDevice((byte)0x74);
		
		IS31FL3731.write(adress,(byte) value);
		
	}
	
	public int i2cRead(int adress) throws Exception {
		
		//INTIALISATION DU BUS1
		I2CBus bus1 = I2CFactory.getInstance(I2CBus.BUS_1);
		I2CDevice IS31FL3731 = bus1.getDevice((byte)0x74);
		
		return IS31FL3731.read(adress);
		
	}
	
// NOT USED ANYMORE but wait before removing permanently (SR)	
//	/**
//	 * An enumeration of useful masks (to be or'ed together) for registers configuration
//	 * when the "function register" (aka Page 9) is currently active.
//	 * Example of use: 
//	 * i2cWrite(handle, FunctionRegister.CONFIG_REG, FunctionRegisterMask.CONFIG_REG_PICTURE_MODE.or(FunctionRegisterMask.CONFIG_REG_FRAME_START_FRAME2)
//	 * @author SR
//	 */
//	public static enum FunctionRegisterMask {
//		
//		/** page 12 of datasheet */
//		AUTOPLAY1_REG_ENDLESSLOOP(0x00),
//		AUTOPLAY1_REG_ALLFRAME(0x00),
//		// TODO SR : to be continued!
//		
//		private int mask; 
//		
//		FunctionRegisterMask(int mask){
//			this.mask = mask;
//		}
//		
//		public int getValue() {
//			return mask;
//		}
//		
//		/** performs a logical OR between this mask and the given mask varargs and returns the result */
//		public int or(FunctionRegisterMask... other) {
//			int result = this.mask;
//			for (FunctionRegisterMask m : other)
//				result |= m.mask;
//			return result;
//		}
//	}	
	
	
	// -------------- test methods --------------
	
	//public static void main(String[] args) {
		
	//	testFunctionRegister();
	//}

	// testing function register
	private static void testFunctionRegister() {
		
		//Write the Exception method
		
		
	}
	

}
