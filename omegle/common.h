/*

Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2011-12 Robert P�sel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#pragma warning(disable:4996)

#define MIRANDA_VER    0x0A00
#define _WIN32_WINNT   0x0500
#define _WIN32_WINDOWS 0x0500

#include <m_stdhdr.h>

#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <map>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#include <io.h>

#include <windows.h>
#include <win2k.h>
#include <commctrl.h>

#include <newpluginapi.h>
#include <m_version.h>
#include <m_system.h>
#include <m_system_cpp.h>
#include <m_chat.h>
#include <m_clistint.h>
#include <m_database.h>
#include <m_langpack.h>
#include <m_netlib.h>
#include <m_options.h>
#include <m_popup.h>
#include <m_protocols.h>
#include <m_protosvc.h>
#include <m_protoint.h>
#include <m_protomod.h>
#include <m_skin.h>
#include <statusmodes.h>
#include <m_icolib.h>
#include <m_utils.h>
#include <m_hotkeys.h>
#include <m_updater.h>
//#include <m_msg_buttonsbar.h>

class OmegleProto;

#define LOG Log

#include "http.h"
#include "utils.h"
#include "client.h"
#include "proto.h"
#include "db.h"
#include "constants.h"
#include "dialogs.h"
#include "theme.h"
#include "resource.h"

extern HINSTANCE g_hInstance;
extern std::string g_strUserAgent;
extern DWORD g_mirandaVersion;