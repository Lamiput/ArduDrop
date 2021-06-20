 /*******************************************************************************
 * Project: ArduDrop - Toolkit for Liquid Art Photographers
 * Copyright (C) 2021 Holger Pasligh
 * 
 * This program incorporates a modified version of "Droplet - Toolkit for Liquid Art Photographers"
 * Copyright (C) 2012 Stefan Brenner
 *
 * This file is part of ArduDrop.
 *
 * ArduDrop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ArduDrop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Droplet. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/


#include <Arduino.h>
#include "ardudrop.h"
#include "logger.h"
#include "command.h"
#include "droplet.h"

//devicemapping for the Uno
char deviceMapping[DEVICE_NUMBERS] = {  0,   1,   2,   3,   4,   5,   6,
                                        7,   8,   9,  10,   11,  12,  13,
                                    };

//devicemapping for the Mega2560 -> no need if you do not want to use so many pins
/*
char deviceMapping[DEVICE_NUMBERS] = {  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
                                       10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
                                       20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
                                       30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
                                       40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
                                    };
*/

/*
 * setup - run once
 */
void setup() {
  // setup pin modes
//  for(int i = 0; i < DEVICE_NUMBERS; i++) {
//    pinMode(deviceMapping[i], OUTPUT);
    // manually set pin to LOW otherwise it is 
    // by default set to HIGH on some boards (i.e. Uno)
//    digitalWrite(deviceMapping[i], LOW);
//  }

  // init serial communication
  Command::Setup();
}

 
/*
 * main loop - do not use long running functions here
 */
void loop() {
  Command::Loop();  
}

