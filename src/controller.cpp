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
#include "serialcom.h"


// init static members
bool Controller::initDone = false;
bool Controller::taskRunning = false;
bool Controller::taskStart = false;
bool Controller::taskCancel = false;
unsigned char Controller::loopState = 0;
unsigned char Controller::roundsToGo = 0;
unsigned long Controller::timeStart = 0;
unsigned long Controller::roundDelay = 0;
Action* Controller::firstAction = NULL;
Action* Controller::currentAction = NULL;


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
 * Controller Loop
 * Statemachine - run only after init
 */
void Controller::Loop(){
  if (!initDone) {
    return;
  }
  switch (loopState)
  {
  case CTRL_STANDBY:
    if (taskStart && (firstAction != NULL)) {
      SerialCom::Log(INFO, "Task started...");
      loopState = CTRL_TASKBEGIN;
      taskRunning = true;
      taskStart = false;
    }
    break;
  case CTRL_TASKBEGIN:
    if (taskCancel) {
      loopState = CTRL_CANCEL;
      return;
    }
    if (roundsToGo <= 0) {
      loopState = CTRL_STANDBY;
      return;
    }
    SerialCom::Log(INFO, ("rounds to go: " + (String)roundsToGo).c_str());
    roundsToGo--;
    timeStart = micros();
    currentAction = firstAction;
    loopState = CTRL_TASK;
    break;
  case CTRL_TASK:
    if (taskCancel) {
      loopState = CTRL_CANCEL;
      return;
    }
    if (currentAction->Offset <= GetDeltaT(timeStart)) {
      digitalWrite(currentAction->Pin, currentAction->Mode);
      if (currentAction->Next != NULL) {
        currentAction = currentAction->Next;
      } else {
        loopState = CTRL_PAUSEBEGIN;
      }
    }
    break;
  case CTRL_PAUSEBEGIN:
    if (taskCancel) {
      loopState = CTRL_CANCEL;
      return;
    }
    if (roundsToGo <= 0) {
      taskRunning = false;
      roundsToGo = 0;
      roundDelay = 0;
      loopState = CTRL_STANDBY;
      SerialCom::Log(INFO, "Task finished");
      return;
    }
    timeStart = micros();
    loopState = CTRL_PAUSE;
    break;
  case CTRL_PAUSE:
    if (taskCancel) {
      loopState = CTRL_CANCEL;
      return;
    }
    if (roundDelay <= GetDeltaT(timeStart)) { loopState = CTRL_TASKBEGIN; }
    break;  
  case CTRL_CANCEL:
    // set all pins to LOW and enter standby
    for(int i = 0; i < DEVICE_NUMBERS; i++) {
      digitalWrite(deviceMapping[i], LOW);
    }
    taskCancel = false;
    taskRunning = false;
    roundsToGo = 0;
    roundDelay = 0;
    loopState = CTRL_STANDBY;
    SerialCom::Log(INFO, "Task canceled");
    break;  
  default:
    loopState = CTRL_STANDBY;
    break;
  }
}


/*
 * Add two actions to memory.
 *    HIGH action at offset
 *    LOW action at offset + duration.
 */
void Controller::AddTask(const unsigned char targetPin, const unsigned long offset, const unsigned long duration) {
  // abort if tasks are currently running
  if (taskRunning) {
    SerialCom::Log(INFO, "denied - tasks are currently running");
    return;
  }
  // check if enough momory is available
  if (freeMemory() < 2 * sizeof(struct Action)) {
    SerialCom::Log(ERROR, "not enough memory available");
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
    SerialCom::Log(ERROR, "denied - tasks are currently running");
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
    SerialCom::Log(ERROR, "denied - tasks are currently running");
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
 * display list of tasks
 */
void Controller::TaskInfo() {
  // abort if tasks are currently running
  if (taskRunning) {
    SerialCom::Log(ERROR, "denied - tasks are currently running");
    return;
  }
  Action *action = firstAction;
  if(action == NULL) {
    SerialCom::Log(MINLEVEL, "No actions defined!");
  } else {
    while(action != NULL) {
      SerialCom::Log(MINLEVEL, ((String)action->Pin + ":" + (String)action->Offset + ":" + (String)action->Mode).c_str());
      action = action->Next;
    }
  }
}


/*
 * Request start of new round of tasks
 */
void Controller::ReqRun(const unsigned char rounds, const unsigned long delay) {
  if (taskRunning) {
    SerialCom::Log(WARN, "task already running...");
    return;
  }
  if (firstAction == NULL) {
    SerialCom::Log(WARN, "no task defined...");
    return;
  }
  roundsToGo = rounds;
  roundDelay = delay;
  taskStart = true;
}


/*
 * Request cancellation tasks
 */
void Controller::ReqCancel() {
  if (taskRunning) {
    taskCancel = true;
    SerialCom::Log(INFO, "aborting Task requested");
  } else {
    SerialCom::Log(INFO, "No tasks to cancel");
  }
}


/*
 * Request static switch of pins
 * only allowed if no task running
 */
void Controller::ReqSwitch(const unsigned char targetPin, const unsigned char mode) {
  if (taskRunning) {
    SerialCom::Log(ERROR, "denied - task running");
    return;
  }
  digitalWrite(targetPin, mode);
}


/*
 * Get time since starttime in micros
 * max span are ~70 minutes
 * consider one overflow - unsigned long
 */
unsigned long Controller::GetDeltaT(const unsigned long tStart) {
  unsigned long tAct = micros();
  if (tAct >= tStart) {
    return (tAct - tStart);
  } else {
    return (tAct + MAXMICROS - tStart);
  }
}

