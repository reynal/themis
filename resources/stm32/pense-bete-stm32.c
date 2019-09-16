// Aide-mémoire de programmation bas niveau des STM32

// ===================================================
//                       GPIO
// ===================================================

// solution 1 : utiliser les fonctions de haut niveau de HAL
HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
HAL_GPIO_ReadPin(port, pin);

// solution 2 : utiliser le registre ODR
// a) ex. pour mettre PA5 à 1
GPIOA->ODR |=  0x00000020; // le bit à 1 représente la pin GPIO parmi les 8 pins du port A à activer (0010.0000 = 20)
// b) et pour la remettre à 0
GPIOA->ODR &=  ~(0x00000020);  // 0010.0000 = 20

// solution 3 : utiliser le registre BSRR ce qui évite de faire des opérations logiques et de faire des memory write/read
// avec BSRR, tout ce fait en une seule instruction (attention, BSRR = 32 bits wide, les 16 bits de poid fort correspondant
// à la mise à zéro des GPIO, les 16 bits de poids faible, à la mise à un, ce qui donne :

GPIOA->BSRR = (1<<5); // PA5 = 1
GPIOA->BSRR = (1<<(16+5)); // PA5 = 0 cf. explication dans http://hertaville.com/stm32f0-gpio-tutorial-part-1.html

// ===================================================
//                       EXTI
// ===================================================

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  if(GPIO_Pin == GPIO_PIN_0) HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
}


// ===================================================
//                       UART
// ===================================================

uint8_t tx_buff[]={1,5,2,3,4,5,6,7,8,9}; // USART or SPI

HAL_UART_Transmit(&huart1, tx_buff, 10, 1000);
// autre carte :
HAL_UART_Receive(&huart1,data_rx,10,1000);

// ===================================================
//                       SPI
// ===================================================
// Attention : le NSS hardware ne marche pas ! (enfin, il marche, mais pas comme il devrait donc il sert à rien => gérer en soft)
// 1) blocking mode:
SPI_HandleTypeDef hspi;
uint8_t tx_buff[10], rx_buff[10];
HAL_SPI_Transmit(&hspi, tx_buff, 2, 100); // size=2 bytes, timeout=100ms
HAL_SPI_TransmitReceive(&hspi, tx_buff, rx_buff, 10, 100); // 10 octets, timeout=100ms

// 2) non-blocking with DMA:
// activer l'interruption globale dans CubeMX

// ===================================================
//                       TIMER
// ===================================================
// timer driven ISR:
TIM_HandleTypeDef htim;
HAL_TIM_Base_Start_IT(&htim);

// utiliser un timer comme compteur :
count = __HAL_TIM_GetCounter(&htim);    //read TIM2 counter value

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){ // callback
  if (htim == &htim1){ // on check que c'est le bon TIMER
  		counter++;
  		if (counter == WAVE_TABLE_LEN) counter = 0;
  		cna4822write(wave_table[counter], 3, MCP4822_CHANNEL_A);
  }      
}

// ===================================================
//                       DMA mem2mem
// ===================================================
DMA_HandleTypeDef hdma_memtomem_dma2_stream0;

uint8_t Buffer_Src[]={0,1,2,3,4,5,6,7,8,9}; // DMA
uint8_t Buffer_Dest[10]; // DMA
