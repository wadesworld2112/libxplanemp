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

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include "common.h"
#include "fsdmessage.h"

#define TCPSOCKET_BUFSIZE 2048

class tcpsocket {
  public:
    tcpsocket();
    ~tcpsocket();
  
    bool open(const string& host, const string& port);
	void close();

    string readln();
    bool writeln(const string& str);

	string ip();
	const ipaddress get_myip() { return _socket.get_myip(); };

	bool connected();
	bool hasdata() { return _socket.waitfor(0); };

	string errorMessage() { return _error; };
	bool eof() { return _eof; };

  private:
	ipstream _socket;
    string _host, _port;
	string _error;
	bool _opened;
	bool _eof;
        
	const static int _maxtoken = 4096;
};

#endif
