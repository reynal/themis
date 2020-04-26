/*
 * vca.c
 *
 *  Created on: Apr 26, 2020
 *      Author: sydxrey
 */



/**
 *  write the given amplitude to the VCA control voltage through the appropriate DAC
 *  @param amplitude must be b/w 0 and 1
 */
void dacVcaWrite(double amplitude){

	amplitude = 1.0 - amplitude; // attenuation by the V2140D VCA is proportional to Control Voltage
	int i = (int)(amplitude * 4095);
	// TODO L4 dacWrite(i, DAC_V2140D_VCA);
}
