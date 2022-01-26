/* * * * * * * * * * * * * * * * * * * * * * *
 * SD Card Configuration File Example Sketch
 *
 * @brief     Read and write parameters from a config file
 * @file      sd-config-file.ino
 * @author    Simon Bluett
 * @website   https://wired.chillibasket.com/
 *
 * @version   0.1.0
 * @date      25th January 2022
 * @copyright Copyright (C) 2022, MIT License
 *
 * @note      This example sketch uses the following libraries:
 *            [SdConfigFile](https://github.com/chillibasket/)
 *            [SdFat](https://github.com/greiman/SdFat)
 *
 * * * * * * * * * * * * * * * * * * * * * * */


/**
 * By default, the SdConfigFile library supports FAT16 and FAT32
 * SD card file system types. However, EXFAT support can also be
 * enabled by using one of the two "#defines" below. Note that
 * support for the EXFAT file system takes up more memory.
 *
 * SD_CONFIG_FILE_USE_EXFAT: support only EXFAT SD Cards
 * SD_CONFIG_USE_FSFAT: support both FAT16/32 and EXFAT SD Cards
 * default: FAT16 and FAT32 support only
 * 
 * Uncomment the line below depending on your requirements:
 */
//#define SD_CONFIG_FILE_USE_EXFAT
//#define SD_CONFIG_USE_FSFAT


/**
 * Include the Sd config file library
 */
#include <SdConfigFile.h>


/**
 * Instantiate Sd config file object, passing
 * the chip select pin number as a variable
 */
SdConfigFile configFile(BUILTIN_SDCARD);


// Define some variables which we will set using the 
// values from the SD card configuration file
int intValue = 0;
long longValue = 0;
float floatValue = 0;
bool boolValue1 = false;
bool boolValue2 = false;
String arduinoStringValue = "";
char cStringValue[20] = { 0 };


/**
 * Setup function
 */
void setup() {

	// Start serial comms and wait for user to open the serial monitor
	Serial.begin(115200);
	while(!Serial);
	delay(1000);


	/**
	 * Reading method 1 - using a while loop
	 */
	Serial.println(F("--- Read Method 1 - While Loop ---"));

	// Use the "read" method and supply the directory and file name
	while (configFile.read("file.txt"))
	{
		// Each parameter can be retreived using the "get" method
		if (configFile.get("IntValue", intValue)) continue;
		if (configFile.get("LongValue", longValue)) continue;
		if (configFile.get("FloatValue", floatValue)) continue;
		if (configFile.get("BoolValue1", boolValue1)) continue;
		if (configFile.get("BoolValue2", boolValue2)) continue;
		if (configFile.get("arduinoString", arduinoStringValue)) continue;
		if (configFile.get("cStringValue", cStringValue, 20)) continue;
	}


	// Print current variable values to the serial monitor
	outputParameterValues();

	// Change some of the parameters
	intValue++;
	longValue--;
	floatValue /= 3.5;
	boolValue1 = !boolValue1;
	boolValue2 = !boolValue2;
	arduinoStringValue += " new";
	cStringValue[0] = '#';


	/**
	 * Writing method 1 - using a while loop
	 */
	Serial.println(F("--- Write Method 1 - While Loop ---"));

	// Use the "write" method and supply the directory and file name
	while (configFile.write("file.txt"))
	{
		// Each parameter can be saved using the "set" method
		// Note that when saving the parameter, the existing 
		// parameter entry is removed from the file and the new
		// value is added to the bottom of the file
		if (configFile.set("IntValue", intValue)) continue;
		if (configFile.set("LongValue", longValue)) continue;
		if (configFile.set("FloatValue", floatValue)) continue;
		if (configFile.set("BoolValue1", boolValue1)) continue;
		if (configFile.set("BoolValue2", boolValue2)) continue;
		if (configFile.set("arduinoString", arduinoStringValue)) continue;
		if (configFile.set("cStringValue", cStringValue)) continue;
	}


	// Print current variable values to the serial monitor
	outputParameterValues();

	// Change some of the parameters
	intValue++;
	longValue--;
	floatValue /= 3.5;
	boolValue1 = !boolValue1;
	boolValue2 = !boolValue2;
	arduinoStringValue += " new";
	cStringValue[0] = '#';


	/**
	 * Read method 2 - using a callback function
	 * The benefit of this method is that the function returns
	 * a True/False value depending on if the read was successful
	 */
	Serial.println(F("--- Read Method 2 - Callback ---"));
	if (configFile.read("configfile.txt", readConfigFunction))
	{
		Serial.println(F("Success: config file parameters read"));
	}
	else
	{
		Serial.println(F("Error: unable to process config file"));
	}
	

	// Print current variable values to the serial monitor
	outputParameterValues();

	// Change some of the parameters
	intValue++;
	longValue--;
	floatValue /= 3.5;
	boolValue1 = !boolValue1;
	boolValue2 = !boolValue2;
	arduinoStringValue += " new";
	cStringValue[0] = '#';

}


/**
 * Main Program Loop
 */
void loop() {
	// Empty!
}


/**
 * Read Method 2 - Callback function
 * 
 * This callback function is used to update the 
 * variableseach time that the "read" method finds 
 * anotherparameter value in the configuration file
 */
void readConfigFunction() {
	if (configFile.get("IntValue", intValue)) return;
	if (configFile.get("LongValue", longValue)) return;
	if (configFile.get("FloatValue", floatValue)) return;
	if (configFile.get("BoolValue1", boolValue1)) return;
	if (configFile.get("BoolValue2", boolValue2)) return;
	if (configFile.get("arduinoString", arduinoStringValue)) return;
	if (configFile.get("cStringValue", cStringValue, 20)) return;
}



/**
 * Output the current variables values to the serial monitor
 */
void outputParameterValues() {
	Serial.print("IntValue: "); Serial.println(intValue);
	Serial.print("LongValue: "); Serial.println(longValue);
	Serial.print("FloatValue: "); Serial.println(floatValue, 5);
	Serial.print("boolValue1: "); Serial.println(boolValue1);
	Serial.print("boolValue2: "); Serial.println(boolValue2);
	Serial.print("arduinoString: "); Serial.println(arduinoStringValue);
	Serial.print("cStringValue: "); Serial.println(cStringValue);
}