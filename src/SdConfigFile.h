/* * * * * * * * * * * * * * * * * * * * * * *
 * SD Card Configuration File - Header
 *
 * @brief     Read and write parameters from a config file
 * @file      SdConfigFile.hpp
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
 * Copyright (c) 2022 Simon Bluett
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

#ifndef SD_CONFIG_FILE_HPP
#define SD_CONFIG_FILE_HPP


#include <SdFat.h>


// The number of digits shown after the decimal
// point of float values when writing them to
// the configuration file can be set here
#ifndef FLOAT_DECIMAL_LENGTH
#define FLOAT_DECIMAL_LENGTH  (4)
#endif /* FLOAT_DECIMAL_LENGTH */


// If lines containing parameter values in the
// config file are longer than 40 characters, the
// length of the buffer can be changed here
#ifndef SDCONFIG_BUFFER_LENGTH
#define SDCONFIG_BUFFER_LENGTH (40)
#endif /* SDCONFIG_BUFFER_LENGTH */


/**
 * @class  SdConfigFile
 * @brief  Sd Card Configuration file reading and writing class
 */
class SdConfigFile {

public:
	// Constructor and destructor
	SdConfigFile(uint8_t chipSelectPin);
	~SdConfigFile() {}

	// Configuration parameter reading methods
	bool read(const char* fileName, void (*callbackFunction)());
	bool read(const char* fileName);
	bool get(const char *itemName, int &itemValue);
	bool get(const char *itemName, float &itemValue);
	bool get(const char *itemName, long &itemValue);
	bool get(const char *itemName, bool &itemValue);
	bool get(const char *itemName, char *itemValue, int maxLength);

	// Configuration parameter writing methods
	bool write(const char* fileName, void (*callbackFunction)());
	bool write(const char* fileName);
	bool set(const char *itemName, int itemValue);
	bool set(const char *itemName, float itemValue, int precision = FLOAT_DECIMAL_LENGTH);
	bool set(const char *itemName, long itemValue);
	bool set(const char *itemName, bool itemValue);
	bool set(const char *itemName, char *itemValue);
	bool remove(const char *itemName);

#ifdef ARDUINO
	// Arduino-specific read methods
	bool read(String fileName) { return read(fileName.c_str()); }
	bool read(String fileName, void (*callbackFunction)()) { return read(fileName.c_str(), callbackFunction); }
	bool get(const char *itemName, String &itemValue);

	// Arduino specific write methods
	bool write(String fileName, void (*callbackFunction)()) { return write(fileName.c_str(), callbackFunction); }
	bool write(String fileName) { return write(fileName.c_str()); }
	bool set(const char *itemName, String &itemValue);
#endif /* ARDUINO */
	
private:
	// Internal utility methods
	bool checkItemName(const char *itemName);
	bool discardChar(char currentChar);

	// Sd card file opening, reading and writing methods
	bool openConfigFile(const char* fileName);
	bool openTempFile();
	bool readConfigLine();
	void printLineToFile();

	// Choose the SD file system type depending
	// on which definitions user has supplied
#if defined(SD_CONFIG_FILE_USE_EXFAT)
	SdExFat sd;
	ExFile origFile;
	ExFile tempFile;
#elif defined(SD_CONFIG_FILE_USE_FSFAT)
	SdFs sd;
	FsFile origFile;
	FsFile tempFile;
#else /* SD_CONFIG_FILE_USE_FAT32 */
	SdFat32 sd;
	File32 origFile;
	File32 tempFile;
#endif

	// Buffer used to store each line from the config file
	char lineBuffer[SDCONFIG_BUFFER_LENGTH];

	// State machine variables
	char *currentPos;
	bool lineOverflow;
	bool commentActive;
	bool writeAppend;
	bool equalsSplit;
	bool paramFound;

	// Sd card SPI chip select pin
	const uint8_t chipSelect;
};


#endif /* SD_CONFIG_FILE_HPP */


///////////////////////////////////////////////////////////////
//////////////// END OF FILE: SdConfigFile.hpp ////////////////
///////////////////////////////////////////////////////////////