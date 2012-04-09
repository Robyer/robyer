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

#pragma once

#define FORCE_DISCONNECT true

#define STATE_INACTIVE			0	// not connected to any stranger
#define STATE_WAITING			1	// connecting to stranger
#define STATE_ACTIVE			2	// active discussion
#define STATE_DISCONNECTING		3	// disconnecting from stranger

class Omegle_client
{
public:

	// Client definition
	Omegle_client( )
	{
		chat_id_ = server_ = "";
		nick_ = NULL;
		send_message_lock_ = NULL;
		//msgid_ = 0;
		state_ = STATE_INACTIVE;
		null_count_ = 0;

		hConnection = NULL;
		hEventsConnection = NULL;
		connection_lock_ = NULL;
	}

	HANDLE hConnection;
	HANDLE hEventsConnection;
	HANDLE connection_lock_;

	// Parent handle
	OmegleProto*  parent;

	// Chat data
	std::string chat_id_;
	std::string server_;
	TCHAR *nick_;

	HANDLE send_message_lock_;
	//int	msgid_;
	
	// State of client
	int state_;

	// Data storage
	std::map< std::string, std::string >    headers;
	void    store_headers( http::response* resp, NETLIBHTTPHEADER* headers, int headers_count );
	
	std::string get_server( );

	// Connection handling
	unsigned int error_count_;
	unsigned int null_count_;

	bool    handle_entry( std::string method );
	bool    handle_success( std::string method );
	bool    handle_error( std::string method, bool force_disconnect = false );

	void __inline increment_error( ) { error_count_++; }
	void __inline decrement_error( ) { if ( error_count_ > 0 ) error_count_--; }
	void __inline reset_error( ) { error_count_ = 0; }	

	bool    start( );
	bool    stop( );
	bool    events( );

	bool    typing_start( );
	bool    typing_stop( );
	bool    recaptcha( );

	std::string get_page( int );
	
	bool    send_message( std::string message_text );

	// HTTP communication
	http::response  flap( const int request_type, std::string* request_data = NULL, std::string* get_data = NULL );

	int     choose_method( int );
	std::string choose_proto( int );
	std::string choose_server( int, std::string* data = NULL, std::string* get_data = NULL );
	std::string choose_action( int, std::string* data = NULL, std::string* get_data = NULL );
	std::string choose_request_url( int, std::string* data = NULL, std::string* get_data = NULL );

	NETLIBHTTPHEADER*   get_request_headers( int request_type, int* headers_count );

	// Netlib handle

	HANDLE handle_;

	void set_handle(HANDLE h)
	{
		handle_ = h;
	}
};
