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

#ifndef __DROPLET_H__
#define __DROPLET_H__

struct Action {
  unsigned long Offset;
  unsigned char Mode;
  unsigned char Pin;
  Action *Next;
};


class Controller
{
private:
  static bool initDone;
  static bool taskRunning;
  static Action *firstAction;
  static void AddAction(Action *newAction);

public:
  static void Setup();
  static void Loop();
  static void AddTask(const unsigned char targetPin, const unsigned long offset, const unsigned long duration);
  static void DeleteTasks();
  static void TaskInfo();
  static void ReqRun(const unsigned char rounds, const unsigned long pause);
  static void ReqCancel();
  static void ReqSwitch();
};


#endif
