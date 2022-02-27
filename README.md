[![GPLv3 License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/)
[![Issues](https://img.shields.io/github/issues-raw/chillibasket/SdConfigFile.svg?maxAge=25000)](https://github.com/chillibasket/SdConfigFile/issues)
[![GitHub last commit](https://img.shields.io/github/last-commit/chillibasket/SdConfigFile.svg?style=flat)](https://github.com/chillibasket/SdConfigFile/commits/master)

# SD Card Configuration File Library
SdConfigFile is an Arduino library to read and write configuration files on a SD card.
<br />

The library supports the following actions:
1. Reading parameter variables from the configuration file
2. Updating the values of existing parameters in the configuration file
3. Adding new parameters to the file
4. Deleting parameters from the file
5. Creating a new configuration file
<br />

This library is still a work in progress. If you have any questions or suggestion, feel free to open a new "Issue".
<br />
<br />


## Installation
1. In GitHub, click on the `Code > Download Zip` button.
2. In the Arduino IDE, click on `Sketch > Include Library > Add .ZIP Library...` and select the downloaded zip file.
3. The library should now be installed. To open the example sketch, click on `File > Examples > SdConfigFile > ReadWrite_ConfigFile`.
<br />
<br />


## Usage

### Reading from a Configuration File
There are two different methods which can be used to read from a configuration file.
1. Using a *While Loop*:
```cpp
	// The variables where data will be stored need to be defined first
	int inValue = 0;
	long longValue = 0;
	float floatValue = 0;
	bool boolValue1 = false;
	String arduinoString = "";
	char cStringValue[20];

	while (configFile.read("configFileName.txt"))
	{
		// Each parameter can be retrieved using the "get" method
		configFile.get("IntValue", intValue);
		configFile.get("LongValue", longValue);
		configFile.get("FloatValue", floatValue);
		configFile.get("BoolValue1", boolValue1);
		configFile.get("arduinoString", arduinoStringValue);
		configFile.get("cStringValue", cStringValue, 20);
	}
```

2. Using a *Callback Function*:
```cpp
// The variables where data will be stored need to be defined GLOBALLY!
int inValue = 0;
long longValue = 0;
float floatValue = 0;
bool boolValue1 = false;
String arduinoString = "";
char cStringValue[20];

void setup() { 
	Serial.begin(9600);

	// Try reading the config file and check if it was successful
	if (configFile.read("configFileName.txt", readConfigCallback))
	{
		Serial.println("Configuration file read successfully");
	}
	else
	{
		Serial.println("An error occurred while reading the configuration file");
	}
}

// This function will be called multiple times while reading the config file
void readConfigCallback()
{
	// Each parameter can be retrieved using the "get" method
	configFile.get("IntValue", intValue);
	configFile.get("LongValue", longValue);
	configFile.get("FloatValue", floatValue);
	configFile.get("BoolValue1", boolValue1);
	configFile.get("arduinoString", arduinoStringValue);
	configFile.get("cStringValue", cStringValue, 20);
}
```

The *Callback Function* method has the benefit that it returns either `True` or `False` depending on if the configuration file could be read correctly. The *While Loop* method is a bit simpler but doesn't offer a way to check if any errors occurred.
<br />
<br />


### Writing to a Configuration File
It is also possible to write to an existing configuration file to add, remove or change the parameters stored within that file.
* If the configuration file does not already exist on the SD card, a new file with the specified name will be created. Otherwise, the existing file is deleted and an updated configuration file with the same name is created.
* When updating a parameter using the `set` method, the old parameter value is removed and the new parameter value is added to the bottom of the file.
* Parameters which are no longer required can be deleted from the configuration file using the `remove` method.

Similar to the read operation, two different methods can be used to write to a configuration file.
1. Using a *While Loop*:
```cpp
// Define the variables which should be stored
int inValue = 123;
long longValue = 4321;
float floatValue = 12.56;
bool boolValue1 = true;
String arduinoString = "Test Arduino String";
char cStringValue[] = "Test cString";

while (configFile.write("configFileName.txt"))
{
	// Each parameter can saved using the "set" method
	// If the parameter is not present in the file already, it will
	// be added to the end of the file
	configFile.set("IntValue", intValue);
	configFile.set("LongValue", longValue);
	configFile.set("FloatValue", floatValue);
	configFile.set("BoolValue1", boolValue1);
	configFile.set("arduinoString", arduinoStringValue);
	configFile.set("cStringValue", cStringValue);

	// To remove a parameter from the file, use the 'remove' method
	configFile.remove("BoolValue2");
}
```

2. Using a *Callback Function*:
```cpp
// The variables where data will be stored need to be defined GLOBALLY!
int inValue = 123;
long longValue = 4321;
float floatValue = 12.56;
bool boolValue1 = true;
String arduinoString = "Test Arduino String";
char cStringValue[] = "Test cString";

void setup() { 
	Serial.begin(9600);

	// Try writing to the config file and check if it was successful
	if (configFile.write("configFileName.txt", writeConfigCallback))
	{
		Serial.println("Configuration file updated successfully");
	}
	else
	{
		Serial.println("An error occurred while writing to the configuration file");
	}
}

// This function will be called multiple times while updating the config file
void readConfigCallback()
{
	// Each parameter can saved using the "set" method
	// If the parameter is not present in the file already, it will
	// be added to the end of the file
	configFile.set("IntValue", intValue);
	configFile.set("LongValue", longValue);
	configFile.set("FloatValue", floatValue);
	configFile.set("BoolValue1", boolValue1);
	configFile.set("arduinoString", arduinoStringValue);
	configFile.set("cStringValue", cStringValue);

	// To remove a parameter from the file, use the 'remove' method
	configFile.remove("BoolValue2");
}
```

The *Callback Function* method has the benefit that it returns either `True` or `False` depending on if the configuration file could be written correctly. The *While Loop* method is a bit simpler but doesn't offer a way to check if any errors occurred.
<br />
<br />


## Tested devices:
* Teensy 3.6
* (More coming soon)
* Let me know if you have tested it on any other devices