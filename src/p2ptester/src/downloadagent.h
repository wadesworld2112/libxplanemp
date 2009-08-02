/***************************************************************************
 *   Copyright (C) 2006 by Martin Domig   *
 *   martin@domig.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef DOWNLOAD_AGENT_H
#define DOWNLOAD_AGENT_H

#include <QtNetwork>

typedef	void (DownloadCallbackFunc)(void *context, QBuffer *buf, bool error, const QString& errorString);

class DownloadAgent: public QObject {
Q_OBJECT
	public:
		/** Constructor */
		DownloadAgent();

		/** Destructor */
		~DownloadAgent();

		/** Download a file, and call given callback function when done */
		void download(QString url, DownloadCallbackFunc *func, void *context);

		/** Set proxy information for downloads */
		void setProxyInformation(QString server, int port, QString user, QString password);

	private slots:
		void httpRequestFinished(int requestId, bool error);

	private:
		QHttp *http;

		class DownloadInfo {
			public:
				DownloadInfo(): getId(0), buffer(0), callback(0) {};
				
				int getId;
				QBuffer *buffer;
				DownloadCallbackFunc *callback;
				void *context;
		};

		QMap<int, DownloadInfo*> downloadMap;
};

// global object
extern DownloadAgent* downloadAgent;

#endif
