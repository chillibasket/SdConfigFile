/**
 * SdConfigFile Library Example Sketch
 * 
 * This example sketch shows how to read parameters from and write new
 * values to an SD Card Configuration File using the SdConfigFile library
 *
 * SdConfigFile <https://github.com/chillibasket/sd-config-file>
 * created:      25th January 2022
 * last updated: 27th February 2022
 * Copyright (C) 2022 by Simon Bluett
 * 
 * Released in the public domain under the MIT license
 *
 * This example sketch requires the SdFat library to be installed:
 * SdFat <https://github.com/greiman/SdFat>
 */


/**
 * By default, the SdConfigFile library supports FAT16 and FAT32
 * SD card file system types. However, EXFAT support can also be
 * enabled by using one of the two "#defines" below. Note that
 * support for the EXFAT file system takes up more memory.
 *
 * SD_CONFIG_FILE_USE_EXFAT: support only EXFAT SD Cards
 * SD_CONFIG_FILE_USE_FSFAT: support both FAT16/32 and EXFAT SD Cards
 * default: FAT16 and FAT32 support only
 * 
 * Uncomment the line below depending on your requirements:
 */
//#define SD_CONFIG_FILE_USE_EXFAT
//#define SD_CONFIG_FILE_USE_FSFAT


/**
 * Include the Sd config file library
 */
#include <SdConfigFile.h>


/**
 * Instantiate Sd config file object, passing
 * the chip select pin number as a variable
 */
SdConfigFile configFile(BUILTIN_SDCARD);


/**
 * Name of the configuration file to open
 */
String configFileName = "test_file.txt";
char configFileName2[] = "test_file2.txt";


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
	while (configFile.read(configFileName))
	{
		// Each parameter can be retrieved using the "get" method
		configFile.get("IntValue", intValue);
		configFile.get("LongValue", longValue);
		configFile.get("FloatValue", floatValue);
		configFile.get("BoolValue1", boolValue1);
		configFile.get("BoolValue2", boolValue2);
		configFile.get("arduinoString", arduinoStringValue);
		configFile.get("cStringValue", cStringValue, 20);
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
	while (configFile.write(configFileName))
	{
		// Each parameter can be saved using the "set" method
		// Note that when saving the parameter, the existing 
		// parameter entry is removed from the file and the new
		// value is added to the bottom of the file
		configFile.set("IntValue", intValue);
		configFile.set("LongValue", longValue);
		configFile.set("FloatValue", floatValue);
		configFile.set("BoolValue1", boolValue1);
		configFile.set("arduinoString", arduinoStringValue);
		configFile.set("cStringValue", cStringValue);

		// New values can also be added to the file using the "set" method
		configFile.set("New Float Value", 12.6745);

		// To remove a parameter from the file, use the "remove" method
		configFile.remove("BoolValue2");
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
	if (configFile.read(configFileName, readConfigFunction))
	{
		Serial.println(F("Success: config file parameters read"));
	}
	else
	{
		Serial.println(F("Error: unable to read from config file"));
	}
	

	// Print current variable values to the serial monitor
	outputParameterValues();

	// Change some of the parameters
	intValue++;
	longValue--;
	floatValue /= 3.5;
	boolValue1 = !boolValue1;
	boolValue2 = !boolValue2;
	arduinoStringValue += " new2";
	cStringValue[0] = '#';


	/**
	 * Write method 2 - using a callback function
	 * The benefit of this method is that the function returns
	 * a True/False value depending on if the read was successful
	 */
	Serial.println(F("--- Write Method 2 - Callback ---"));
	if (configFile.write(configFileName2, writeConfigFunction))
	{
		Serial.println(F("Success: config file parameters write"));
	}
	else
	{
		Serial.println(F("Error: unable to write to config file"));
	}
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
 * variables each time that the "read" method finds 
 * another parameter value in the configuration file
 */
void readConfigFunction() {
	configFile.get("IntValue", intValue);
	configFile.get("LongValue", longValue);
	configFile.get("FloatValue", floatValue);
	configFile.get("BoolValue1", boolValue1);
	configFile.get("BoolValue2", boolValue2);
	configFile.get("arduinoString", arduinoStringValue);
	configFile.get("cStringValue", cStringValue, 20);
}


/**
 * Write Method 2 - Callback function
 * 
 * This callback function is used to update the 
 * variables each time that the "write" method finds 
 * another parameter value in the configuration file
 */
void writeConfigFunction() {
	configFile.set("IntValue", intValue);
	configFile.set("LongValue", longValue);
	configFile.set("FloatValue", floatValue);
	configFile.set("BoolValue1", boolValue1);
	configFile.set("BoolValue2", boolValue2);
	configFile.set("arduinoString", arduinoStringValue);
	configFile.set("cStringValue", cStringValue);
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