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

If you have any questions or suggestion, feel free to open a new GitHub "Issue". If you come across any issues, make sure to specify the hardware you are using and provide examples of the code where the error occurs.
<br />
<br />


## Installation
1. In GitHub, click on the `Code > Download Zip` button.
2. In the Arduino IDE, click on `Sketch > Include Library > Add .ZIP Library...` and select the downloaded zip file.
3. The library should now be installed. To open the example sketch, click on `File > Examples > SdConfigFile > ReadWrite_ConfigFile`.
<br />
<br />


## Usage

### 1. Configuration File Format
Here is an example configuration file:
```
// Comments are skipped by the library while reading a file 
// A comment line is marked by two forward slashes // at the start of a line
# Alternatively, the hash symbol # can also be used to define a comment
# When writing to a file, the comments will be retained

# Each parameter is defined by the name, followed by an equals sign and the value
# No other data (such as a comment) is allowed on the same line
ParameterName=1234
LongValue=12678
FloatValue=0.24689
arduinoString=Hello, this is a string with spaces
cStringValue=This is also a string

# Boolean values can be capitalised or lowercase, or can be written using a 0 or 1
BoolValue1=True
BoolValue2=0
boolValue3=false

```

The supported data types which can be written to and read from the configuration file are:
* Integer value (int)
* Long Integer value (long)
* Floating point value (float)
* Boolean value (bool)
* Arduino text string (String)
* C-style character array (\*char[])
<br />
<br />


### 2. Reading from a Configuration File
There are two different methods which can be used to read from a configuration file.
1. Using a *While Loop*:
```cpp
// The variables where data will be stored need to be defined first
int inValue = 0;
long longValue = 0;
float floatValue = 0;
bool boolValue1 = false;
String arduinoStringValue = "";
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
String arduinoStringValue = "";
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


### 3. Writing to a Configuration File
It is also possible to write to an existing configuration file to add, remove or change the parameters stored within that configuration file.
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
String arduinoStringValue = "Test Arduino String";
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
String arduinoStringValue = "Test Arduino String";
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
<br />
<br />


## Change Log
* **Version 0.1.1** [27th February 2022]
    1. Added support for file names using the Arduino `String` variable name.
    2. Added option to create a new file if no configuration file with the specified name exists yet.
    3. Fixed a bug which occurred when writing to a file using the callback function.
* **Version 0.1.0** [26th January 2022]
    1. Initial version of the library
