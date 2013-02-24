/**
*@todo get the front sensor working again
*@todo ensure that the rounding is working
*/

#include "sensors.h"
#include "debug_frmwrk.h"
//----------------------------------------------------------------
//constants
const uint16_t DESIRED_DISTANCE = 1500;
const uint16_t VERY_CLOSE = 2700;
const uint16_t NO_WALL = 2500;
const unsigned long frontSensor = 1<<15; //this was 17 on robot 5

//Variables
//int analogSensorPins[] = {23, 24, 25, 30}; // FL, BL, BR, FR
int analogSensorPins[] = {25, 26, 23, 24}; // FL, BL, BR, FR
uint32_t currentReadings[] = {0,0,0,0,0}; // initialised readings
//----------------------------------------------------------------
SensorPair roundingValues(SensorPair sensorValue)
{
  uint16_t tempF = 0;
  uint16_t tempR = 0;  
  tempF = ((sensorValue.FrontSensor + 50) / 100) * 100;
  tempR = ((sensorValue.RearSensor + 50) / 100) * 100;
  sensorValue.RearSensor = tempR;
  sensorValue.FrontSensor = tempF;
  return sensorValue;
}

void pinConfSetup(uint8_t p_Portnum, uint8_t p_Pinnum, uint8_t p_Funcnum, uint8_t p_Pinmode, uint8_t p_OpenDrain)
{
	//Sets up the pin to the spec that has been passed to it
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum   = p_Funcnum;
	PinCfg.OpenDrain = p_OpenDrain;
	PinCfg.Pinmode   = p_Pinmode;
	PinCfg.Portnum   = p_Portnum;		
	PinCfg.Pinnum 	 = p_Pinnum;
	PINSEL_ConfigPin(&PinCfg);
}

SensorPair getLeftSensorValues()
{
	
	struct SensorPair returnValue;
	//assigns the FL and BL to the SensorPair
	returnValue.FrontSensor = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);
	returnValue.RearSensor = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_1);	

  if ((returnValue.FrontSensor > NO_WALL) && (returnValue.RearSensor > NO_WALL))
    // returns the SensorPair
	  return roundingValues(returnValue);	
	else if (returnValue.FrontSensor < NO_WALL)
	  returnValue.FrontSensor = 0;
	else if (returnValue.RearSensor < NO_WALL)
	  returnValue.FrontSensor = 0;	
	return roundingValues(returnValue); 
}

SensorPair getRightSensorValues()
{
	// creates the return value
	struct SensorPair returnValue;
	
	//assigns the FL and BL to the SensorPair
	returnValue.FrontSensor = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_4);
	returnValue.RearSensor = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2);
 
  if ((returnValue.FrontSensor > NO_WALL) && (returnValue.RearSensor > NO_WALL))
    // returns the SensorPair
	  return roundingValues(returnValue);	
	else if (returnValue.FrontSensor < NO_WALL)
	  returnValue.FrontSensor = 0;
	else if (returnValue.RearSensor < NO_WALL)
	  returnValue.FrontSensor = 0;	

	return roundingValues(returnValue);
}
// Test routine

int getFrontSensorValue()
{
/**
 * @todo Get this working as at the moment this doesn't send a value back like it did on robot 5.
 */
  unsigned temp = GPIO_ReadValue(0);
  int temp17 =  (temp >> 16) & 1;	
  currentReadings[4] = temp17 ;
  if (currentReadings[4] > 0)
  	return 1;
  else
    return 0;
}

void initSensors()
{
	// configures the analogue pins
	pinConfSetup(PINSEL_PORT_0, analogSensorPins[0], PINSEL_FUNC_1, PINSEL_PINMODE_PULLUP,PINSEL_PINMODE_NORMAL);
	pinConfSetup(PINSEL_PORT_0, analogSensorPins[1], PINSEL_FUNC_1, PINSEL_PINMODE_PULLUP,PINSEL_PINMODE_NORMAL);	
	pinConfSetup(PINSEL_PORT_0, analogSensorPins[2], PINSEL_FUNC_1, PINSEL_PINMODE_PULLUP,PINSEL_PINMODE_NORMAL);	
	pinConfSetup(PINSEL_PORT_0, analogSensorPins[3], PINSEL_FUNC_1, PINSEL_PINMODE_PULLUP,PINSEL_PINMODE_NORMAL); /**@todo check that this intialisation code is correct. I seem to remember that not all are function 1*/	
	// sets the direction of the GPIO pin and clears the value.
	GPIO_SetDir(0, frontSensor, 0); /**@todo check that this initialisation is correct. If it is then get help*/
	// Set up the ADC sampling at 200kHz (maximum rate).
	ADC_Init(LPC_ADC, 200000);
	
	//Allows the channels to communicate
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_1, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_3, ENABLE);

	// Set ADC to continuously sample.
	ADC_StartCmd (LPC_ADC, ADC_START_CONTINUOUS);

	// Set ADC to start converting.
	ADC_BurstCmd (LPC_ADC, ENABLE);
	// Enable interrupts for ADC conversion completing.
	NVIC_EnableIRQ(ADC_IRQn);

  // Enable interrupts globally.
  __enable_irq();
}

void ADC_IRQHandler(void)
{
	// counter made to refresh the readings.

	int counter = 0;
	unsigned temp = GPIO_ReadValue(0);
	// temp 16 should shift the values contained within the buffer 16 bits along until we can read that the 16 bit in the stirng contains a 0 or a 1.
	int temp16 =  (temp >> 15) & 1;	
	// captures the readings at the point the interrupt is called
	uint16_t newReadings[] = {ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0),ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_3),ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_1),ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_2),temp16};

	// refreshes the old readings
	while(counter < 5)
	{
		currentReadings[counter] = newReadings[counter];
		counter = counter+1;
	}
	/*_DBG_("#################");
	_DBG("FL:");	_DBD16(currentReadings[0]); _DBG_("");
	_DBG("BL:");	_DBD16(currentReadings[2]); _DBG_("");
	_DBG("FR:");	_DBD16(currentReadings[1]); _DBG_("");
	_DBG("BR:");	_DBD16(currentReadings[3]); _DBG_("");*/
	  _DBG_("");_DBD(currentReadings[4]);
	  if (currentReadings[4] > 0)
	    _DBG_("I see nothing");	 
	  else  
	  	 _DBG_("Front sensor has sensed something");
}
