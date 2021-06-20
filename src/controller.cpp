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

// include arduino types and constants
#include <Arduino.h>

#include "controller.h"
#include "ardudrop.h"
#include "utils.h"
#include "logger.h"

// init static members
bool Controller::initDone = false;
bool Controller::taskRunning = false;
Action* Controller::firstAction = NULL;

/*
 * Setup Controller - run only once
 */
void Controller::Setup() {
  if (initDone) {
    return;
  }
  // setup pins as output
  for(int i = 0; i < DEVICE_NUMBERS; i++) {
    pinMode(deviceMapping[i], OUTPUT);
    // manually set pin to LOW as some boards default to HIGH
    digitalWrite(deviceMapping[i], LOW);
  }
  initDone = true;
}


/*
 * Add two actions to memory.
 *    HIGH action at offset
 *    LOW action at offset + duration.
 */
void Controller::AddTask(const unsigned char targetPin, const unsigned long offset, const unsigned long duration) {
  // abort if tasks are currently running
  if (taskRunning) {
    Logger::Log(INFO, "denied - tasks are currently running");
    return;
  }
  // check if enough momory is available
  if (freeMemory() < 2 * sizeof(struct Action)) {
    Logger::Log(ERROR, "not enough memory available");
    return;
  }
  // opening action
  Action *actionOn = (Action*) malloc(sizeof(struct Action));
  actionOn->Offset = offset;
  actionOn->Mode = HIGH;
  actionOn->Pin = targetPin;
  actionOn->Next = NULL;
  AddAction(actionOn);
  // closing action
  Action *actionOff = (Action*) malloc(sizeof(struct Action));
  actionOff->Offset = offset + duration;
  actionOff->Mode = LOW;
  actionOff->Pin = targetPin;
  actionOff->Next = NULL;
  AddAction(actionOff);
}


/*
 * Add a new action to droplet
 * Actions are sorted by their offset in ascending order
 */
void Controller::AddAction(Action *newAction) {
  // abort if tasks are currently running
  if (taskRunning) {
    Logger::Log(ERROR, "denied - tasks are currently running");
    return;
  }
  Action *action = firstAction;
  // first action
  if(action == NULL) {
    firstAction = newAction;
    return;
  }
  // add action at first position
  if(firstAction->Offset > newAction->Offset) {
    firstAction = newAction;
    newAction->Next = action;
    return;
  }
  // go to correct position
  while(action->Next != NULL && action->Next->Offset <= newAction->Offset) {
    action = action->Next;
  }
  // insert at new position
  newAction->Next = action->Next;
  action->Next = newAction;
}


/* 
 * Removes all actions from droplet and releases their memory
 */
void Controller::DeleteTasks() {
  // abort if tasks are currently running
  if (taskRunning) {
    Logger::Log(ERROR, "denied - tasks are currently running");
    return;
  }
  Action *action = firstAction;  
  while(action != NULL) {
    Action *next = action->Next;
    free(action);
    action = next;
  }
  firstAction = NULL;
}


/*
 * use logger to display list of tasks
 */
void Controller::TaskInfo() {
  // abort if tasks are currently running
  if (taskRunning) {
    Logger::Log(ERROR, "denied - tasks are currently running");
    return;
  }
  Action *action = firstAction;
  if(action == NULL) {
    Logger::Log(MINLEVEL, "No actions defined!");
  } else {
    while(action != NULL) {
      Logger::Log(MINLEVEL, ((String)action->Pin + ":" + (String)action->Offset + ":" + (String)action->Mode).c_str());
      action = action->Next;
    }
  }
}



