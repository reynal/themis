/*
 * stlink_dma.c
 *
 *  Created on: Mar 28, 2020
 *      Author: sydxrey
 *
 *
 *      printf support based on STLink UART TX with DMA transfer (DMA1, channel 7, hardcoded for STM32L4)
 */

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stlink_tx_dma.h>
#include <string.h>


/* External variables --------------------------------------------------------*/

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;


/* Function prototypes -----------------------------------------------*/

static size_t stlink_dma_buff_write(const char* data, size_t btw); // TODO make static
static size_t stlink_dma_buff_get_writable_size();
static int stlink_dma_buff_get_readable_size();
static uint8_t * stlink_dma_buff_get_linear_block_read_address();
static size_t stlink_dma_buff_get_linear_block_read_length();
static int stlink_dma_buff_skip(size_t len);
static void stlink_dma_transmit();

/* RING BUFFER for transmit data:

 buffer has size TX_BUFF_SIZE
 Reading data starts at tx_buff_r
 Writing data starts at tx_buff_w
 When tx_buff_w reaches TX_BUFF_SIZE-1, it gets wrapped up to the beginning of the buffer
 Buffer is considered empty when r == w and full when w == r - 1
 As a rule of thumb, if w < r, w can never reach r.
 */
#define TX_BUFF_SIZE 1024 // buffer size
static uint8_t tx_buff[TX_BUFF_SIZE]; 	// buffer data
static size_t tx_buff_r;			// Next read pointer.
static size_t tx_buff_w;			// Next write pointer.

static size_t tx_len; // variable to be shared among IRQ handler and dma_transmit()

/* User code -----------------------------------------------*/

/**
 *
 */
void stlink_Tx_dma_init(){

	/* TODO L432 vers F446 :

	// init buffer:
	memset(tx_buff, 0x00, TX_BUFF_SIZE);

	__HAL_DMA_DISABLE(hdma_STlink_tx);

	// clear all interrupt flags for channel 7:
	hdma_STlink_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF7;

	// program DMA transfer features: (so far only CPAR is set, CMAR and CNDTR will be set later on)
	hdma_STlink_tx->Instance->CPAR = (uint32_t)&(huart_STlink->Instance->TDR); // set peripheral address to UART TDR register

	// enable only TC (transfer complete) and TE (transfer error) interrupts for DMA channel:
	__HAL_DMA_DISABLE_IT(hdma_STlink_tx, DMA_IT_HT);
	__HAL_DMA_ENABLE_IT(hdma_STlink_tx, (DMA_IT_TC | DMA_IT_TE));

	// make sure TC interrupt for UART ain't pending:
	__HAL_UART_CLEAR_FLAG(huart_STlink, UART_CLEAR_TCF);

	// Enable DMA-to-UART transmit request by setting the DMAT bit in the UART CR3 register:
	SET_BIT(huart_STlink->Instance->CR3, USART_CR3_DMAT);

	// now to start the real transfer we'd just need to enable the DMA peripheral (see stlink_dma_transmit() below)

	*/
}



/**
 * Send debug string over UART
 * @param str: String to send
 */
void stlink_dma_send_string(const char* str) {

    size_t len = strlen(str);
    size_t w = stlink_dma_buff_get_writable_size();
    if (len > w) len = w; // write only part of str that fits into w

    stlink_dma_buff_write(str, len);
    stlink_dma_transmit();
}

/**
 * Checks for data in buffer and starts transfer if not in progress
 */
static void stlink_dma_transmit() {

	/* TODO L432 vers F446 :

    // If transfer is not on-going, ie EN=0 in CCR register
    if ((hdma_STlink_tx->Instance->CCR &  DMA_CCR_EN) == 0U){



        tx_len = stlink_dma_buff_get_linear_block_read_length();

        // Limit maximal size to transmit at a time // SR: why ?
        //if (tx_len > 32) tx_len = 32;

        // Anything to transmit?
        if (tx_len > 0) {
        	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
            uint8_t* ptr = stlink_dma_buff_get_linear_block_read_address();

            // Configure DMA
        	hdma_STlink_tx->Instance->CNDTR = tx_len;
        	hdma_STlink_tx->Instance->CMAR = (uint32_t)ptr;

        	//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

            // Start transfer
        	__HAL_DMA_ENABLE(hdma_STlink_tx);
        	// from now on, control will be handed over to stlink_dma_irq_handler() which will in turn
        	// make use of tx_len to move the tx_buff_r pointer

        }


    }

	 */
}

/**
 * Transfer Complete interrupt handler
 */
void stlink_dma_irq_handler(){

	/* TODO L432 vers F446 :

	uint32_t flag_it = hdma_STlink_tx->DmaBaseAddress->ISR;
	//__HAL_DMA_DISABLE_IT(huartSTlink_dmatx, DMA_IT_TE | DMA_IT_TC | DMA_IT_HT);

	// if TC interrupt:

	// Clear interrupt flags
	if ((flag_it & DMA_FLAG_TC7) != 0U)
		hdma_STlink_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF7; // DMA_ISR_TCIF7;

	// stop DMA request from UART
    //CLEAR_BIT(huartSTlink->Instance->CR3, USART_CR3_DMAT);
    //SET_BIT(huartSTlink->Instance->CR1, USART_CR1_TCIE); useful ?

    // else TE interrupt:
	//else if (flag_it & DMA_FLAG_TE7 != 0U)...
	//hdma_uartSTlink_tx->DmaBaseAddress->IFCR = DMA_ISR_GIF7 // clear global flag

	__HAL_DMA_DISABLE(hdma_STlink_tx); // disable DMA so as to be able to reprogramm next transfer in stlink_dma_transmit()

    stlink_dma_buff_skip(tx_len);// Data sent, ignore these

    stlink_dma_transmit();                   // Try to send more data

    */


}


// ---------------------------- ring buffer management -------------------------------------


/**
 * Skip (ignore; advance read pointer) buffer data
 * Marks data as read in the buffer and increases free memory for up to `len` bytes
 * Useful at the end of streaming transfer such as DMA
 * @param 	len: Number of bytes to skip and mark as read
 * @return  Number of bytes skipped
 */
static int stlink_dma_buff_skip(size_t len) {

    int full = stlink_dma_buff_get_readable_size();       /* Get buffer used length */
    tx_buff_r += BUF_MIN(len, full);              /* Advance read pointer */
    if (tx_buff_r >= TX_BUFF_SIZE) {                /* Subtract possible overflow */
        tx_buff_r -= TX_BUFF_SIZE;
    }
    return len;
}

/**
 * Get number of bytes in tx_buff_data available to read
 */
static int stlink_dma_buff_get_readable_size() {

    size_t w, r, size;

    /* Use temporary values in case they are changed during operations */
    w = tx_buff_w;
    r = tx_buff_r;
    if (w == r) {
        size = 0;
    } else if (w > r) {
        size = w - r;
    } else {
        size = TX_BUFF_SIZE - (r - w);
    }
    return size;
}


/**
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Buffer handle
 * \return          Linear buffer start address
 */
static uint8_t* stlink_dma_buff_get_linear_block_read_address() {

    return tx_buff + tx_buff_r;
}

/**
 * \brief           Get length of linear block address before it overflows for read operation
 * \param[in]       buff: Buffer handle
 * \return          Linear buffer size in units of bytes for read operation
 */
static size_t stlink_dma_buff_get_linear_block_read_length() {

    size_t w, r, len;

    /* Use temporary values in case they are changed during operations */
    w = tx_buff_w;
    r = tx_buff_r;
    if (w > r) {
        len = w - r;
    } else if (r > w) {
        len = TX_BUFF_SIZE - r;
    } else {
        len = 0;
    }
    return len;
}

/**
 * Write data to buffer. Copies data from `data` array to buffer and marks buffer as full for maximum `count` number of bytes
 * @param 	data: Pointer to data to write into buffer
 * @param 	btw: Number of bytes to write
 * @return 	Number of bytes written to buffer.
 *          When returned value is less than `btw`, there was not enough memory available to copy full data array
 */
static size_t stlink_dma_buff_write(const char* data, size_t btw) {

    size_t n;

    /* Calculate maximum number of bytes available to write */
    n = stlink_dma_buff_get_writable_size();
    btw = BUF_MIN(n, btw); // write at most "free" bytes
    if (btw == 0) {
        return 0;
    }

    /* Step 1: Write data to linear part of buffer */
    n = BUF_MIN(TX_BUFF_SIZE - tx_buff_w, btw);
    memcpy(tx_buff + tx_buff_w, data, n); // dest, src, size
    tx_buff_w += n;
    btw -= n;

    /* Step 2: Write data to beginning of buffer (overflow part) */
    if (btw > 0) {
        memcpy(tx_buff, data+n, btw);
        tx_buff_w = btw;
    }

    if (tx_buff_w >= TX_BUFF_SIZE) {
    	tx_buff_w = 0;
    }
    return n + btw;
}

// our own implementation of putchar used by printf, so that printf is forwarded
// to the Virtual Com Port (need Hyperterminal or a dedicated terminal on the PC or Mac host station)
/*int __io_putchar(int ch) {


	// check if at least one byte is available. If not, trigger DMA to empty buffer
    //size_t w = tx_buff_w;
    //size_t r = tx_buff_r;
    //if (w == r-1 || (r==0 && w==TX_BUFF_SIZE-1)) stlink_dma_transmit();

	tx_buff[tx_buff_w++] = ch;
	if (tx_buff_w >= TX_BUFF_SIZE) tx_buff_w = 0;

	if (ch =='\n') stlink_dma_transmit(); // transmit DMA at EOL

	return ch;
}*/

// traditionnal blocking call implementation
int __io_putchar(int ch) {
	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 0xFFFF); // blocking call
	return ch;
}

/**
 * Get number of bytes in buffer available to write
 *
 * @return Number of free bytes in tx_buff_data ; 0 if w==r-1
 */
static size_t stlink_dma_buff_get_writable_size() {

    size_t size, w, r;

    /* Use temporary values in case they are changed during operations */
    w = tx_buff_w;
    r = tx_buff_r;
    if (w == r) { // empty buffer
        size = TX_BUFF_SIZE - 1;
    } else if (r > w) {
        size = r - w - 1; // if w == r-1, buffer is full (writable size = 0)
    } else { // w > r
        size = TX_BUFF_SIZE - w + r -1;
    }

    return size;
}

// -------- testbed -----------

/**
 *
 */
void stlink_dma_test(){

	//char sprintf_buff[256];

	stlink_Tx_dma_init();
	int i=0;

	while (1) {
	        /* Send test data */

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);

			printf("pour avoir une autre approche %d\n", i++); // 70us

			//sprintf(sprintf_buff, "pour avoir une autre approche %d\n", i++); // 40us
			//stlink_dma_send_string(sprintf_buff);

			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

	        HAL_Delay(100);

	        //cpu_usage = osGetCPUUsage();            /* Get CPU load */
	        //sprintf(sprintf_buff, "CPU Load: %d%%\r\n", (int)cpu_usage);
	        //stlink_dma_send_string(buff);
	}

}

/*void stlink_dma_buffer_test(){

	stlink_dma_buff_write("hello", 5);
	stlink_dma_buff_skip(5);
	size_t n;
	n = stlink_dma_buff_get_writable_size();
	n = stlink_dma_buff_get_readable_size();
	n = stlink_dma_buff_get_linear_block_read_length();
	uint8_t* ptr = stlink_dma_buff_get_linear_block_read_address();
	stlink_dma_buff_write("gutentag", 8);
	n = stlink_dma_buff_get_writable_size();
	n = stlink_dma_buff_get_readable_size();
	n = stlink_dma_buff_get_linear_block_read_length();
	ptr = stlink_dma_buff_get_linear_block_read_address();
	stlink_dma_buff_skip(n);
	n = stlink_dma_buff_get_linear_block_read_length();
	ptr = stlink_dma_buff_get_linear_block_read_address();
}*/
