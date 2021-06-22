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
 * along with ArduDrop. If not, see <http://www.gnu.org/licenses/>.
 *******************************************************************************/

#ifndef __ARDUDROP_H__
#define __ARDUDROP_H__

#define BAUD_RATE         9600
#define MAX_INPUT_SIZE      50 // max length of serial command
#define TIMES_BUFFER_SIZE   40 // max number of tasks within a single command
#define DEVICE_NUMBERS      14 // how many digital pins should be mapped? 
#define MIN_DURATION        10 // default length of tasks in ms

extern const char deviceMapping[DEVICE_NUMBERS];

#endif