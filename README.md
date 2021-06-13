# Summary 

Droplet ist an Open Source Project for the microcontroller-based High-Speed-Photography with special attention to Waterdrop Photography and Liquid Art.

More informations can be found on www.droplet.at


# Serial Protocol
## Introduction
The controller has no physical interface for configuration

Therefore the configuration is send via this serial protocol

All commands end with newline "\n"

All times in milliseconds

## Droplet Message Format
Command          = SetCommand | RunCommand | HighCommand | LowCommand | InfoCommand | ClearCommand | CancelCommand 

<br>

SetCommand       = "S" FieldSeparator DeviceConfig

RunCommand       = "R" FieldSeparator { Passes { FieldSeparator Delay } }

HighCommand      = "H" FieldSeparator DeviceNumber

LowCommand       = "L" FieldSeparator DeviceNumber 

InfoCommand      = "I"

ClearCommand     = "X"

CancelCommand    = "C"

<br>

DeviceConfig     = DeviceNumber FieldSeparator DeviceType FieldSeparator [ Times ] ChksumSeparator Chksum

DeviceNumber     = DigitWithoutZero

DeviceType       = Valve | Flash | Camera

<br>

Valve            = "V"

Flash            = "F"

Camera           = "C"

<br>

Times            = Time { FieldSeparator Time }

Time             = Offset TimeSeparator Duration

<br>

Offset           = "0" | Number

Duration         = "0" | Number

Chksum           = "0" | Number

<br>

Passes           =  "0" | Number

Delay            =  "0" | Number

<br>

FieldSeparator   = ";"

TimeSeperator    = "|"

ChksumSeparator  = "^"

<br>

Number           = DigitWithoutZero { Digit } ;

DigitWithoutZero = "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

Digit            = "0" | DigitWithoutZero ;


## Examples
### Setup
S;1;V;300|50;370|20^740

"Set Tasks for Valve at ArduinoPin 1: Open at 300ms for 50ms and at 370ms for 20ms"


### Run
R

"Run once without delay"

R;1

"Start 1 round"

R;10;5000

"Start 10 rounds with 5 seconds delay"


### Switch High Low
H;1

"send HIGH to device #1"

L;1

"send LOW to device #1"
