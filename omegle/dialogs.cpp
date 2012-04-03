/*

Omegle plugin for Miranda Instant Messenger
_____________________________________________

Copyright © 2011-12 Robert Pösel

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

#include "common.h"

static BOOL LoadDBCheckState(OmegleProto* ppro, HWND hwnd, int idCtrl, const char* szSetting, BYTE bDef = 0)
{
	BOOL state = DBGetContactSettingByte(NULL, ppro->m_szModuleName, szSetting, bDef);
	CheckDlgButton(hwnd, idCtrl, state);
	return state;
}

static BOOL StoreDBCheckState(OmegleProto* ppro, HWND hwnd, int idCtrl, const char* szSetting)
{
	BOOL state = IsDlgButtonChecked(hwnd, idCtrl);
	DBWriteContactSettingByte(NULL, ppro->m_szModuleName, szSetting, (BYTE)state);
	return state;
}

INT_PTR CALLBACK OmegleAccountProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
	OmegleProto *proto;

	switch ( message )
	{

	case WM_INITDIALOG:
		TranslateDialogDefault(hwnd);

		proto = reinterpret_cast<OmegleProto*>(lparam);
		SetWindowLong(hwnd,GWLP_USERDATA,lparam);

		DBVARIANT dbv;
		if( !DBGetContactSettingTString(NULL, proto->ModuleName(), OMEGLE_KEY_NAME, &dbv) )
		{
			SetDlgItemText(hwnd, IDC_NAME, dbv.ptszVal);
			DBFreeVariant(&dbv);
		}

		LoadDBCheckState(proto, hwnd, IDC_MEET_COMMON, OMEGLE_KEY_MEET_COMMON);

		return TRUE;

	case WM_COMMAND:
		if ( HIWORD( wparam ) == EN_CHANGE && reinterpret_cast<HWND>(lparam) == GetFocus() )
		{
			switch(LOWORD(wparam))
			{
			case IDC_NAME:
				SendMessage(GetParent(hwnd),PSM_CHANGED,0,0);
			}
		}
		break;

	case WM_NOTIFY:
		if ( reinterpret_cast<NMHDR*>(lparam)->code == PSN_APPLY )
		{
			proto = reinterpret_cast<OmegleProto*>(GetWindowLong(hwnd,GWLP_USERDATA));
			//char str[128];
			TCHAR tstr[128];

			GetDlgItemText(hwnd, IDC_NAME, tstr, sizeof(tstr));
			if ( lstrlen( tstr ) > 0 )
				DBWriteContactSettingTString(NULL, proto->m_szModuleName, OMEGLE_KEY_NAME, tstr);
			else
				DBDeleteContactSetting(NULL, proto->m_szModuleName, OMEGLE_KEY_NAME);

			// TODO: change proto->facy.nick_ here

/*			GetDlgItemTextA(hwnd,IDC_UN,str,sizeof(str));
			DBWriteContactSettingString(0,proto->ModuleName(),OMEGLE_KEY_LOGIN,str);*/

			StoreDBCheckState(proto, hwnd, IDC_MEET_COMMON, OMEGLE_KEY_MEET_COMMON);

			return TRUE;
		}
		break;

	}
	return FALSE;
}

INT_PTR CALLBACK OmegleOptionsProc( HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam )
{
	OmegleProto *proto = reinterpret_cast<OmegleProto*>(GetWindowLong(hwnd,GWLP_USERDATA));

	switch ( message )
	{

	case WM_INITDIALOG:
	{
		TranslateDialogDefault(hwnd);

		proto = reinterpret_cast<OmegleProto*>(lparam);
		SetWindowLong(hwnd,GWLP_USERDATA,lparam);

		DBVARIANT dbv;
		if( !DBGetContactSettingTString(NULL, proto->ModuleName(), OMEGLE_KEY_NAME, &dbv) )
		{
			SetDlgItemText(hwnd, IDC_NAME, dbv.ptszVal);
			DBFreeVariant(&dbv);
		}

		LoadDBCheckState(proto, hwnd, IDC_MEET_COMMON, OMEGLE_KEY_MEET_COMMON);
		LoadDBCheckState(proto, hwnd, IDC_HI_ENABLED, OMEGLE_KEY_HI_ENABLED);
		LoadDBCheckState(proto, hwnd, IDC_NOCLEAR, OMEGLE_KEY_NO_CLEAR);
		LoadDBCheckState(proto, hwnd, IDC_DONTSTOP, OMEGLE_KEY_DONT_STOP);
		LoadDBCheckState(proto, hwnd, IDC_REUSE_QUESTIONS, OMEGLE_KEY_REUSE_QUESTION);

	} return TRUE;

	case WM_COMMAND: {
		if ((LOWORD(wparam)==IDC_NAME /*|| LOWORD(wparam)==IDC_PW || LOWORD(wparam)==IDC_GROUP*/) &&
		    (HIWORD(wparam)!=EN_CHANGE || (HWND)lparam!=GetFocus()))
			return 0;
		else
			SendMessage(GetParent(hwnd),PSM_CHANGED,0,0);

	} break;

	case WM_NOTIFY:
		if ( reinterpret_cast<NMHDR*>(lparam)->code == PSN_APPLY )
		{
			char str[128]; TCHAR tstr[128];

			GetDlgItemText(hwnd, IDC_NAME, tstr, sizeof(tstr));
			if ( lstrlen( tstr ) > 0 )
				DBWriteContactSettingTString(NULL, proto->m_szModuleName, OMEGLE_KEY_NAME, tstr);
			else
				DBDeleteContactSetting(NULL, proto->m_szModuleName, OMEGLE_KEY_NAME);

			// TODO: change proto->facy.nick_ here

			StoreDBCheckState(proto, hwnd, IDC_MEET_COMMON, OMEGLE_KEY_MEET_COMMON);
			StoreDBCheckState(proto, hwnd, IDC_HI_ENABLED, OMEGLE_KEY_HI_ENABLED);
			StoreDBCheckState(proto, hwnd, IDC_NOCLEAR, OMEGLE_KEY_NO_CLEAR);
			StoreDBCheckState(proto, hwnd, IDC_DONTSTOP, OMEGLE_KEY_DONT_STOP);
			StoreDBCheckState(proto, hwnd, IDC_REUSE_QUESTIONS, OMEGLE_KEY_REUSE_QUESTION);
			
			return TRUE;
		}
		break;

	}
	
	return FALSE;
}
