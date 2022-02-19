/*
 * misc.h
 *
 *  Created on: Nov 13, 2020
 *      Author: sydxrey
 */

#ifndef INC_MISC_H_
#define INC_MISC_H_

#define US      * 1
#define MS      * 1000 US

/*typedef enum {
	FALSE,
	TRUE
} Boolean;*/


// binary output formatting:
// usage : printf("Leading text "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c\n"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#endif /* INC_MISC_H_ */
