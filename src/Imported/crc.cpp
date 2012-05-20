// File:    crc.cpp
// Author:  David Dantowitz

#include "stdafx.h"
#include "crc.h"

int crc_table[16] =
{
    0x00000000,	0xfdb71064,	0xfb6e20c8,	0x06d930ac,
    0xf6dc4190,	0x0b6b51f4,	0x0db26158,	0xf005713c,
    0xedb88320,	0x100f9344,	0x16d6a3e8,	0xeb61b38c,
    0x1b64c2b0,	0xe6d3d2d4,	0xe00ae278,	0x1dbdf21c
};

int compute_crc(int old_crc, const char* s, int len)
{
    int i;
 
    for (i = 0; i < len; i++)
    {
        /* XOR in the data. */
        old_crc ^= s[i];
 
        /* Perform the XORing for each nybble */
        old_crc = (old_crc >> 4) ^ crc_table[old_crc & 0x0f];
        old_crc = (old_crc >> 4) ^ crc_table[old_crc & 0x0f];
    }
 
    return old_crc;
}
