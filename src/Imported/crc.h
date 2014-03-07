// File:    crc.h
// Author:  David Dantowitz
// Origin:  http://wiki.wxwidgets.org/Development:_Small_Table_CRC

#pragma once

#ifndef CRC_H_INCLUDED
#define CRC_H_INCLUDED

#define INITIAL_CRC 0xFFFFFFFF

int compute_crc( int old_crc, const char* s, int len );

#endif // CRC_H_INCLUDED
