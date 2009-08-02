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

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include "common.h"
#include "tcpsocket.h"

// STL headers
#include <list>

namespace HTTP {

enum Status {
	Status_Connecting,
	Status_Downloading,
	Status_Finished,
	Status_Error
};

class HTTPClient
{
public:
	HTTPClient();

	Status Download(const string& url, const string& file, float timestamp);

	// call this once in a while until it returns Status_Finished or Status_Error
	Status status(float timestamp, float timeout);

	// returns a error message in user readable form
	string message() { return _message; };

private:
	string _filename, _message;
	tcpsocket _socket;

	typedef std::list<string> Content;
	Content _content;
	Status _status;
	bool _ok;
	bool _headers;
	float _starttime;

	void poll();
};

} // namespace

#endif