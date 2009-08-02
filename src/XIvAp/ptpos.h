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

#ifndef _PTPOS_H_
#define _PTPOS_H_

const UInt16 IVAO_EchoRequest		= 0x0401;
const UInt16 IVAO_EchoReply			= 0x0403;

const UInt16 IVAO_PTPOS_Interval	= 0x0501;
const UInt16 IVAO_PTPOS_Position	= 0x0502;
const UInt16 IVAO_PTPOS_Params		= 0x0504;
const UInt16 IVAO_PTPOS_CallsignReq	= 0x0505;
const UInt16 IVAO_PTPOS_Callsign	= 0x0506;

// size of one position packet, including TL headers:
// lat (float), lon (float), alt (float), PBH (uint32), timestamp (uint32), speed (uint16)
#define PTPOS_POSITION_SIZE  24

#endif
