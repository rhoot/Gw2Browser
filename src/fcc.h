/* \file       fcc.h
*  \brief      Contains magic numbers for identify the content of files.
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>

This file is part of Gw2Browser.

Gw2Browser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifndef FCC_H_INCLUDED
#define FCC_H_INCLUDED

#include "stdafx.h"

namespace gw2b {

	enum FourCC {

		// Offset 0
		FCC_ATEX = 0x58455441,
		FCC_ATTX = 0x58545441,
		FCC_ATEC = 0x43455441,
		FCC_ATEP = 0x50455441,
		FCC_ATEU = 0x55455441,
		FCC_ATET = 0x54455441,
		FCC_3DCX = 0x58434433,
		FCC_DXT = 0x00545844,
		FCC_DDS = 0x20534444,
		FCC_strs = 0x73727473,
		FCC_asnd = 0x646e7361,
		FCC_RIFF = 0x46464952,	// resource interchange file format

		// Texture codec
		FCC_DXT1 = 0x31545844,
		FCC_DXT2 = 0x32545844,
		FCC_DXT3 = 0x33545844,
		FCC_DXT4 = 0x34545844,
		FCC_DXT5 = 0x35545844,
		FCC_DXTN = 0x4e545844,
		FCC_DXTL = 0x4c545844,
		FCC_DXTA = 0x41545844,

		// RIFF FourCC
		FCC_WEBP = 0x50424557,

		// PF FourCC
		FCC_ARMF = 0x464d5241,
		FCC_ASND = 0x444e5341,
		FCC_ABNK = 0x4b4e4241,
		FCC_ABIX = 0x58494241,
		FCC_AMSP = 0x50534d41,
		FCC_CINP = 0x504e4943,
		FCC_MODL = 0x4c444f4d,
		FCC_GEOM = 0x4d4f4547,
		FCC_DEPS = 0x53504544,
		FCC_EULA = 0x616c7565,
		FCC_hvkC = 0x436b7668,
		FCC_mapc = 0x6370616d,
		FCC_mpsd = 0x6473706d,
		FCC_PIMG = 0x474d4950,
		FCC_AMAT = 0x54414d41,
		FCC_anic = 0x63696e61,
		FCC_emoc = 0x636f6d65,
		FCC_prlt = 0x746c7270,
		FCC_cmpc = 0x63706d63,
		FCC_txtm = 0x6d747874,
		FCC_txtv = 0x76747874,

		// Sound id's
		FCC_OggS = 0x5367674f,

		// Not quite FourCC
		FCC_MZ = 0x5a4d,
		FCC_PF = 0x4650,
		FCC_JPEG = 0xffd8ff,
	};

}; // namespace gw2b

#endif // FCC_H_INCLUDED
