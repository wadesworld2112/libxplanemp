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

#ifndef TLV_PACKET_H
#define TLV_PACKET_H

#ifndef uint16_t
#define uint16_t unsigned short
#endif
#ifndef uint32_t
#define uint32_t unsigned int
#endif

#define TLV_BUF_SIZE 1024

/** TLV class
 *
 *  Odd numbered tags are TV, even are TLV
 */
class TLVPacket {
public:
	/** Constructors / Destructors */
	TLVPacket();
	~TLVPacket();

	/** RECEIVING ********************/

	/** parse the contents of this buffer (for later reading) */
	void setBuffer(const char* buffer, int size);

	/** get the value of a TV word */
	uint16_t getValue(uint16_t tag);

	/** get the length of a TLV word */
	uint16_t getLength(uint16_t tag);

	/** get the buffer of a TLV word */
	const char* getContent(uint16_t tag);

	/** returns true if this tag is present, false otherwise */
	bool hasEntry(uint16_t tag);


	/** SENDING ********************/

	/** call this for each new packet */
	void reset();

	/** set a TV pair */
	bool setTV(uint16_t tag, uint16_t value);

	/** set a TLV entry */
	bool setTLV(uint16_t tag, uint16_t size, const char* buffer);

	/** Create the packet.
	 *  The packet will be created in *buffer, packetLen will be set to the
	 *  actuall packet length.
	 *  This function returns true on success, false if the buffer is too small
	 */
	bool assemble(char *buffer, uint16_t bufLen, uint16_t *packetLen);

	/** returns the buffer size required to assemble the current packet */
	uint16_t assembleSize();

private:
	int _bufSize, _bufPos;
	char *_buf;
	void growBuffer(int newSize);

	typedef std::map<uint16_t, uint16_t> TVmap;
	typedef std::map<uint16_t, int> OffsetMap;

	TVmap tv; // TV pairs

	TVmap tl; // TL pairs
	OffsetMap tlv; // offsets to the TL pairs
};

#endif
