/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2009-11 Michal Zelinka, 2011-12 Robert P�sel

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

#define CODE_BLOCK_BEGIN        {
#define CODE_BLOCK_TRY          try {
#define CODE_BLOCK_CATCH        } catch(const std::exception &e) {
#define CODE_BLOCK_INFINITE     while( true ) {
#define CODE_BLOCK_END          }

#define FLAG_CONTAINS(x,y)      ( ( x & y ) == y )
#define REMOVE_FLAG(x,y)        ( x = ( x & ~y ) )

#define LOG Log

#define LOG_NOTIFY              0
#define LOG_WARNING             1
#define LOG_ALERT               2
#define LOG_FAILURE             3
#define LOG_CRITICAL            4

#if defined( _UNICODE )
#define NIIF_INTERN_TCHAR NIIF_INTERN_UNICODE // m_clist.h
#define mir_tstrdup mir_wstrdup // m_system.h
#else
#define NIIF_INTERN_TCHAR 0
#define mir_tstrdup mir_strdup
#endif
