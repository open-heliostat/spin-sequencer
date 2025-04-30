#include <Arduino.h>

#include <SX126x-Arduino.h>
#include <SPI.h>

// ESP32 - SX126x pin configuration
int PIN_LORA_RESET = 42;  // LORA RESET
int PIN_LORA_DIO_1 = 39; // LORA DIO_1
int PIN_LORA_BUSY = 40;  // LORA SPI BUSY
int PIN_LORA_NSS = 41;	// LORA SPI CS
int PIN_LORA_SCLK = 7;  // LORA SPI CLK
int PIN_LORA_MISO = 8;  // LORA SPI MISO
int PIN_LORA_MOSI = 9;  // LORA SPI MOSI
// int RADIO_TXEN = 26;	 // LORA ANTENNA TX ENABLE
int RADIO_RXEN = 38;	 // LORA ANTENNA RX ENABLE

// Define LoRa parameters
#define RF_FREQUENCY 868000000  // Hz
#define TX_OUTPUT_POWER 22		// dBm
#define LORA_BANDWIDTH 0		// [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR 7 // [SF7..SF12]
#define LORA_CODINGRATE 1		// [1: 4/5, 2: 4/6,  3: 4/7,  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8  // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0   // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false
#define RX_TIMEOUT_VALUE 3000
#define TX_TIMEOUT_VALUE 3000

#define BUFFER_SIZE 64 // Define the payload size here

static RadioEvents_t RadioEvents;
static uint16_t BufferSize = BUFFER_SIZE;
static uint8_t RcvBuffer[BUFFER_SIZE];
static uint8_t TxdBuffer[BUFFER_SIZE];
static bool isMaster = true;
const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";


// Function declarations
void OnTxDone(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnTxTimeout(void);
void OnRxTimeout(void);
void OnRxError(void);
void OnCadDone(bool cadResult);

time_t timeToSend;

time_t cadTime;

uint8_t pingCnt = 0;
uint8_t pongCnt = 0;

hw_config hwConfig;

class LoRaController
{
public:
    LoRaController()
    {
        // Constructor
    }

    void begin()
    {
        // Define the HW configuration between MCU and SX126x
        hwConfig.CHIP_TYPE = SX1262_CHIP;		  // Example uses an eByte E22 module with an SX1262
        hwConfig.PIN_LORA_RESET = PIN_LORA_RESET; // LORA RESET
        hwConfig.PIN_LORA_NSS = PIN_LORA_NSS;	 // LORA SPI CS
        hwConfig.PIN_LORA_SCLK = PIN_LORA_SCLK;   // LORA SPI CLK
        hwConfig.PIN_LORA_MISO = PIN_LORA_MISO;   // LORA SPI MISO
        hwConfig.PIN_LORA_DIO_1 = PIN_LORA_DIO_1; // LORA DIO_1
        hwConfig.PIN_LORA_BUSY = PIN_LORA_BUSY;   // LORA SPI BUSY
        hwConfig.PIN_LORA_MOSI = PIN_LORA_MOSI;   // LORA SPI MOSI
        // hwConfig.RADIO_TXEN = RADIO_TXEN;		  // LORA ANTENNA TX ENABLE
        hwConfig.RADIO_RXEN = RADIO_RXEN;		  // LORA ANTENNA RX ENABLE
        hwConfig.USE_DIO2_ANT_SWITCH = false;	  // Example uses an CircuitRocks Alora RFM1262 which uses DIO2 pins as antenna control
        hwConfig.USE_DIO3_TCXO = false;			  // Example uses an CircuitRocks Alora RFM1262 which uses DIO3 to control oscillator voltage
        hwConfig.USE_DIO3_ANT_SWITCH = false;	 // Only Insight ISP4520 module uses DIO3 as antenna control

        lora_hardware_init(hwConfig);

        // Initialize the Radio callbacks
        RadioEvents.TxDone = OnTxDone;
        RadioEvents.RxDone = OnRxDone;
        RadioEvents.TxTimeout = OnTxTimeout;
        RadioEvents.RxTimeout = OnRxTimeout;
        RadioEvents.RxError = OnRxError;
        RadioEvents.CadDone = OnCadDone;

        // Initialize the Radio
        Radio.Init(&RadioEvents);

        // Set Radio channel
        Radio.SetChannel(RF_FREQUENCY);

        // Set Radio TX configuration
        Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                        LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                        true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

        // Set Radio RX configuration
        Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                        LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                        LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                        0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

        // Start LoRa
        log_i("LoRa", "Starting Radio.Rx");
        Radio.Rx(RX_TIMEOUT_VALUE);

        timeToSend = millis();
    }
};

/**@brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void)
{
	log_i("LoRa", "OnTxDone");
	Radio.Rx(RX_TIMEOUT_VALUE);
}

/**@brief Function to be executed on Radio Rx Done event
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
	log_i("LoRa", "OnRxDone");
	delay(10);
	BufferSize = size;
	memcpy(RcvBuffer, payload, BufferSize);

	log_i("LoRa", "RssiValue=%d dBm, SnrValue=%d\n", rssi, snr);

	for (int idx = 0; idx < size; idx++)
	{
		log_i("LoRa", "%02X ", RcvBuffer[idx]);
	}
	log_i("LoRa", "");

	digitalWrite(LED_BUILTIN, HIGH);

	if (isMaster == true)
	{
		if (BufferSize > 0)
		{
			if (strncmp((const char *)RcvBuffer, (const char *)PongMsg, 4) == 0)
			{
				log_i("LoRa", "Received a PONG in OnRxDone as Master");

				// Wait 500ms before sending the next package
				delay(500);

				// Check if our channel is available for sending
				Radio.Standby();
				Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
				cadTime = millis();
				Radio.StartCad();
				// Sending next Ping will be started when the channel is free
			}
			else if (strncmp((const char *)RcvBuffer, (const char *)PingMsg, 4) == 0)
			{ // A master already exists then become a slave
				log_i("LoRa", "Received a PING in OnRxDone as Master");
				isMaster = false;
				// Check if our channel is available for sending
				Radio.Sleep();
				Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
				cadTime = millis();
				Radio.StartCad();
				// Sending next Pong will be started when the channel is free
			}
			else // valid reception but neither a PING or a PONG message
			{	// Set device as master and start again
				isMaster = true;
				Radio.Rx(RX_TIMEOUT_VALUE);
			}
		}
	}
	else
	{
		if (BufferSize > 0)
		{
			if (strncmp((const char *)RcvBuffer, (const char *)PingMsg, 4) == 0)
			{
				log_i("LoRa", "Received a PING in OnRxDone as Slave");

				// Check if our channel is available for sending
				Radio.Standby();
				Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
				cadTime = millis();
				Radio.StartCad();
				// Sending Pong will be started when the channel is free
			}
			else // valid reception but not a PING as expected
			{	// Set device as master and start again
				log_i("LoRa", "Received something in OnRxDone as Slave");
				isMaster = true;
				Radio.Rx(RX_TIMEOUT_VALUE);
			}
		}
	}
}

/**@brief Function to be executed on Radio Tx Timeout event
 */
void OnTxTimeout(void)
{
	// Radio.Sleep();
	log_i("LoRa", "OnTxTimeout");
	digitalWrite(LED_BUILTIN, LOW);

	Radio.Rx(RX_TIMEOUT_VALUE);
}

/**@brief Function to be executed on Radio Rx Timeout event
 */
void OnRxTimeout(void)
{
	log_i("LoRa", "OnRxTimeout");

	digitalWrite(LED_BUILTIN, LOW);

	if (isMaster == true)
	{
		// Wait 500ms before sending the next package
		delay(500);

		// Check if our channel is available for sending
		Radio.Standby();
		Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
		cadTime = millis();
		Radio.StartCad();
		// Sending the ping will be started when the channel is free
	}
	else
	{
		// No Ping received within timeout, switch to Master
		isMaster = true;
		// Check if our channel is available for sending
		Radio.Standby();
		Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
		cadTime = millis();
		Radio.StartCad();
		// Sending the ping will be started when the channel is free
	}
}

/**@brief Function to be executed on Radio Rx Error event
 */
void OnRxError(void)
{
	log_i("LoRa", "OnRxError");
	digitalWrite(LED_BUILTIN, LOW);

	if (isMaster == true)
	{
		// Wait 500ms before sending the next package
		delay(500);

		// Check if our channel is available for sending
		Radio.Standby();
		Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
		cadTime = millis();
		Radio.StartCad();
		// Sending the ping will be started when the channel is free
	}
	else
	{
		Radio.Rx(RX_TIMEOUT_VALUE);
	}
}

/**@brief Function to be executed on CAD Done event
 */
void OnCadDone(bool cadResult)
{
	time_t duration = millis() - cadTime;
	if (cadResult)
	{
		log_i("LoRa", "CAD returned channel busy after %ldms\n", duration);
		Radio.Rx(RX_TIMEOUT_VALUE);
	}
	else
	{
		log_i("LoRa", "CAD returned channel free after %ldms\n", duration);
		if (isMaster)
		{
			log_i("LoRa", "Sending a PING in OnCadDone as Master");
			// Send the next PING frame
			TxdBuffer[0] = 'P';
			TxdBuffer[1] = 'I';
			TxdBuffer[2] = 'N';
			TxdBuffer[3] = 'G';
		}
		else
		{
			log_i("LoRa", "Sending a PONG in OnCadDone as Slave");
			// Send the reply to the PONG string
			TxdBuffer[0] = 'P';
			TxdBuffer[1] = 'O';
			TxdBuffer[2] = 'N';
			TxdBuffer[3] = 'G';
		}
		// We fill the buffer with numbers for the payload
		for (int i = 4; i < BufferSize; i++)
		{
			TxdBuffer[i] = i - 4;
		}

		Radio.Send(TxdBuffer, BufferSize);
	}
}