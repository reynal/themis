package device;

import java.awt.*;
import java.io.*;
import javax.swing.*;

import com.pi4j.io.i2c.*; // pi4j-core.jar must be in the project build path! [SR]
import com.pi4j.io.i2c.I2CFactory.UnsupportedBusNumberException;

import java.util.Scanner;
import java.util.logging.*;

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
	protected I2CDevice i2cDevice;
	private static final Logger LOGGER = Logger.getLogger("confLogger");
	public static final int MIN_PWM = 0;
	public static final int MAX_PWM = 255;
	
	public static final int[] GAMMA_CORRECTION_32 = { // dataseeht page 17
			0, 1, 2, 4, 6, 10, 13, 18,
			22, 28, 33, 39, 46, 53, 61, 69,
			78, 86, 96, 106, 116, 126, 138, 149,
			161, 173, 186, 199, 212, 226, 240, 255
	};
	
	public static final int[] GAMMA_CORRECTION_16 = {0, 2, 4, 6, 10, 13, 18, 22, 28, 33, 46, 61, 86, 116, 161, 199, 255};
	


	// TODO : store led coordinates being used by View's (aka register led / unregister led)
	
	// -------------- constructors --------------
	
	/**
	 * @throws UnsupportedBusNumberException 
	 * @throws IOException 
	 * 
	 */
	public IS31FL3731() throws IOException, UnsupportedBusNumberException  {
		
		
		// - init I2C bus, create device using given address
		i2cDevice = I2CFactory.getInstance(I2CBus.BUS_1).getDevice(DeviceAddress.AD_GND.getValue());
		LOGGER.info("I2C Bus ok");
		
		// - select function register
		selectFunctionRegister();
		
		// - write appropriate parameter values to function register
		// default setDisplayMode(DisplayMode.PICTURE_MODE, 0);
		// default setDisplayedFrame(0);
		// default setAutoPlayLoopingParameters(1, 1);
		// default setAutoPlayFrameDelayTime(23);
		// default setDisplayOptions(false, false, 0);
		// default setAudioSynchronization(false);
		// default setBreathControl(false, 0, 0, 0);
		setShutdown(false); // get out of shutdown mode
		// default setAutoGainControl(false, false, 0);
		// default setAudioSampleRate(14);
		
		// - then selects default frame 1
		
		selectFrameRegister(0);
		
		// switch all LEDs off and set all pwm to 0
		LOGGER.info("Init LED state...");
		int addr = FrameRegister.ONOFF_REG_BASE_ADDR.getAddress();
		for (int i=0; i<18; i++)  i2cDevice.write(addr+i, (byte)0); // OxOO to Ox11 = 18 registers (turn all LEDs off)
		addr = FrameRegister.PWM_REG_BASE_ADDR.getAddress();
		for (int i=0; i<144; i++) i2cDevice.write(addr+i, (byte)0); // 0x24 to 0xB3 = 144 registers (set all PWM duty cycle data to 0)
		LOGGER.info("LED state init'd");
	}
	
	
	// -------------- public and protected methods --------------
		
	/**
	 * Configure the current display mode in the Configuration Register (00h)
	 * @param startFrame in Auto Frame Play Mode, sets the initial frame from which the animation starts
	 * @author SR
	 */
	public void setDisplayMode(DisplayMode displayMode, int startFrame) throws IOException {
		
		startFrame %= 8; // must be lower than 8
		configure(FunctionRegister.CONFIG_REG, displayMode.getValue() | startFrame);
		
	}
	
	/**
	 * sets the currently displayed frame when in Picture Mode (01h)
	 * @author SR
	 */
	public void setDisplayedFrame(int frame) throws IOException {
		
		configure(FunctionRegister.PICTURE_DISPLAY, frame % 8);
		
	}
	
	/**
	 * Sets looping parameter when in Auto Play Mode
	 * @param loopCount the number of loops playing, from 1 to 7 ; 0 for endless looping
	 * @param frameCount the number of frames playing, from 1 to 7 ; 0 for all frames
	 * @author SR
	 */
	public void setAutoPlayLoopingParameters(int loopCount, int frameCount) throws IOException {
		
		loopCount %= 8;
		frameCount %= 8;
		configure(FunctionRegister.AUTOPLAY1, frameCount % 8);
		
		
	}
	
	/**
	 * Set the delay in MS between frames in Auto Play Mode. The methods picks the closest register parameter value.
	 * @param delayMs
	 * @author SR
	 */
	public void setAutoPlayFrameDelayTime(int delayMs) throws IOException {
		
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
	 */
	public void setDisplayOptions(boolean useFrame1IntensityForAllFrames, boolean enableBlink, double blinkPeriodTimeSec) throws IOException {
		
		// tau = 0.27s, see datasheet page 13
		int value = (int)Math.round(3.7037037 * blinkPeriodTimeSec);
		if (value > 7) value = 7;
		if (enableBlink) value |= 0x08;
		if (useFrame1IntensityForAllFrames) value |= 0x20;
		configure(FunctionRegister.DISP_OPTION, value);
	}
	
	/**
	 * @param enableSync enable audio signal to modulate the intensity of the matrix
	 */
	public void setAudioSynchronization(boolean enableSync) throws IOException {
		
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
	 */
	public void setBreathControl(boolean enableBreathing, int fadeOutTimeMs, int fadeInTimeMs, int extinguishTimeMs) throws IOException {
		
		byte val1 = 0;
		byte val2 = 0;
		
		if (enableBreathing == false){
			
			val2 |= (byte) (1 << 4);
			configure(FunctionRegister.BREATH_CTRL2, val2);			
		}
		
		else {
			
			if (fadeOutTimeMs <8 && fadeInTimeMs <8) {
			
				val1 |= (byte) fadeInTimeMs;
				val1 |= (byte) (fadeOutTimeMs << 4);
			}
			
			if (extinguishTimeMs <8) {
			
				val2 |= (byte) extinguishTimeMs;
				val2 |= (byte) (1 << 4);
			
			}
		
			configure(FunctionRegister.BREATH_CTRL1, val1);
			configure(FunctionRegister.BREATH_CTRL2, val2);
			
		}
		
		
		
	}
	
	/**
	 * Shutdown register.
	 * @param enable if true, shut it down (reduces energy) otherwise (if false) sets the device to normal mode
	 */
	public void setShutdown(boolean state) throws IOException {
		
		LOGGER.info(state ? "Entering sw shutdown" : "Exiting sw shutdown");
		
		if (state == true) {
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
	 */
	public void setAutoGainControl(boolean enableAGC, boolean fastMode, int audioGain) throws IOException {
		
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
	 * Sets the audio sample rate of the input signal when in Audio Frame Play Mode.
	 * @param sampleRateMs
	 */
	public void setAudioSampleRate(int sampleRateMs) throws IOException {
		
		// TODO - DONE
		if (sampleRateMs == 0) {
			configure(FunctionRegister.AUDIO_ADC_RATE, 256);
		}
		
		else if (sampleRateMs>0 && sampleRateMs<256) {
			configure(FunctionRegister.AUDIO_ADC_RATE, sampleRateMs);
		}
	}
	
	/**
	 * Read the value of the FrameState register, i.e., the index of the currently active frame.
	 * @throws IOException
	 */
	public int readFrameStateRegister () throws IOException {
		
		selectFunctionRegister();
		int address = FunctionRegister.FRAME_STATE.getAddress(); 
		int val = i2cDevice.read(address);
		return val;
		
	}
	
	/**
	 * Switch the given LED on or off
	 * @param row
	 * @param col 0 <= col <= 7 : matrix A ; 8 <= col <= 15 : matrix B
	 * @param state true for the "on" state, false otherwise
	 * @throws IOException in case byte cannot be written to the i2c device or i2c bus
	 */
	public void switchLED(LEDCoordinate ledCoordinate, boolean state) throws IOException{
		
		  int reg = getLEDRowRegisterAdress(ledCoordinate.getRow(), ledCoordinate.AorB); 
		  int bit  = 1 << (ledCoordinate.getColumn() & 7) ;
		  int old = i2cDevice.read(reg);
		  if (state == false)
		    old &= (~bit) ;
		  else
		    old |=   bit ;
		  i2cDevice.write(reg, (byte)old);		
	}
	
	/**
	 * Switches a complete row of 8 LEDs on or off
	 * @param row
	 * @param onLeds a byte with 1 where LEDs are on and 0 otherwise
	 * @param m either A or B (see device datasheet)
	 * @throws IOException in case byte cannot be written to the i2c device or i2c bus
	 */
	public void switchLEDRow(int row, Matrix m, int onLeds) throws IOException{
		
		  int reg = getLEDRowRegisterAdress(row, m);
		  i2cDevice.write(reg, (byte)(onLeds & 0xFF));
	}
	
	/**
	 * Sets the intensity of the given LED.
	 * @param row
	 * @param col 0 <= col <= 7
	 * @param m A or B
	 * @param pwm 0-255
	 * @throws IOException in case byte cannot be written to the i2c device or i2c bus
	 */
	public void setLEDpwm(LEDCoordinate ledCoordinate, int pwm) throws IOException {
		
		int reg = ledCoordinate.getPWMRegisterAdress();
		i2cDevice.write(reg, (byte)(pwm & 0xFF));
	}
	
	/**
	 * Sets the intensity of the given LED after applying a 32-step gamma correction
	 * @param ledCoordinate
	 * @param pwmGammaCorrected32 0 to 31
	 * @throws IOException
	 */
	public void setLEDpwmGammaCorrected32(LEDCoordinate ledCoordinate, int pwmGammaCorrected32) throws IOException {
		
		if (pwmGammaCorrected32 > 31) pwmGammaCorrected32=31;
		else if (pwmGammaCorrected32<0) pwmGammaCorrected32=0;
		
		setLEDpwm(ledCoordinate, GAMMA_CORRECTION_32[pwmGammaCorrected32]);
	}
	
	/**
	 * Sets the intensity of the given LED after applying a 16-step gamma correction
	 * @param ledCoordinate
	 * @param pwmGammaCorrected16 0 to 15
	 * @throws IOException
	 */
	public void setLEDpwmGammaCorrected16(LEDCoordinate ledCoordinate, int pwmGammaCorrected16) throws IOException {
		
		if (pwmGammaCorrected16 > 15) pwmGammaCorrected16=15;
		else if (pwmGammaCorrected16<0) pwmGammaCorrected16=0;
		
		setLEDpwm(ledCoordinate, GAMMA_CORRECTION_16[pwmGammaCorrected16]);
	}


	// -------------- private or package methods --------------

	/**
	 * Selects one of 8 possible frames (aka pages) for further configuration. 
	 * Every frame is a picture with independent LED configurations.
	 * Further writing to registers will be directed to the active frame, 
	 * though it doesn't mean this frame is the currently displayed one (both things are independent).
	 * This method write to the special 0xFD command register.
	 * @author SR
	 */
	private void selectFrameRegister(int frame) throws IOException {
		
		if (frame < 0 || frame > 8) throw new IllegalArgumentException("Valid page number ranges from 0 to 7 : " + frame);
		
		i2cDevice.write(COMMAND_REGISTER, (byte)frame);
		
	}
	
	/**
	 * Selects the special "Function register page" for further configuration.
	 * Further writing to registers will be directed to this special page. 
	 * This method write to the special 0xFD command register.
	 * @author SR
	 * @throws IOException 
	 */
	private void selectFunctionRegister() throws IOException  {
	
		i2cDevice.write(COMMAND_REGISTER, (byte)FUNCTION_REGISTER);
	}
	
	/**
	 * Write the given value to the given FunctionRegister
	 * @param register
	 * @param value may be the result of FunctionRegisterMask or'ed together
	 * @throws IOException 
	 */
	private void configure(FunctionRegister register, int value) throws IOException  {
		
		// read Command Register to keep track of the currently active page so that we can go back to it later
		int current = i2cDevice.read(COMMAND_REGISTER);
		if (current != FUNCTION_REGISTER) selectFunctionRegister();

		i2cDevice.write(register.getAddress(),(byte)value);
		
		// make previously active PAGE active again if this wasn't the FUNCTION REGISTER page		
		if (current != FUNCTION_REGISTER) i2cDevice.write(COMMAND_REGISTER, (byte)current);
	}
	
	
	/**
	 * @param ledRow
	 * @param matrixAB
	 * @return the adress of the ON/OFF register for the given row
	 */
	public static int getLEDRowRegisterAdress(int ledRow, Matrix matrixAB) { 
		
		int addr = FrameRegister.ONOFF_REG_BASE_ADDR.getAddress() + 2*ledRow;
		if (matrixAB == Matrix.B) addr++;
		return addr;
	}
	
	/**
	 * @param row
	 * @return the adress of the PWM register for the given (row,col) coordinate
	 */
	public static int getPWMRegisterAdress(LEDCoordinate ledCoordinate) { 
		
		return ledCoordinate.getPWMRegisterAdress();
		
	}	
	
	// -------------- enums and inner classes --------------
	
	/**
	 * this class represents a pair of (row, column) coordinate for a given matrix.
	 * 
	 * @author SR
	 *
	 */
	public static class LEDCoordinate { 

		private static final long serialVersionUID = 1L;
		private static final boolean APPLY_BUG_FIX = true;
		public Matrix AorB;
		private int col, row;

		@SuppressWarnings("unused")
		public LEDCoordinate(int row, int col, Matrix AorB) {
			this.row = row % 9;
			this.col = col % 8;
			this.AorB = AorB;
			
			// for matrix A, LED were soldered the opposite way (with A and K reversed)... and we need to switch another LED to obtain the desired result
			if (APPLY_BUG_FIX && AorB == Matrix.A) { // comment out if hardware is fine!
				//System.out.print("Applying bug fix for " + this);
				Point trueLED = bugFixMap[row][col];
				this.row = trueLED.x; 
				this.col = trueLED.y;
				//System.out.println(" -> " + this);
			}
			
		}
		public int getPWMRegisterAdress() {
			int addr = FrameRegister.PWM_REG_BASE_ADDR.getAddress();
			addr += row * 16; // 16 leds per row (A+B)
			addr += col;
			if (AorB == Matrix.B) addr += 8;
			return addr;
		}
		
		public int getColumn() { return col;}
		
		public int getRow() { return row;}
		
		public void setColumn(int col) { this.col = col;}
		
		public void setRow(int row) { this.row = row;}
		
		public boolean equals(Object obj) {			
			return super.equals(obj) && ((LEDCoordinate)obj).AorB == this.AorB; 
		}
		
		public String toString() {
			return AorB + "(" + row + "," + col + ")";
		}
		
		private static Point[][] bugFixMap;
		
		// bug fix for the A matrix (with green LEDs), where LED were soldered in the wrong direction...
		// the following HashMap maps the LED we want to switch to the LED we should actually switch physically
		// for example switching C1-1 (row=0, col=0) actually switches C2-1 (row=1, col=0)
		static {
			bugFixMap = new Point[9][8]; //Point(row, col)
			
			bugFixMap[0][0]=new Point(1,0); // C1-1 -> C2-1
			bugFixMap[0][1]=new Point(2,0); // C1-2 -> C3-1
			bugFixMap[0][2]=new Point(3,0);
			bugFixMap[0][3]=new Point(4,0);
			bugFixMap[0][4]=new Point(5,0);
			bugFixMap[0][5]=new Point(6,0);
			bugFixMap[0][6]=new Point(7,0);
			bugFixMap[0][7]=new Point(8,0);

			bugFixMap[1][0]=new Point(0,0);
			bugFixMap[1][1]=new Point(2,1);
			bugFixMap[1][2]=new Point(3,1);
			bugFixMap[1][3]=new Point(4,1);
			bugFixMap[1][4]=new Point(5,1);
			bugFixMap[1][5]=new Point(6,1);
			bugFixMap[1][6]=new Point(7,1);
			bugFixMap[1][7]=new Point(8,1);

			bugFixMap[2][0]=new Point(0,1);
			bugFixMap[2][1]=new Point(1,1);
			bugFixMap[2][2]=new Point(3,2);
			bugFixMap[2][3]=new Point(4,2);
			bugFixMap[2][4]=new Point(5,2);
			bugFixMap[2][5]=new Point(6,2);
			bugFixMap[2][6]=new Point(7,2);
			bugFixMap[2][7]=new Point(8,2);

			bugFixMap[3][0]=new Point(0,2);
			bugFixMap[3][1]=new Point(1,2);
			bugFixMap[3][2]=new Point(2,2);
			bugFixMap[3][3]=new Point(4,3);
			bugFixMap[3][4]=new Point(5,3);
			bugFixMap[3][5]=new Point(6,3);
			bugFixMap[3][6]=new Point(7,3);
			bugFixMap[3][7]=new Point(8,3);

			bugFixMap[4][0]=new Point(0,3);
			bugFixMap[4][1]=new Point(1,3);
			bugFixMap[4][2]=new Point(2,3);
			bugFixMap[4][3]=new Point(3,3);
			bugFixMap[4][4]=new Point(5,4);
			bugFixMap[4][5]=new Point(6,4);
			bugFixMap[4][6]=new Point(7,4);
			bugFixMap[4][7]=new Point(8,4);

			bugFixMap[5][0]=new Point(0,4);
			bugFixMap[5][1]=new Point(1,4);
			bugFixMap[5][2]=new Point(2,4);
			bugFixMap[5][3]=new Point(3,4);
			bugFixMap[5][4]=new Point(4,4);
			bugFixMap[5][5]=new Point(6,5);
			bugFixMap[5][6]=new Point(7,5);
			bugFixMap[5][7]=new Point(8,5);

			bugFixMap[6][0]=new Point(0,5);
			bugFixMap[6][1]=new Point(1,5);
			bugFixMap[6][2]=new Point(2,5);
			bugFixMap[6][3]=new Point(3,5);
			bugFixMap[6][4]=new Point(4,5);
			bugFixMap[6][5]=new Point(5,5);
			bugFixMap[6][6]=new Point(7,6);
			bugFixMap[6][7]=new Point(8,6);

			bugFixMap[7][0]=new Point(0,6);
			bugFixMap[7][1]=new Point(1,6);
			bugFixMap[7][2]=new Point(2,6);
			bugFixMap[7][3]=new Point(3,6);
			bugFixMap[7][4]=new Point(4,6);
			bugFixMap[7][5]=new Point(5,6);
			bugFixMap[7][6]=new Point(6,6);
			bugFixMap[7][7]=new Point(8,7);

			bugFixMap[8][0]=new Point(0,7);
			bugFixMap[8][1]=new Point(1,7);
			bugFixMap[8][2]=new Point(2,7);
			bugFixMap[8][3]=new Point(3,7);
			bugFixMap[8][4]=new Point(4,7);
			bugFixMap[8][5]=new Point(5,7);
			bugFixMap[8][6]=new Point(6,7);
			bugFixMap[8][7]=new Point(7,7);


		}
		
		 
		
	}
	
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
	 * An enum for the two matrices of this IS31FL3731 device
	 * Matrix A corresponds to columns below 7 and B to columns above
	 * @author sydxrey
	 */
	public static enum Matrix {
		A, B;
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
		PWM_REG_BASE_ADDR(0x24); // there are 144=2x72 such registers, one for each LED (from left to right)

	
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
	
	public static void main(String[] args) throws IOException, UnsupportedBusNumberException, InterruptedException {
		
		//testFunctionRegister();
		//new TestDevice();
		testBasicHeadless();
		//testBugFix();
		//testCalib();
	}
	
	private static void testCalib() throws IOException, UnsupportedBusNumberException, InterruptedException {

		Scanner in = new Scanner(System.in);
		LEDCoordinate ledCoord = new LEDCoordinate(0, 0, Matrix.B);
		LEDCoordinate ledCoord2 = new LEDCoordinate(0, 2, Matrix.B);

		IS31FL3731 device = new IS31FL3731();
		device.switchLED(ledCoord, true);
		device.switchLED(ledCoord2, true);
		device.setLEDpwm(ledCoord, 200);

		/*
		 * while (true) { int pwm = in.nextInt(); device.setLEDpwm(ledCoord, pwm); }
		 */
		int pwm = 255;
		while (true) {
			for (int i = 0; i < 16; i++) {
				in.nextLine();
				device.setLEDpwmGammaCorrected16(ledCoord, i);
				device.setLEDpwm(ledCoord2, pwm);
				pwm = 255 - pwm;
				System.out.println(i);
				// Thread.sleep(400);
			}
		}
	}
	
	
	private static void testBugFix() throws IOException, UnsupportedBusNumberException, InterruptedException {

		Scanner in = new Scanner(System.in);
		int row = 4;
		int col = 2;
		LEDCoordinate ledA = new LEDCoordinate(row, col, Matrix.A);
		LEDCoordinate ledB = new LEDCoordinate(row, col, Matrix.B);
		System.out.println("Switching " + ledA + " and " + ledB);

		IS31FL3731 device = new IS31FL3731();
		device.switchLED(ledA, true);
		device.switchLED(ledB, true);
		device.setLEDpwm(ledA, 250);
		device.setLEDpwm(ledB, 250);
	}
	
	
	private static void testBasicHeadless() throws IOException, UnsupportedBusNumberException, InterruptedException {
		
		IS31FL3731 device = new IS31FL3731();

		while (true) {
			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					LEDCoordinate ledA = new LEDCoordinate(row, col, Matrix.A);
					LEDCoordinate ledB = new LEDCoordinate(row, col, Matrix.B);
					device.switchLED(ledA, true);
					device.switchLED(ledB, true);
					// System.out.print(led);
					for (int pwm = 0; pwm < 16; pwm++) {
						device.setLEDpwmGammaCorrected16(ledA, pwm);
						device.setLEDpwmGammaCorrected16(ledB, pwm);
						// System.out.print(".");
						Thread.sleep(5);
					}
					for (int pwm = 15; pwm >= 0; pwm--) {
						device.setLEDpwmGammaCorrected16(ledA, pwm);
						device.setLEDpwmGammaCorrected16(ledB, pwm);
						// System.out.print(".");
						Thread.sleep(5);
					}
				}
			}
		}
	}

	// testing function register
	private static class TestDevice extends JFrame {
		
		IS31FL3731 device;
		int currentRow, currentCol, currentPwm;
		Matrix currentMatrix = Matrix.B;
		
		JSpinner jsLine, jsCol; // jsMatrix;
		JSlider pwmSlider;
		JButton butOpen, butSend;
		
		TestDevice(){
		
			super("test du IS31FL3731 et de la carte du frontpane");
			super.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
			JPanel pane = new JPanel();
			pane.setLayout(new GridLayout(5,2));

//			pane.add(new JLabel("matrix :"));		
//			jsMatrix = new JSpinner();
//			pane.add(jsMatrix);
//			jsMatrix.addChangeListener(e -> selectMatrix());

			pane.add(new JLabel("line :"));
			jsLine = new JSpinner();
			pane.add(jsLine);
			jsLine.addChangeListener(e -> selectLine());
		
			pane.add(new JLabel("col :"));		
			jsCol = new JSpinner();
			pane.add(jsCol);
			jsCol.addChangeListener(e -> selectCol());
		
			pane.add(new JLabel("pwm :"));		
			pwmSlider = new JSlider(0, 255);
			pwmSlider.setPaintTicks(true);
			pwmSlider.setPaintLabels(true);
			pane.add(pwmSlider);
			pwmSlider.addChangeListener(e -> selectPwm());
		
			butOpen = new JButton("test");
			pane.add(butOpen);
			butOpen.addActionListener(e -> testLEDs());
		
			butSend = new JButton("send to device");
			pane.add(butSend);
			butSend.addActionListener(e -> sendToDevice());

			setContentPane(pane);
			//pack();
			setSize(800,300);
			setVisible(true);
			
			openDevice();
		
		}
		
		private void testDevice()  {
			try {
			for (int row=0; row<8; row++) {
				device.switchLEDRow(row, currentMatrix, 0xFF);
				for (int col=0; col<8; col++) {
					for (int pwm=0; pwm < 256; pwm+=16) {
						device.setLEDpwm(new LEDCoordinate(row, col, currentMatrix), pwm);
						System.out.print(".");
						Thread.sleep(1);
					}
					for (int pwm=240; pwm >= 0; pwm-=16) {
						device.setLEDpwm(new LEDCoordinate(row, col, currentMatrix), pwm);
						System.out.print(".");
						Thread.sleep(1);
					}
				}
			}
			} catch (Exception e) { e.printStackTrace();}
		}

		private void sendToDevice() {
			System.out.println("Sending data to device : row=" + currentRow + ", col=" + currentCol + ", matrix="+currentMatrix + ", pwm=" + currentPwm);
			try {
				device.setLEDpwm(new LEDCoordinate(currentRow, currentCol, currentMatrix), currentPwm);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}

		private void selectPwm() {
			currentPwm = (Integer)pwmSlider.getValue();
			if (currentPwm > 255) currentPwm = 255;
			else if (currentCol < 0) currentPwm = 0;
			sendToDevice();
		}

		private void selectCol() {
			currentCol = (Integer)jsCol.getValue();
			if (currentCol > 7) currentCol = 7;
			else if (currentCol < 0) currentCol = 0;
		}

		private void selectLine() {
			currentRow = (Integer)jsLine.getValue();
			if (currentRow > 8) currentRow = 8;
			else if (currentRow < 0) currentRow = 0;
		}

		/*private void selectMatrix() {
			currentMatrix = (Integer)jsMatrix.getValue() == 0 ? Matrix.A : Matrix.B;
		}*/

		private void openDevice() {
			try {
				device = new IS31FL3731();
			} catch (IOException | UnsupportedBusNumberException e) {
				LOGGER.severe("Error instanciating IS31FL3731 device");
				e.printStackTrace();
				System.exit(0);
			}
		}
		
		private void testLEDs() {
			try {
			for (int row=0; row<9; row++) {
				// all LEDs off:
					device.switchLEDRow(row, currentMatrix, 0xFF);
				for (int col=0; col<8; col++) {
					device.setLEDpwm(new LEDCoordinate(row, col, currentMatrix), 255);
					Thread.sleep(20);
				}
			}			
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
		}
	}
	

}
