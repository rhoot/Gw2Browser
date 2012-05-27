// File:    AtexAsm.h
// Author:  Xtridence
// Origin:  http://forum.xentax.com/viewtopic.php?f=10&t=8499&start=120

#pragma once

#ifndef IMPORTED_ATEXASM_H_INCLUDED
#define IMPORTED_ATEXASM_H_INCLUDED

struct SImageDescriptor
{
	int xres,yres;
	const unsigned char *Data;
	int a;
	int b;
	unsigned char *image;
	int imageformat;
	int c;
};

int DecompressAtex(int a, int b, int imageformat, int d, int e,int f, int g);
bool AtexDecompress(const unsigned int *input, unsigned int unknown, unsigned int imageformat, SImageDescriptor ImageDescriptor, unsigned int *output);

#endif // IMPORTED_ATEXASM_H_INCLUDED
