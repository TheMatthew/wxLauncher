/*
 Copyright (C) 2009-2011 wxLauncher Team
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef CMDLINEMANAGER_H
#define CMDLINEMANAGER_H

#include <wx/wx.h>

#include "apis/EventHandlers.h"

/** Command line has changed. */
LAUNCHER_DECLARE_EVENT_TYPE(EVT_CMD_LINE_CHANGED);
/** Custom flags have changed. */
LAUNCHER_DECLARE_EVENT_TYPE(EVT_CUSTOM_FLAGS_CHANGED);

class CmdLineManager {
private:
	CmdLineManager();
	~CmdLineManager();

public:
	static void RegisterCmdLineChanged(wxEvtHandler *handler);
	static void UnRegisterCmdLineChanged(wxEvtHandler *handler);
	static void RegisterCustomFlagsChanged(wxEvtHandler *handler);
	static void UnRegisterCustomFlagsChanged(wxEvtHandler *handler);
	static void GenerateCmdLineChanged();
	static void GenerateCustomFlagsChanged();
private:
	static EventHandlers CmdLineChangedHandlers;
	static EventHandlers CustomFlagsChangedHandlers;
};
#endif
