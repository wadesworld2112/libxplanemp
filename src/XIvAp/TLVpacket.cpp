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
#include "common.h"
#include "TLVpacket.h"

TLVPacket::TLVPacket()
{
	_bufSize = TLV_BUF_SIZE;
	_buf = new char[_bufSize];
	_bufPos = 0;
}

TLVPacket::~TLVPacket()
{
	reset();
	if(_buf != NULL) // it wont be...
		delete[] _buf;
}

void TLVPacket::growBuffer(int newSize)
{
	if(newSize < _bufSize)
		return;

	char *buf = new char[newSize];
	memcpy(buf, _buf, _bufSize);
	delete[] _buf;
	_buf = buf;
}

void TLVPacket::setBuffer(const char* buffer, int size)
{
	reset();
	if(size > _bufSize)
		growBuffer(size);

	const char *p = buffer;
	int bytes = 0;

	while(bytes < size) {
		uint16_t tag = ntohs(*(uint16_t*)p);
		p += sizeof(uint16_t);
		bytes += sizeof(uint16_t);

		if(tag % 2 == 0) { // TLV
			uint16_t length = ntohs(*(uint16_t*)p);
			p += sizeof(uint16_t);
			bytes += sizeof(uint16_t);
			if(length > size - bytes) {
				printf("broken packet - length is %i but total packetlen is %i\r\n", length, size);
				return;
			}
			setTLV(tag, length, p);
			p += length;
			bytes += length;
		} else { // TV
			uint16_t value = ntohs(*(uint16_t*)p);
			p += sizeof(uint16_t);
			bytes += sizeof(uint16_t);
			setTV(tag, value);
		}
	}
}

uint16_t TLVPacket::getValue(uint16_t tag)
{
	if(tag % 2 == 0) // even number - this aint right
		return 0;
	if(!hasEntry(tag))
		return 0;
	return tv[tag];
}

uint16_t TLVPacket::getLength(uint16_t tag)
{
	if(tag % 2 == 1) // odd number - this aint right
		return 0;
	if(!hasEntry(tag))
		return 0;

	return tl[tag];
}

const char* TLVPacket::getContent(uint16_t tag)
{
	if(tag % 2 == 1) // odd number - this aint right
		return 0;
	if(!hasEntry(tag))
		return 0;

	return _buf + tlv[tag];
}

bool TLVPacket::hasEntry(uint16_t tag)
{
	if(tag % 2 == 0) // even?
		return tlv.find(tag) != tlv.end();
	else  // odd
		return tv.find(tag) != tv.end();		
}

void TLVPacket::reset()
{
	// clear all entries
	tv.clear();
	tl.clear();
	tlv.clear();
	_bufPos = 0;
}

bool TLVPacket::setTV(uint16_t tag, uint16_t value)
{
	if(tag % 2 == 0)
		return false;
	tv[tag] = value;
	return true;
}

bool TLVPacket::setTLV(uint16_t tag, uint16_t size, const char* buffer)
{
	if(tag % 2 == 1)
		return false;

	if(_bufSize < _bufPos + size)
		growBuffer(_bufPos + size + 128);

	tl[tag] = size;
	tlv[tag] = _bufPos;
	memcpy(_buf + _bufPos, buffer, size);
	_bufPos += size;

	return true;
}

bool TLVPacket::assemble(char *buffer, uint16_t bufLen, uint16_t *packetLen)
{
	uint16_t len = assembleSize();
	if(len > bufLen)	// buffer is not large enough
		return false;

	*packetLen = len;
	char *p = buffer;

	TVmap::const_iterator tvit = tv.begin();
	TVmap::const_iterator tlit = tl.begin();

	while(tvit != tv.end() || tlit != tl.end()) {
		bool useTV;
		if(tlit == tl.end()) useTV = true;              // no more TLV values
		else if(tvit == tv.end()) useTV = false;          // no more TV values
		else if(tvit->first < tlit->first) useTV = true; // TV tag is smaller than TLV
		else useTV = false;

		if(useTV) {
			// take TV value next
			*((uint16_t*)p) = htons(tvit->first);
			p += sizeof(uint16_t);
			*((uint16_t*)p) = htons(tvit->second);
			p += sizeof(uint16_t);
			++tvit;
		} else {
			// take TLV value next
			*((uint16_t*)p) = htons(tlit->first);
			p += sizeof(uint16_t);
			*((uint16_t*)p) = htons(tlit->second);
			p += sizeof(uint16_t);
			memcpy(p, _buf + tlv[tlit->first], tlit->second);
			p += tlit->second;
			++tlit;
		}
	}
	return true;
}

uint16_t TLVPacket::assembleSize()
{
	uint16_t result = (uint16_t)tv.size() * 2 * sizeof(uint16_t); // 2 16bit values for each TV pair
	result += (uint16_t)tl.size() * 2 * sizeof(uint16_t); // 2 16bit values for each TLV pair...
	result += _bufPos; // ...plus the amount of data in the buffers

	return result;
}
