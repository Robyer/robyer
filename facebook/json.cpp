/*

Facebook plugin for Miranda Instant Messenger
_____________________________________________

Copyright � 2009-11 Michal Zelinka

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

File name      : $HeadURL: http://eternityplugins.googlecode.com/svn/trunk/facebook/json.cpp $
Revision       : $Revision: 94 $
Last change by : $Author: n3weRm0re.ewer $
Last change on : $Date: 2011-02-05 16:57:18 +0100 (so, 05 2 2011) $

*/

#include "common.h"
#include "JSON_CAJUN/reader.h"
#include "JSON_CAJUN/writer.h"
#include "JSON_CAJUN/elements.h"

int facebook_json_parser::parse_buddy_list( void* data, List::List< facebook_user >* buddy_list )
{
	using namespace json;

	try
	{
		facebook_user* current = NULL;
		std::string buddyData = static_cast< std::string* >( data )->substr( 9 );
		std::istringstream sDocument( buddyData );
		Object objDocument;
		Reader::Read(objDocument, sDocument);

		const Object& objRoot = objDocument;
		const Array& wasAvailableIDs = objRoot["payload"]["buddy_list"]["wasAvailableIDs"];

		for ( Array::const_iterator itWasAvailable( wasAvailableIDs.Begin() );
			itWasAvailable != wasAvailableIDs.End(); ++itWasAvailable)
		{
			const Number& member = *itWasAvailable;
			char was_id[32];
			lltoa( member.Value(), was_id, 10 );

			current = buddy_list->find( std::string( was_id ) );
			if ( current != NULL )
				current->status_id = ID_STATUS_OFFLINE;
		}

		const Object& nowAvailableList = objRoot["payload"]["buddy_list"]["nowAvailableList"];

		for (Object::const_iterator itAvailable(nowAvailableList.Begin());
			itAvailable != nowAvailableList.End(); ++itAvailable)
		{
			const Object::Member& member = *itAvailable;

			current = buddy_list->find( member.name );
			if ( current == NULL )
				buddy_list->insert( std::make_pair( member.name, new facebook_user( ) ) );

			current = buddy_list->find( member.name );
			const Object& objMember = member.element;
			const Boolean idle = objMember["i"];

			current->user_id = current->real_name = member.name;
			current->is_idle = ( idle.Value( ) == 1 );
			current->status_id = current->is_idle ? ID_STATUS_AWAY : ID_STATUS_ONLINE;			
		}

		const Object& userInfosList = objRoot["payload"]["buddy_list"]["userInfos"];

		for (Object::const_iterator itUserInfo(userInfosList.Begin());
			itUserInfo != userInfosList.End(); ++itUserInfo)
		{
			const Object::Member& member = *itUserInfo;

			current = buddy_list->find( member.name );
			if ( current == NULL )
				continue;

			const Object& objMember = member.element;
			const String& realName = objMember["name"];
			const String& imageUrl = objMember["thumbSrc"];

			current->real_name = utils::text::slashu_to_utf8(
			    utils::text::special_expressions_decode( realName.Value( ) ) );
			current->image_url = utils::text::slashu_to_utf8(
			    utils::text::special_expressions_decode( imageUrl.Value( ) ) );
		}
	}
	catch (Reader::ParseException& e)
	{
		proto->Log( "!!!!! Caught json::ParseException: %s", e.what() );
		proto->Log( "      Line/offset: %d/%d", e.m_locTokenBegin.m_nLine + 1, e.m_locTokenBegin.m_nLineOffset + 1 );
	}
	catch (const Exception& e)
	{
		proto->Log( "!!!!! Caught json::Exception: %s", e.what() );
	}
	catch (const std::exception& e)
	{
		proto->Log( "!!!!! Caught std::exception: %s", e.what() );
	}

	return EXIT_SUCCESS;
}

int facebook_json_parser::parse_friends( void* data )
{
	using namespace json;

	try
	{
		facebook_user* current = NULL;
		std::string buddyData = static_cast< std::string* >( data )->substr( 9 );
		std::istringstream sDocument( buddyData );
		Object objDocument;
		Reader::Read(objDocument, sDocument);

		const Object& objRoot = objDocument;
		const Object& payload = objRoot["payload"];

		for ( Object::const_iterator payload_item( payload.Begin() ); payload_item != payload.End(); ++payload_item)
		{
			const Object::Member& member = *payload_item;

			const Object& objMember = member.element;

			const String& realName = objMember["name"];
			const String& imageUrl = objMember["thumbSrc"];
			//const String& vanity = objMember["vanity"];
			const Number& gender_num = objMember["gender"];

			facebook_user fbu;
			fbu.user_id = member.name;

			fbu.real_name = utils::text::slashu_to_utf8(
			    utils::text::special_expressions_decode( realName.Value() ) );
			fbu.image_url = utils::text::slashu_to_utf8(
			    utils::text::special_expressions_decode( imageUrl.Value() ) );

			int gender = 0;
			if (gender_num.Value() == 1) {
				gender = 70; // woman
			} else if (gender_num.Value() == 2) {
				gender = 77; // man
			}

			HANDLE hContact = proto->AddToContactList(&fbu);
							
			if ( DBGetContactSettingByte(hContact,proto->m_szModuleName,"Gender", 0) != gender )
				DBWriteContactSettingByte(hContact,proto->m_szModuleName,"Gender", gender );
			
//			DBWriteContactSettingWord(hContact,proto->m_szModuleName,"Status",ID_STATUS_OFFLINE );

			DBVARIANT dbv;
			// Update Real name
			bool update_required = true;
			if ( !DBGetContactSettingUTF8String(hContact,proto->m_szModuleName,FACEBOOK_KEY_NAME,&dbv) )
			{
				update_required = strcmp( dbv.pszVal, fbu.real_name.c_str() ) != 0;
				DBFreeVariant(&dbv);
			}
			if ( update_required )
			{
				DBWriteContactSettingUTF8String(hContact,proto->m_szModuleName,FACEBOOK_KEY_NAME,fbu.real_name.c_str());
				DBWriteContactSettingUTF8String(hContact,proto->m_szModuleName,"Nick",fbu.real_name.c_str());
			}

/*			// Check avatar change
			update_required = true;
			if ( !DBGetContactSettingString(hContact,proto->m_szModuleName,FACEBOOK_KEY_AV_URL,&dbv) )
			{
				update_required = strcmp( dbv.pszVal, fbu.image_url.c_str() ) != 0;
				DBFreeVariant(&dbv);
			}
			if ( update_required ) {
				DBWriteContactSettingString(hContact,proto->m_szModuleName,FACEBOOK_KEY_AV_URL,fbu.image_url.c_str());
				DBWriteContactSettingByte(hContact,proto->m_szModuleName,FACEBOOK_KEY_NEW_AVATAR, 1);
			}
			
			// TODO: remove and wait for avatar request...
			if ( update_required || !proto->AvatarExists(&fbu) )
			{
				proto->Log("***** Saving new avatar url: %s",fbu.image_url.c_str());
				proto->ProcessAvatar(hContact,&(fbu.image_url));
			}*/

			//ForkThread(&FacebookProto::UpdateFriendWorker, proto, (void*)fbu);
		}
	}
	catch (Reader::ParseException& e)
	{
		proto->Log( "!!!!! Caught json::ParseException: %s", e.what() );
		proto->Log( "      Line/offset: %d/%d", e.m_locTokenBegin.m_nLine + 1, e.m_locTokenBegin.m_nLineOffset + 1 );
	}
	catch (const Exception& e)
	{
		proto->Log( "!!!!! Caught json::Exception: %s", e.what() );
	}
	catch (const std::exception& e)
	{
		proto->Log( "!!!!! Caught std::exception: %s", e.what() );
	}

	return EXIT_SUCCESS;
}


int facebook_json_parser::parse_messages( void* data, std::vector< facebook_message* >* messages, std::vector< facebook_notification* >* notifications )
{
	using namespace json;

	try
	{
		std::string messageData = static_cast< std::string* >( data )->substr( 9 );
		std::istringstream sDocument( messageData );
		Object objDocument;
		Reader::Read(objDocument, sDocument);

		const Object& objRoot = objDocument;
		const Array& messagesArray = objRoot["ms"];

		for (Array::const_iterator itMessage(messagesArray.Begin());
			itMessage != messagesArray.End(); ++itMessage)
		{
			const Object& objMember = *itMessage;

			const String& type = objMember["type"];

			if ( type.Value( ) == "msg" ) // chat message
			{
				const Number& from = objMember["from"];
				char was_id[32];
				lltoa( from.Value(), was_id, 10 );
				
				const Object& messageContent = objMember["msg"];
				const String& text = messageContent["text"];
        
				const Number& time_sent = messageContent["time"];
				if (time_sent.Value() > proto->facy.last_message_time_) // Check agains duplicit messages
				{
					proto->facy.last_message_time_ = time_sent.Value();

  					facebook_message* message = new facebook_message( );
					message->message_text= utils::text::special_expressions_decode(
						utils::text::slashu_to_utf8( text.Value( ) ) );
					message->time = ::time( NULL );
					message->user_id = was_id;

					messages->push_back( message );
				} else {
					proto->Log("      Got duplicit message?");
				}
			}
			else if ( type.Value( ) == "messaging" ) // inbox message
			{				
				const String& type = objMember["event"];

				if (type.Value() == "deliver") {
					const Object& messageContent = objMember["message"];

					const Number& from = messageContent["sender_fbid"];
					char was_id[32];
					lltoa( from.Value(), was_id, 10 );
				
					const String& text = messageContent["body"];
        
					const Number& time_sent = messageContent["timestamp"];
					if (time_sent.Value() > proto->facy.last_message_time_) // Check agains duplicit messages
					{
						proto->facy.last_message_time_ = time_sent.Value();

  						facebook_message* message = new facebook_message( );
						message->message_text= utils::text::special_expressions_decode(
							utils::text::slashu_to_utf8( text.Value( ) ) );

						message->time = ::time( NULL );

						message->user_id = was_id;

						messages->push_back( message );
					} else {
						proto->Log("      Got duplicit message?");
					}
				}
			}
			else if ( type.Value( ) == "group_msg" ) // chat message
			{
				if ( (::time(NULL) - proto->facy.last_grpmessage_time_) < 15 ) // RM TODO: remove dont notify more than once every 15 secs
					continue;

				proto->facy.last_grpmessage_time_ = ::time(NULL);
        
				const String& from_name = objMember["from_name"];
				const String& group_name = objMember["to_name"];

				const Number& to = objMember["to"];
				char group_id[32];
				lltoa( to.Value(), group_id, 10 );
	
				const Number& from = objMember["from"];
				char was_id[32];
				lltoa( from.Value(), was_id, 10 );

				// Ignore if message is from self user
				if (was_id == proto->facy.self_.user_id)
					continue;

				const Object& messageContent = objMember["msg"];
  				const String& text = messageContent["text"];

				std::string popup_text = utils::text::remove_html(
					utils::text::special_expressions_decode(
						utils::text::slashu_to_utf8( from_name.Value( ) ) ) );
				popup_text += ": ";
				popup_text += utils::text::remove_html(
					utils::text::special_expressions_decode(
						utils::text::slashu_to_utf8( text.Value( ) ) ) );

				std::string title = Translate("Groupchat");
				title += ": ";
				title += utils::text::remove_html(
					utils::text::special_expressions_decode(
						utils::text::slashu_to_utf8( group_name.Value( ) ) ) );

				std::string url = "/home.php?sk=group_";
				url += group_id;

				proto->Log("      Got groupchat message");
		    
				TCHAR* szTitle = mir_a2t_cp(title.c_str(), CP_UTF8);
				TCHAR* szText = mir_a2t_cp(popup_text.c_str(), CP_UTF8);
				TCHAR* szUrl = mir_a2t_cp(url.c_str(), CP_UTF8);
				proto->NotifyEvent(szTitle,szText,NULL,FACEBOOK_EVENT_OTHER, szUrl);
				mir_free(szTitle);
				mir_free(szText);
			}
			else if ( type.Value( ) == "app_msg" ) // event notification
			{
				const String& text = objMember["response"]["payload"]["title"];
				const String& link = objMember["response"]["payload"]["link"];
				// RM TODO: include additional text of notification if exits? (e.g. comment text)
				//const String& text2 = objMember["response"]["payload"]["alert"]["text"];

				const Number& time_sent = objMember["response"]["payload"]["alert"]["time_sent"];        
				if (time_sent.Value() > proto->facy.last_notification_time_) // Check agains duplicit notifications
				{
					proto->facy.last_notification_time_ = time_sent.Value();

					facebook_notification* notification = new facebook_notification( );
					notification->text = utils::text::remove_html(
  						utils::text::special_expressions_decode(
							utils::text::slashu_to_utf8( text.Value( ) ) ) );

  					notification->link = utils::text::special_expressions_decode( link.Value( ) );

					notifications->push_back( notification );
				}
			}
			else if ( type.Value( ) == "typ" ) // chat typing notification
			{
				const Number& from = objMember["from"];
				char user_id[32];
				lltoa( from.Value(), user_id, 10 );

				facebook_user fbu;
				fbu.user_id = user_id;

				HANDLE hContact = proto->AddToContactList(&fbu);
				
				if ( DBGetContactSettingWord(hContact,proto->m_szModuleName,"Status", 0) == ID_STATUS_OFFLINE )
					DBWriteContactSettingWord(hContact,proto->m_szModuleName,"Status",ID_STATUS_ONLINE);

				const Number& state = objMember["st"];
				if (state.Value() == 1)
					CallService(MS_PROTO_CONTACTISTYPING, (WPARAM)hContact, (LPARAM)60);
				else
					CallService(MS_PROTO_CONTACTISTYPING, (WPARAM)hContact, (LPARAM)PROTOTYPE_CONTACTTYPING_OFF);
			}
			/*else if ( type.Value( ) == "inbox" )
			{ // Not needed because we are getting directly messages...
				if ( proto->m_iStatus == ID_STATUS_INVISIBLE )
				{ // Notify messages only in invisible status
					const Number& unseen = objMember["unseen"];
					if (unseen.Value() > 0) {

						char count[32];
						lltoa( unseen.Value(), count, 10 );

						std::string message = Translate("Got new messages: ");
						message += count;

						TCHAR* tmessage = mir_a2t(message.c_str());
						proto->NotifyEvent( proto->m_tszUserName, tmessage, NULL, FACEBOOK_EVENT_OTHER, TEXT(FACEBOOK_URL_MESSAGES) );
						mir_free( tmessage );
					}
				}
			}*/
			else
				continue;
		}
	}
	catch (Reader::ParseException& e)
	{
		proto->Log( "!!!!! Caught json::ParseException: %s", e.what() );
		proto->Log( "      Line/offset: %d/%d", e.m_locTokenBegin.m_nLine + 1, e.m_locTokenBegin.m_nLineOffset + 1 );
	}
	catch (const Exception& e)
	{
		proto->Log ( "!!!!! Caught json::Exception: %s", e.what() );
	}

	return EXIT_SUCCESS;
}
