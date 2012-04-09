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

void OmegleProto::UpdateChat(const TCHAR *name, const TCHAR *message, bool addtolog)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);

	GCEVENT gce  = {sizeof(gce)};
	gce.pDest    = &gcd;
	gce.ptszText = message;
	gce.time     = ::time(NULL);
	gce.dwFlags  = GC_TCHAR;
	gcd.iType  = GC_EVENT_MESSAGE;

	if (name == NULL) {
		gcd.iType = GC_EVENT_INFORMATION;
		name = TranslateT("Server");
		gce.bIsMe = false;
	} else {
		gce.bIsMe = !_tcscmp(name, this->facy.nick_);
	}

	if (addtolog)
		gce.dwFlags  |= GCEF_ADDTOLOG;

	gce.ptszNick = name;
	gce.ptszUID  = gce.ptszNick;

	CallServiceSync(MS_GC_EVENT,0,reinterpret_cast<LPARAM>(&gce));
}

int OmegleProto::OnChatEvent(WPARAM wParam,LPARAM lParam)
{
	GCHOOK *hook = reinterpret_cast<GCHOOK*>(lParam);
	char *text;

	if(strcmp(hook->pDest->pszModule,m_szModuleName))
		return 0;

	switch(hook->pDest->iType)
	{
	case GC_USER_MESSAGE:
	{
		text = mir_t2a_cp(hook->ptszText,CP_UTF8);

		std::string* response_data = new std::string(text);
	
		if (*response_data == "/new")
			ForkThread(&OmegleProto::NewChatWorker, this, NULL);
		else if (*response_data == "/quit")
			ForkThread(&OmegleProto::StopChatWorker, this, NULL);
		else {
			switch (facy.state_)
			{
			case STATE_ACTIVE:
			{
				DBVARIANT dbv;
				if (*response_data == "/asl") {
					*response_data = "";
					if ( !getU8String( OMEGLE_KEY_ASL,&dbv ) ) {
						*response_data = dbv.pszVal;
						DBFreeVariant(&dbv);
					}
				}

				LOG("**Chat - Outgoing message: %s", response_data->c_str());
				ForkThread(&OmegleProto::SendMsgWorker, this, (void*)response_data);

				break;
			}

			case STATE_INACTIVE:
				UpdateChat(NULL, TranslateT("First you have to connect to some Stranger by sending '/new' message. You can use this to change actual Stranger during conversation too. Send '/quit' message if you want to end conversation."), false);
				break;

			//case STATE_WAITING:
			//case STATE_DISCONNECTING:
			default:
				break; // do nothing here
			}
		}
	
		break;
	}

	case GC_USER_TYPNOTIFY:
	{
		if ( facy.state_ == STATE_ACTIVE ) {
			//LOG("**Chat - Self typing: %s", response_data->c_str());
			ForkThread(&OmegleProto::SendTypingWorker, this, (void*)mir_tstrdup(hook->ptszText));
		}

		break;
	}

	case GC_USER_LEAVE:
	case GC_SESSION_TERMINATE:
	{
		mir_free( this->facy.nick_ );

		ForkThread(&OmegleProto::StopChatWorker, this, NULL);
		break;
	}
	}

	return 0;
}

/*void OmegleProto::SendChatEvent(int type)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.pDest = &gcd;

	CallServiceSync(MS_GC_EVENT,WINDOW_CLEARLOG,reinterpret_cast<LPARAM>(&gce));
}*/

void OmegleProto::AddChatContact(const TCHAR *name)
{	
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType  = GC_EVENT_JOIN;

	GCEVENT gce    = {sizeof(gce)};
	gce.pDest      = &gcd;
	gce.dwFlags    = GC_TCHAR | GCEF_ADDTOLOG;
	gce.ptszNick   = name;
	gce.ptszUID    = gce.ptszNick;
	gce.time       = static_cast<DWORD>(time(0));

	if (name == NULL)
		gce.bIsMe = false;
	else 
		gce.bIsMe = !_tcscmp(name, this->facy.nick_);

	if (gce.bIsMe)
		gce.ptszStatus = _T("Admin");
	else
		gce.ptszStatus = _T("Normal");

	CallServiceSync(MS_GC_EVENT,0,reinterpret_cast<LPARAM>(&gce));
}

void OmegleProto::DeleteChatContact(const TCHAR *name)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType  = GC_EVENT_PART;

	GCEVENT gce    = {sizeof(gce)};
	gce.pDest      = &gcd;
	gce.dwFlags    = GC_TCHAR | GCEF_ADDTOLOG;
	gce.ptszNick   = name;
	gce.ptszUID    = gce.ptszNick;
	gce.time       = static_cast<DWORD>(time(0));
	if (name == NULL)
		gce.bIsMe = false;
	else 
		gce.bIsMe = !_tcscmp(name, this->facy.nick_);

	CallServiceSync(MS_GC_EVENT,0,reinterpret_cast<LPARAM>(&gce));
}

int OmegleProto::OnJoinChat(WPARAM,LPARAM suppress)
{	
	GCSESSION gcw = {sizeof(gcw)};

	// Create the group chat session
	gcw.dwFlags   = GC_TCHAR;
	gcw.iType     = GCW_CHATROOM;
	gcw.pszModule = m_szModuleName;
	gcw.ptszName  = m_tszUserName;
	gcw.ptszID    = m_tszUserName;
	CallServiceSync(MS_GC_NEWSESSION, 0, (LPARAM)&gcw);

	if(m_iStatus == ID_STATUS_OFFLINE)
		return 0;

	// Create a group
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);

	GCEVENT gce = {sizeof(gce)};
	gce.pDest = &gcd;
	gce.dwFlags = GC_TCHAR;

	gcd.iType = GC_EVENT_ADDGROUP;

	gce.ptszStatus = _T("Admin");
	CallServiceSync( MS_GC_EVENT, NULL, reinterpret_cast<LPARAM>(&gce) );
	
	gce.ptszStatus = _T("Normal");
	CallServiceSync( MS_GC_EVENT, NULL, reinterpret_cast<LPARAM>(&gce) );

	SetTopic(TranslateT("Omegle is a great way of meeting new friends!"));
		
	// Note: Initialization will finish up in SetChatStatus, called separately
	if(!suppress)
		SetChatStatus(m_iStatus);

	return 0;
}

void OmegleProto::SetTopic(const TCHAR *topic)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_TOPIC;

	GCEVENT gce = {sizeof(gce)};
	gce.pDest = &gcd;
	gce.dwFlags = GC_TCHAR;
	gce.time = ::time(NULL);
	
	gce.ptszText = topic;
	CallServiceSync(MS_GC_EVENT,0,  reinterpret_cast<LPARAM>(&gce));
}

int OmegleProto::OnLeaveChat(WPARAM,LPARAM)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.time = ::time(NULL);
	gce.pDest = &gcd;

	CallServiceSync(MS_GC_EVENT,SESSION_OFFLINE,  reinterpret_cast<LPARAM>(&gce));
	CallServiceSync(MS_GC_EVENT,SESSION_TERMINATE,reinterpret_cast<LPARAM>(&gce));

	return 0;
}

void OmegleProto::SetChatStatus(int status)
{
	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.time = ::time(NULL);
	gce.pDest = &gcd;
	
	if(status == ID_STATUS_ONLINE)
	{		
		// Free previously loaded name
		mir_free(facy.nick_);
		
		// Load actual name from database
		DBVARIANT dbv;
		if ( !DBGetContactSettingTString(NULL, m_szModuleName, OMEGLE_KEY_NAME, &dbv) )
		{
			facy.nick_ = mir_tstrdup(dbv.ptszVal);
			DBFreeVariant(&dbv);
		} else {
			facy.nick_ = mir_tstrdup(TranslateT("You"));
			DBWriteContactSettingTString(NULL, m_szModuleName, OMEGLE_KEY_NAME, facy.nick_);
		}
		
		// Add self contact
		AddChatContact(facy.nick_);

		CallServiceSync(MS_GC_EVENT,SESSION_INITDONE,reinterpret_cast<LPARAM>(&gce));
		CallServiceSync(MS_GC_EVENT,SESSION_ONLINE,  reinterpret_cast<LPARAM>(&gce));
	}
	else
	{
		CallServiceSync(MS_GC_EVENT,SESSION_OFFLINE,reinterpret_cast<LPARAM>(&gce));
	}
}

void OmegleProto::ClearChat()
{
	if (getByte(OMEGLE_KEY_NO_CLEAR, 0))
		return;

	GCDEST gcd = { m_szModuleName };
	gcd.ptszID = const_cast<TCHAR*>(m_tszUserName);
	gcd.iType = GC_EVENT_CONTROL;

	GCEVENT gce = {sizeof(gce)};
	gce.dwFlags = GC_TCHAR;
	gce.pDest = &gcd;

	CallServiceSync(MS_GC_EVENT,WINDOW_CLEARLOG,reinterpret_cast<LPARAM>(&gce));
}