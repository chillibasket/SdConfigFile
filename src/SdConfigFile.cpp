/* * * * * * * * * * * * * * * * * * * * * * *
 * SD Card Configuration File
 *
 * @brief     Read and write parameters from a config file
 * @file      SdConfigFile.cpp
 * @author    Simon Bluett
 * @website   https://wired.chillibasket.com/
 *
 * @version   0.1.0
 * @date      25th January 2022
 * @copyright Copyright (C) 2022, MIT License
 *
 * @note      This class uses the SdFat library to perform
 *            all SD card operations; this library needs to 
 *            be installed first: https://github.com/greiman/SdFat
 *
 * * * * * * * * * * * * * * * * * * * * * * */

/**
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "SdConfigFile.h"


/**
 * Constructor
 * 
 * @param[in]  chipSelectPin The SPI chip select pin for the SD card device
 */
SdConfigFile::SdConfigFile(uint8_t chipSelectPin) : chipSelect(chipSelectPin) {
	lineOverflow = false;
	commentActive = false;
	writeAppend = false;
	currentPos = NULL;
}


/**
 * Open the configuration file
 * 
 * @param[in]  fileName  The name and path of the config file to open
 * @return     True if file was opened successfully, false otherwise
 */
bool SdConfigFile::openConfigFile(const char* fileName) {

	// Try connecting to the SD card
	if (!sd.begin(chipSelect)) {
		sd.initErrorHalt(&Serial);
		return false;
	}

	// Check that the file exists
	if (!sd.exists(fileName)) {
		Serial.println(F("Config file not found"));
		return false;
	}

	// If another file is already open, close it
	if (origFile) origFile.close();

	// Try opening the file
	if (!origFile.open(fileName, FILE_READ)) {
		Serial.println(F("Can't open the config file"));
		return false;
	}

	commentActive = false;
	lineOverflow = false;
	return true;
}


/**
 * Open the temporary file
 * 
 * @param[in]  fileName  The name and path of the config file to open
 * @return     True if file was opened successfully, false otherwise
 */
bool SdConfigFile::openTempFile() {

	for (int i = 0; i < 3; i++) {

		// Try connecting to the SD card
		if (!sd.begin(chipSelect)) {
			sd.initErrorHalt(&Serial);
			continue;
		}

		strcpy(lineBuffer, "_temp");
		lineBuffer[5] = i;
		lineBuffer[6] = '\0';

		// Check that the file exists
		if (sd.exists(lineBuffer)) {
			if (!sd.remove(lineBuffer)) continue;
		}

		// Try opening the file
		if (!tempFile.open(lineBuffer, FILE_WRITE)) {
			Serial.println(F("Unable to open temporary file"));
			continue;
		}

		// Open and close file to create it
		tempFile.print("");
		tempFile.close();
		tempFile.open(lineBuffer, FILE_WRITE);

		return true;
	}

	return false;
}


/**
 * Read the configuration file until another configuation parameter is found
 * 
 * @return  True if a configuration parameter is found, false if the end of file is reached
 */
bool SdConfigFile::readConfigLine() {

	if (origFile) {

		while (origFile.available()) {

			printLineToFile();

			// Read in a new line - Note: removes '\r' but leaves '\n'
			int bufferLength = origFile.fgets(lineBuffer, sizeof(lineBuffer));
			lineOverflow = true;
			equalsSplit = false;
			currentPos = lineBuffer;

			// Line needs to be at least three characters in length to be valid
			// (eg. v=1) Lines shorted than this can't contain any useful info
			if (bufferLength > 3) {

				// Check that no line overflow has occurred
				if (lineBuffer[bufferLength - 1] == '\n') {
					lineOverflow = false;
				}

				// If we are still in a comment line, continue skipping
				if (commentActive) {
					if (!lineOverflow) commentActive = false;
					continue;
				}

				// Check if line is commented out
				if ((lineBuffer[0] == '#') || (lineBuffer[0] == '/' && lineBuffer[1] == '/')) {
					if (lineOverflow) commentActive = true;
					continue;
				}

				// Split at equals sign
				currentPos = strtok(lineBuffer, "=");

				// If no equals sign was present, then the string doesn't contain a parameter
				if ((int)strlen(currentPos) == bufferLength && !lineOverflow) {
					continue;
				}

				equalsSplit = true;
				return true;
			}
		}

		printLineToFile();

		// Close the config file
		if (origFile) origFile.close();
	}

	return false;
}


/**
 * Print data to temporary file
 */
void SdConfigFile::printLineToFile() {
	if (tempFile && currentPos != NULL) {
		tempFile.print(currentPos);

		if (equalsSplit) {                  // If the line was already split
			currentPos = strtok(NULL, "="); // Get the value on other side of the equals sign
			checkItemName("");              // Strips spaces, etc. from the value
			tempFile.print("=");            // Add in the equals sign again 
			tempFile.println(currentPos);   // Print the value to the file
		}
	}
}


/**
 * Read configurations from the SD card config file using a callback function
 * 
 * @param[in]  fileName  The name and path of the config file to open
 * @param[in]  callbackFunction  The function to run when there is data available to parse
 * @return     True if the read completed successfully, false otherwise
 */
bool SdConfigFile::read(const char* fileName, void (*callbackFunction)()) {

	if (!openConfigFile(fileName)) return false;

	// While we have data left to read in the file
	while (origFile) {

		if (readConfigLine()) {
			// Raise the callback function
			if (currentPos) callbackFunction();
		}

	}

	return true;
}


/**
 * Read configurations from the SD card config file using a while loop
 * 
 * @param[in]  fileName  The name and path of the config file to open
 * @return     True if there is still data to read, false if finished reading file
 */
bool SdConfigFile::read(const char* fileName) {

	if (!origFile) openConfigFile(fileName);

	while (origFile) {
		if (readConfigLine()) {
			if (currentPos) return true;
		}
	}

	return false;
}


/**
 * Write the new configurations to the SD card config file using a while loop
 * 
 * @param[in]  fileName  The name and path of the config file to write to
 * @return     True if there is still data to write, false if finished updating reading file
 */
bool SdConfigFile::write(const char* fileName) {

	// Open a temporary file were the changes will be saved
	if (!tempFile) {
		if (!openTempFile()) {
			Serial.println(F("Unable to open temporary file"));
			return false;
		}
		writeAppend = false;
		currentPos = NULL;
	}

	if (!writeAppend) {

		// Open up the configuation file
		if (!origFile) {
			if (!openConfigFile(fileName)) {
				Serial.println(F("Unable to open config file"));
				tempFile.close();
				return false;
			}
		}

		// Read the file and write it to the temporary file. Line containing
		// configuration parameters which need to changed are skipped
		while (origFile && tempFile) {
			if (readConfigLine()) {
				if (currentPos) return true;
			}
		}

		// Once the end of the file has been reached, we can append
		// the updated parameters to the end of the temporary file
		writeAppend = true;
		return true;
	}

	// Delete the original configuration file and rename the temporary file
	if (sd.remove(fileName)) {
		if (!tempFile.rename(fileName)) {
			Serial.println(F("Unable to rename temporary file"));
		}
	}
	
	tempFile.close();

	return false;
}



///////////////////////////////////////////////////////////////
//
// Internal utility methods
//
///////////////////////////////////////////////////////////////

/**
 * Check whether a string matches the current config entry name
 * @param[in]  itemName  The string against which to check the current entry
 * @return     True if the names match, false otherwise
 */
bool SdConfigFile::checkItemName(const char *itemName) {

	// Remove spaces from front of character array
	while (currentPos[0] == ' ') currentPos++;
	int stringLength = strlen(currentPos);

	// Remove spaces, tabs and line ending characters from end of character array
	while (stringLength > 0 && discardChar(currentPos[stringLength - 1])) stringLength--;
	currentPos[stringLength] = '\0';

	// Check if both name strings match and that the string isn't empty
	if (strcmp(itemName, currentPos) == 0 && stringLength != 0) {
		currentPos = strtok(NULL, "=");
		if (currentPos) return true;
	}

	return false;
}


/**
 * Check for whitespace, tab or line ending characters
 * @param[in]  currentChar The character to test
 * @return     True if the character matches, false otherwise
 */
bool SdConfigFile::discardChar(char currentChar) {
	if (currentChar == ' ' || currentChar == '\n' || currentChar == '\t') return true;
	return false;
}



///////////////////////////////////////////////////////////////
//
// Overloaded "Get" functions to retreive a variety of variables
// types from the configuration file using the same method name
//
///////////////////////////////////////////////////////////////

/**
 * Get an integer config value
 * @param[in]  itemName  The configuration item name
 * @param[out] itemValue The integer variable where value will be saved
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::get(const char *itemName, int &itemValue) {
	if (checkItemName(itemName)) {
		itemValue = atoi(currentPos);
		return true;
	}
	return false;
}


/**
 * Get a float config value
 * @param[in]  itemName  The configuration item name
 * @param[out] itemValue The float variable where value will be saved
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::get(const char *itemName, float &itemValue) {
	if (checkItemName(itemName)) {
		itemValue = atof(currentPos);
		return true;
	}
	return false;
}


/**
 * Get a long config value
 * @param[in]  itemName  The configuration item name
 * @param[out] itemValue The long variable where value will be saved
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::get(const char *itemName, long &itemValue) {
	if (checkItemName(itemName)) {
		itemValue = atol(currentPos);
		return true;
	}
	return false;
}


/**
 * Get a boolean config value
 * @param[in]  itemName  The configuration item name
 * @param[out] itemValue The boolean variable where value will be saved
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::get(const char *itemName, bool &itemValue) {
	if (checkItemName(itemName)) {
		if (atoi(currentPos) == 1 || strstr(currentPos, "True") || strstr(currentPos, "true")) {
			itemValue = true;
		} else {
			itemValue = false;
		}
		return true;
	}
	return false;
}


/**
 * Get a string config value and save it in a character array
 * @param[in]  itemName  The configuration item name
 * @param[out] itemValue The character array where value will be saved
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::get(const char *itemName, char *itemValue, int maxLength) {
	if (checkItemName(itemName)) {
		// Remove empty spaces from front of character array
		while (currentPos[0] == ' ') currentPos++;
		int stringLength = strlen(currentPos);

		// Remove spaces, tabs and line ending characters from end of character array
		while (stringLength > 0 && discardChar(currentPos[stringLength - 1])) stringLength--;
		if (stringLength >= maxLength) stringLength = maxLength - 1;

		// Copy the string to the destination character array
		strncpy(itemValue, currentPos, stringLength);
		itemValue[stringLength + 1] = '\0';
		return true;
	}
	return false;
}


/**
 * Get a string config value and save it in an Arduino String object
 * @param[in]  itemName  The configuration item name
 * @param[out] itemValue The Arduino String object where value will be saved
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::get(const char *itemName, String &itemValue) {
	if (checkItemName(itemName)) {
		itemValue = currentPos;
		itemValue.trim();
		return true;
	}
	return false;
}



///////////////////////////////////////////////////////////////
//
// Overloaded "Set" functions to store a variety of variable
// types to the configuration file using the same method name
//
///////////////////////////////////////////////////////////////

/**
 * Set an long config value
 * @param[in]  itemName  The configuration item name
 * @param[in]  itemValue The long value to which config parameter will be set
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::set(const char *itemName, long itemValue) {
	if (writeAppend && tempFile) {
		tempFile.print(itemName);
		tempFile.print("=");
		tempFile.println(itemValue);
	} else if (checkItemName(itemName)) {
		currentPos = NULL;
		return true;
	}
	return false;
}


/**
 * Set an integer config value
 * @param[in]  itemName  The configuration item name
 * @param[in]  itemValue The integer value to which config parameter will be set
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::set(const char *itemName, int itemValue) {
	return set(itemName, (long)itemValue);
}


/**
 * Set a boolean config value
 * @param[in]  itemName  The configuration item name
 * @param[in]  itemValue The bool value to which config parameter will be set
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::set(const char *itemName, bool itemValue) {
	return set(itemName, (long)itemValue);
}


/**
 * Set a float config value
 * @param[in]  itemName  The configuration item name
 * @param[in]  itemValue The float value to which config parameter will be set
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::set(const char *itemName, float itemValue) {
	if (writeAppend && tempFile) {
		tempFile.print(itemName);
		tempFile.print("=");
		tempFile.println(itemValue,FLOAT_DECIMAL_LENGTH);
	} else if (checkItemName(itemName)) {
		currentPos = NULL;
		return true;
	}
	return false;
}


/**
 * Set a character array string config value
 * @param[in]  itemName  The configuration item name
 * @param[in]  itemValue The character array to which config parameter will be set
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::set(const char *itemName, char *itemValue) {
	if (writeAppend && tempFile) {
		tempFile.print(itemName);
		tempFile.print("=");
		tempFile.println(itemValue);
	} else if (checkItemName(itemName)) {
		currentPos = NULL;
		return true;
	}
	return false;
}


/**
 * Set a Arduino String config value
 * @param[in]  itemName  The configuration item name
 * @param[in]  itemValue The character array to which config parameter will be set
 * @return     True if configuration was set, false if current item name did not match
 */
bool SdConfigFile::set(const char *itemName, String &itemValue) {
	if (writeAppend && tempFile) {
		tempFile.print(itemName);
		tempFile.print("=");
		tempFile.println(itemValue);
	} else if (checkItemName(itemName)) {
		currentPos = NULL;
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////
//////////////// END OF FILE: SdConfigFile.cpp ////////////////
///////////////////////////////////////////////////////////////