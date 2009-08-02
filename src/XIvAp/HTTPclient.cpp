/****************************************************************************

    X-IvAp  -  Martin Domig <martin@domig.net>
    Copyright (C) 2006 by Martin Domig

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/
#include "HTTPClient.h"

using namespace HTTP;

HTTPClient::HTTPClient():
_filename(""),
_message(""),
_ok(true),
_headers(true),
_starttime(0)
{
}

#define SOCKWRITE(str) { if(_ok) { \
	_ok = _socket.writeln(str); \
	if(!_ok) { _message = _socket.errorMessage(); _status = Status_Error; } \
	} \
}


Status HTTPClient::Download(const string& url, const string& file, float timestamp)
{
	_content.clear();
	if(_socket.connected()) _socket.close();
	_ok = true;

	string server = url;
	int p = pos("//", server);		// http://www.host.com/path/to/the/file.ext
	if(p < 0) {
		_message = "Invalid protocol";
		_status = Status_Error;
		return _status;
	}
	del(server, 0, p + 2);			// www.host.com/path/to/the/file.ext
	p = pos('/', server);
	if(p < 3) {
		_status = Status_Error;
		_message = "Invalid host '" + server + "'";
		return _status;
	}
	string path = server;
	server = copy(server, 0, p);
	del(path, 0, p);

	_filename = file;

	_socket.open(server, "80");
	_status = Status_Connecting;


	bool ok = true;

	SOCKWRITE("GET " + path + " HTTP/1.1");
	SOCKWRITE("Host: " + server);
	SOCKWRITE("Connection: close");
	SOCKWRITE("User-Agent: X-IvAp");
	SOCKWRITE("Cache-Control: max-age=0");
	SOCKWRITE("");

	_headers = true;
	_starttime = timestamp;
	return _status;
}

Status HTTPClient::status(float timestamp, float timeout)
{
	if(_status == Status_Downloading && timestamp > _starttime + timeout) {
		_status = Status_Error;
		_message = "Timeout reached";
		_socket.close();
		_headers = true;
		_starttime = 0;
		return _status;
	}

	string str;
	do {
		str = _socket.readln();
		if(length(str) > 0) {
			_status = Status_Downloading;
			if(!_headers)
				_content.push_back(str);
		} else if(_socket.connected() && !_socket.eof()) {
			if(_socket.hasdata()) {
				str = "wait";
				_headers = false;
			}
		}
	} while(str != "");

	if(_socket.connected())
		return _status;

	if(_content.size() == 0) {
		_status = Status_Error;
		_message = "Empty download";
		return _status;
	}

	_status = Status_Finished;
	FILE *fp = fopen(_filename, "w");
	if(fp == NULL) {
		_status = Status_Error;
		_message = "Could not open file for writing: " + _filename;
		return _status;
	}
	for(Content::const_iterator i = _content.begin(); i != _content.end(); ++i)
		fputs((*i) + "\r\n", fp);
	fclose(fp);
	return _status;
}
