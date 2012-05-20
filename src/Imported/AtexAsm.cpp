// File:    AtexAsm.h
// Author:  Xtridence
// Origin:  http://forum.xentax.com/viewtopic.php?f=10&t=8499&start=120

#include "stdafx.h"
#include <cstdio>
#include "AtexAsm.h"

unsigned int ImageFormats[]={ 0x0B2,0x12,0x0B2,0x72,0x12,0x12,0x12,0x100,0x1A4,0x1A4,0x1A4,0x104,0x0A2,0x78,0x400,0x71,0x0B1,0x0B1,0x0B1,0x0B1,0x0A1,0x11,0x201 };

unsigned char byte_79053C[]={0x6,0x10,0x6,0x0F,0x6,0x0E,0x6,0x0D,0x6,0x0C,0x6,0x0B,0x6,0x0A,0x6,0x9,0x6,0x8,0x6,0x7,0x6,0x6,0x6,0x5,0x6,0x4,0x6,0x3,0x6,0x2,0x6,0x1,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0};
unsigned char byte_79053D[]={0x10,0x6,0x0F,0x6,0x0E,0x6,0x0D,0x6,0x0C,0x6,0x0B,0x6,0x0A,0x6,0x9,0x6,0x8,0x6,0x7,0x6,0x6,0x6,0x5,0x6,0x4,0x6,0x3,0x6,0x2,0x6,0x1,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x2,0x11,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0,0x1,0x0};

int ImgFmt(unsigned int Format)
{
	if (Format >= ArraySize(ImageFormats)) {
		return -1;
	}
	return ImageFormats[Format];
}

#pragma warning(push)
#pragma warning(disable: 4102)  // unreferenced label

void NakedCall AtexSubCode6()
{
	InlineAsm{

push	ebp
mov	ebp, esp
sub	esp, 50h
mov	[ebp+-0x8], ecx
mov	[ebp+-0x4], edx
xor	ecx, ecx
push	ebx
mov	cl, byte ptr [ebp+-0x4+2]
mov	eax, edx
push	esi
push	edi
and	eax, 0FFh
xor	ebx, ebx
mov	edi, ecx
mov	ecx, eax
mov	bl, dh
mov	edx, eax
shr	ecx, 5
sub	edx, ecx
mov	ecx, ebx
shr	ecx, 6
mov	esi, ebx
mov	[ebp+-0x3C], edi
sub	esi, ecx
mov	ecx, edi
shr	ecx, 5
shr	edx, 3
sub	edi, ecx
mov	ecx, edx
shr	ecx, 2
shr	esi, 2
lea	ecx, [ecx+edx*8]
mov	[ebp+-0x14], edx
mov	[ebp+-0x20], ecx
mov	ecx, esi
shr	ecx, 4
shr	edi, 3
lea	ecx, [ecx+esi*4]
mov	[ebp+-0x10], esi
mov	[ebp+-0x1C], ecx
mov	ecx, edi
shr	ecx, 2
inc	edx
mov	[ebp+-0xC], edi
lea	ecx, [ecx+edi*8]
lea	eax, [eax+eax*2]
mov	[ebp+-0x18], ecx
mov	ecx, edx
shr	ecx, 2
inc	esi
lea	ecx, [ecx+edx*8]
mov	edx, esi
shr	edx, 4
inc	edi
lea	edx, [edx+esi*4]
mov	esi, edi
shr	esi, 2
shl	eax, 2
lea	esi, [esi+edi*8]
mov	edi, [ebp+-0x20]
sub	ecx, edi
mov	[ebp+-0x38], ecx
mov	ecx, [ebp+-0x1C]
sub	edx, ecx
mov	ecx, [ebp+-0x18]
mov	[ebp+-0x34], edx
lea	edx, [edi+edi*2]
shl	edx, 2
sub	eax, edx
xor	edx, edx
div	[ebp+-0x38]
sub	esi, ecx
mov	[ebp+-0x2C], eax
mov	eax, [ebp+-0x1C]
lea	eax, [eax+eax*2]
shl	eax, 2
mov	edx, eax
lea	eax, [ebx+ebx*2]
shl	eax, 2
sub	eax, edx
xor	edx, edx
div	[ebp+-0x34]
mov	[ebp+-0x28], eax
lea	eax, [ecx+ecx*2]
shl	eax, 2
mov	ecx, eax
mov	eax, [ebp+-0x3C]
xor	edx, edx
xor	edi, edi
lea	eax, [eax+eax*2]
shl	eax, 2
sub	eax, ecx
div	esi
xor	ecx, ecx
mov	[ebp+-0x24], eax
lea	eax, [ebp+-0x50]

loc_62A842:				//; CODE XREF: AtexSubCode6+134j
							mov	edx, [ebp+ecx+-0x2C]
							cmp	edx, 2
								jnb	short loc_62A851
								mov	edx, [ebp+ecx+-0x14]
								jmp	short loc_62A876
									; ---------------------------------------------------------------------------

loc_62A851:				//; CODE XREF: AtexSubCode6+F9j
						cmp	edx, 6
						jnb	short loc_62A862
						mov	edx, [ebp+ecx+-0x14]
						mov	[eax], edx
							inc	edx
							mov	[eax+4], edx
							jmp	short loc_62A87B
							; ---------------------------------------------------------------------------

loc_62A862:				//; CODE XREF: AtexSubCode6+104j
						cmp	edx, 0Ah
						mov	edx, [ebp+ecx+-0x14]
						jnb	short loc_62A875
							lea	esi, [edx+1]
							mov	[eax], esi
								mov	[eax+4], edx
								jmp	short loc_62A87B
								; ---------------------------------------------------------------------------

loc_62A875:				//; CODE XREF: AtexSubCode6+119j
						inc	edx

loc_62A876:				//; CODE XREF: AtexSubCode6+FFj
						mov	[eax+4], edx
						mov	[eax], edx

loc_62A87B:				//; CODE XREF: AtexSubCode6+110j
						; AtexSubCode6+123j
						add	ecx, 4
						add	eax, 8
						cmp	ecx, 0Ch
						jb	short loc_62A842
						mov	esi, [ebp+-0x40]
						mov	ebx, [ebp+-0x48]
						mov	ecx, [ebp+-0x50]
						mov	eax, [ebp+-0x4C]
						shl	esi, 6
							or	esi, ebx
							mov	ebx, [ebp+-0x44]
							shl	esi, 5
								or	esi, ecx
								mov	ecx, [ebp+-0x3C]
								shl	ecx, 6
									or	ecx, ebx
									mov	[ebp+-0x4], edi
									shl	ecx, 5
									or	ecx, eax
									lea	ebx, [ebp+-0x50]
									xor	eax, eax

loc_62A8B4:				//; CODE XREF: AtexSubCode6+192j
						mov	edx, [ebx]
						cmp	edx, [ebx+4]
						jz	short loc_62A8D9
							cmp	edx, [ebp+eax+-0x14]
							jnz	short loc_62A8C7
								mov	edx, [ebp+eax+-0x2C]
								jmp	short loc_62A8D0
									; ---------------------------------------------------------------------------

loc_62A8C7:				//; CODE XREF: AtexSubCode6+16Fj
						mov	edx, 0Ch
						sub	edx, [ebp+eax+-0x2C]

loc_62A8D0:				//; CODE XREF: AtexSubCode6+175j
							add	edi, edx
							mov	edx, [ebp+-0x4]
							inc	edx
								mov	[ebp+-0x4], edx

loc_62A8D9:				//; CODE XREF: AtexSubCode6+169j
						add	eax, 4
						add	ebx, 8
						cmp	eax, 0Ch
						jb	short loc_62A8B4
						mov	eax, [ebp+-0x4]
						test	eax, eax
							jz	short loc_62A8F6
							shr	eax, 1
							add	eax, edi
							xor	edx, edx
							div	[ebp+-0x4]
							mov	edi, eax

loc_62A8F6:				//; CODE XREF: AtexSubCode6+199j
						mov	eax, [ebp+0x8]
						test	eax, eax
							jz	short loc_62A915
							cmp	edi, 5
							jz	short loc_62A90E
							cmp	edi, 6
							jz	short loc_62A90E
							mov	eax, [ebp+-0x4]
							test	eax, eax
								jnz	short loc_62A915

loc_62A90E:				//; CODE XREF: AtexSubCode6+1B0j
						; AtexSubCode6+1B5j
						mov	edx, 1
						jmp	short loc_62A917
						; ---------------------------------------------------------------------------

loc_62A915:				//; CODE XREF: AtexSubCode6+1ABj
						; AtexSubCode6+1BCj
						xor	edx, edx

loc_62A917:				//; CODE XREF: AtexSubCode6+1C3j
						mov	eax, [ebp+-0x4]
						test	eax, eax
							jnz	short loc_62A935
							test	edx, edx
							jnz	short loc_62A935
							cmp	ecx, 0FFFFh
							jz	short loc_62A92F
							xor	edi, edi
							inc	ecx
							jmp	short loc_62A935
							; ---------------------------------------------------------------------------

loc_62A92F:				//; CODE XREF: AtexSubCode6+1D8j
						mov	edi, 0Ch
						dec	esi

loc_62A935:				//; CODE XREF: AtexSubCode6+1CCj
						; AtexSubCode6+1D0j ...
						cmp	ecx, esi
						sbb	eax, eax
						inc	eax
						cmp	edx, eax
						jz	short loc_62A94D
						mov	eax, esi
						mov	esi, ecx
						mov	ecx, eax
						mov	eax, 0Ch
						sub	eax, edi
						mov	edi, eax

loc_62A94D:				//; CODE XREF: AtexSubCode6+1ECj
						test	edx, edx
						jz	short loc_62A958
						mov	eax, 2
						jmp	short loc_62A976
						; ---------------------------------------------------------------------------

loc_62A958:				//; CODE XREF: AtexSubCode6+1FFj
						cmp	edi, 2
						jnb	short loc_62A961
						xor	eax, eax
						jmp	short loc_62A976
						; ---------------------------------------------------------------------------

loc_62A961:				//; CODE XREF: AtexSubCode6+20Bj
						cmp	edi, 6
						jnb	short loc_62A96D
						mov	eax, 2
						jmp	short loc_62A976
						; ---------------------------------------------------------------------------

loc_62A96D:				//; CODE XREF: AtexSubCode6+214j
						cmp	edi, 0Ah
						sbb	eax, eax
						and	eax, 2
						inc	eax

loc_62A976:				//; CODE XREF: AtexSubCode6+206j
						; AtexSubCode6+20Fj ...
						mov	edx, [ebp+-0x8]
						pop	edi
							shl	ecx, 10h
							or	ecx, esi
							pop	esi
							mov	[edx], ecx
							lea	ecx, ds:0[eax*4]
							or	ecx, eax
								pop	ebx
								mov	eax, ecx
								shl	eax, 4
								or	eax, ecx
								mov	ecx, eax
								shl	ecx, 8
								or	ecx, eax
								mov	eax, ecx
								shl	eax, 10h
								or	eax, ecx
								mov	[edx+4], eax
								mov	esp, ebp
								pop	ebp
								retn	4
}
}

void NakedCall AtexSubCode3()
{
	InlineAsm{

				push	ebp
				mov	ebp, esp
				sub	esp, 2Ch
				push	ebx
				push	esi
				mov	esi, [ebp+0x0c]
				mov	[ebp+-0x4], ecx
				push	edi
				mov	[ebp+-0x14], edx
				mov	ecx, [esi+10h]
				mov	eax, [esi+0Ch]
				mov	edi, ecx
				mov	edx, eax
				shr	edi, 1Ch
				shl	eax, 4
				or	edi, eax
				mov	eax, [esi+8]
				shr	edx, 1Ch
				cmp	eax, 4
				mov	[esi+0Ch], edi
				jb	short loc_60F932
				shl	ecx, 4
				add	eax, 0FFFFFFFCh
				mov	[esi+10h], ecx
				mov	[esi+8], eax
				xor	ebx, ebx
				jmp	short loc_60F975
				; ---------------------------------------------------------------------------

				loc_60F932:				//; CODE XREF: AtexSubCode3+30j
				mov	ecx, [esi]
				mov	edi, [esi+4]
				cmp	ecx, edi
				jz	short loc_60F96D
				mov	edi, [ecx]
				add	ecx, 4
				mov	[esi], ecx
				lea	ecx, [eax+1Ch]
				mov	ebx, edi
				mov	[ebp+0x0c], ecx
				shr	ebx, cl
				mov	ecx, [esi+0Ch]
				mov	[esi+10h], edi
				or	ecx, ebx
				mov	[esi+0Ch], ecx
				mov	ecx, 4
				sub	ecx, eax
				mov	eax, [ebp+0x0c]
				shl	edi, cl
				mov	[esi+8], eax
				xor	ebx, ebx
				mov	[esi+10h], edi
				jmp	short loc_60F975
				; ---------------------------------------------------------------------------

				loc_60F96D:				//; CODE XREF: AtexSubCode3+49j
				xor	ebx, ebx
				mov	[esi+10h], ebx
				mov	[esi+8], ebx

				loc_60F975:				//; CODE XREF: AtexSubCode3+40j
				; AtexSubCode3+7Bj
				mov	eax, edx
				mov	[ebp+-0x24], ebx
				shl	eax, 4
				or	eax, edx
				mov	[ebp+-0x20], ebx
				mov	ecx, eax
				shl	ecx, 8
				or	eax, ecx
				mov	edx, eax
				shl	edx, 10h
				or	eax, edx
				mov	[ebp+-0x1C], eax
				mov	[ebp+-0x18], eax
				cmp	[ebp+0x10], ebx
				jz	loc_60FBDE

				loc_60F99F:				//; CODE XREF: AtexSubCode3+2E8j
				mov	eax, [esi+0Ch]
				xor	ecx, ecx
				shr	eax, 1Ah
				mov	cl, ds:byte_79053D[eax*2]
				inc	ecx
				mov	[ebp+0x0c], ecx
				xor	ecx, ecx
				mov	cl, ds:byte_79053C[eax*2]
				mov	edi, ecx
				cmp	edi, 20h
				jb	short loc_60F9D3
				//push	3Ch
				//mov	edx, offset aPCodeBaseCompr ; "P:\\Code\\Base\\compress/CmpIo.h"
				//mov	ecx, offset unk_838224
				//call	near ptr ImageError

				loc_60F9D3:				//; CODE XREF: AtexSubCode3+D0j
				test	edi, edi
				jz	short loc_60F9EF
				mov	edx, [esi+10h]
				mov	eax, [esi+0Ch]
				mov	ecx, 20h
				sub	ecx, edi
				shr	edx, cl
				mov	ecx, edi
				shl	eax, cl
				or	edx, eax
				mov	[esi+0Ch], edx

				loc_60F9EF:				//; CODE XREF: AtexSubCode3+E5j
				mov	eax, [esi+8]
				cmp	edi, eax
				mov	[ebp+-0xC], eax
				ja	short loc_60FA07
				mov	edx, [esi+10h]
				mov	ecx, edi
				shl	edx, cl
				sub	eax, edi
				mov	[esi+10h], edx
				jmp	short loc_60FA42
				; ---------------------------------------------------------------------------

				loc_60FA07:				//; CODE XREF: AtexSubCode3+107j
				mov	ecx, [esi]
				mov	edx, [esi+4]
				cmp	ecx, edx
				jz	short loc_60FA3D
				mov	edx, [ecx]
				add	ecx, 4
				sub	eax, edi
				mov	[esi], ecx
				mov	[esi+10h], edx
				lea	ecx, [eax+20h]
				mov	eax, edx
				shr	edx, cl
				mov	[ebp+-0x8], ecx
				mov	ecx, [esi+0Ch]
				or	ecx, edx
				mov	[esi+0Ch], ecx
				mov	ecx, edi
				sub	ecx, [ebp+-0xC]
				shl	eax, cl
				mov	[esi+10h], eax
				mov	eax, [ebp+-0x8]
				jmp	short loc_60FA42
				; ---------------------------------------------------------------------------

				loc_60FA3D:				//; CODE XREF: AtexSubCode3+11Ej
				xor	eax, eax
				mov	[esi+10h], eax

				loc_60FA42:				//; CODE XREF: AtexSubCode3+115j
				; AtexSubCode3+14Bj
				mov	ecx, [esi+10h]
				mov	[esi+8], eax
				mov	eax, [esi+0Ch]
				mov	edi, ecx
				mov	edx, eax
				add	eax, eax
				shr	edi, 1Fh
				or	edi, eax
				mov	[esi+0Ch], edi
				mov	edi, [esi+8]
				shr	edx, 1Fh
				cmp	edi, 1
				mov	[ebp+-0xC], edx
				jb	short loc_60FA72
				add	ecx, ecx
				dec	edi
				mov	[esi+10h], ecx
				mov	[esi+8], edi
				jmp	short loc_60FAB6
				; ---------------------------------------------------------------------------

				loc_60FA72:				//; CODE XREF: AtexSubCode3+175j
				mov	eax, [esi]
				mov	ecx, [esi+4]
				cmp	eax, ecx
				jz	short loc_60FAAE
				mov	edx, [eax]
				add	eax, 4
				mov	[esi], eax
				lea	ecx, [edi+1Fh]
				mov	[esi+10h], edx
				mov	eax, edx
				shr	edx, cl
				mov	[ebp+-0x8], ecx
				mov	ecx, [esi+0Ch]
				or	ecx, edx
				mov	edx, [ebp+-0xC]
				mov	[esi+0Ch], ecx
				mov	ecx, 1
				sub	ecx, edi
				shl	eax, cl
				mov	[esi+10h], eax
				mov	eax, [ebp+-0x8]
				mov	[esi+8], eax
				jmp	short loc_60FAB6
				; ---------------------------------------------------------------------------

				loc_60FAAE:				//; CODE XREF: AtexSubCode3+189j
				xor	eax, eax
				mov	[esi+10h], eax
				mov	[esi+8], eax

				loc_60FAB6:				//; CODE XREF: AtexSubCode3+180j
				; AtexSubCode3+1BCj
				mov	edi, edx
				test	edx, edx
				mov	[ebp+-0x8], edi
				jz	short loc_60FB3B
				mov	eax, [esi+0Ch]
				mov	edi, [esi+10h]
				mov	ecx, eax
				add	eax, eax
				shr	edi, 1Fh
				or	edi, eax
				mov	[esi+0Ch], edi
				mov	edi, [esi+8]
				shr	ecx, 1Fh
				cmp	edi, 1
				mov	[ebp+-0x10], ecx
				jb	short loc_60FAED
				mov	eax, [esi+10h]
				add	eax, eax
				dec	edi
				mov	[esi+10h], eax
				mov	[esi+8], edi
				jmp	short loc_60FB35
				; ---------------------------------------------------------------------------

				loc_60FAED:				//; CODE XREF: AtexSubCode3+1EDj
				mov	eax, [esi]
				cmp	eax, [esi+4]
				jz	short loc_60FB2D
				mov	ecx, [eax]
				add	eax, 4
				mov	[esi+10h], ecx
				mov	[esi], eax
				mov	eax, [esi+10h]
				lea	ecx, [edi+1Fh]
				mov	edx, eax
				mov	[ebp+-0x8], ecx
				shr	edx, cl
				mov	ecx, [esi+0Ch]
				or	ecx, edx
				mov	edx, [ebp+-0xC]
				mov	[esi+0Ch], ecx
				mov	ecx, 1
				sub	ecx, edi
				shl	eax, cl
				mov	ecx, [ebp+-0x10]
				mov	[esi+10h], eax
				mov	eax, [ebp+-0x8]
				mov	[esi+8], eax
				jmp	short loc_60FB35
				; ---------------------------------------------------------------------------

				loc_60FB2D:				//; CODE XREF: AtexSubCode3+202j
				xor	eax, eax
				mov	[esi+10h], eax
				mov	[esi+8], eax

				loc_60FB35:				//; CODE XREF: AtexSubCode3+1FBj
				; AtexSubCode3+23Bj
				lea	edi, [ecx+edx]
				mov	[ebp+-0x8], edi

				loc_60FB3B:				//; CODE XREF: AtexSubCode3+1CDj
				mov	eax, [ebp+0x0c]
				test	eax, eax
				jz	short loc_60FBA3

				loc_60FB42:				//; CODE XREF: AtexSubCode3+2B1j
				cmp	ebx, [ebp+0x10]
				jz	loc_60FBDE
				mov	ecx, ebx
				mov	edx, 1
				and	ecx, 1Fh
				mov	eax, ebx
				shl	edx, cl
				mov	ecx, [ebp+0x08]
				shr	eax, 5
				shl	eax, 2
				test	[eax+ecx], edx
				jnz	short loc_60FB8F
				test	edi, edi
				jz	short loc_60FB8C
				mov	ecx, [ebp+-0x4]
				mov	edi, [ebp+edi*8+-0x2C]
				mov	[ecx], edi
				mov	edi, [ebp+-0x8]
				mov	edi, [ebp+edi*8+-0x28]
				mov	[ecx+4], edi
				mov	ecx, [ebp+-0x14]
				mov	edi, [eax+ecx]
				or	edi, edx
				mov	[eax+ecx], edi
				mov	edi, [ebp+-0x8]

				loc_60FB8C:				//; CODE XREF: AtexSubCode3+279j
				dec	[ebp+0x0c]

				loc_60FB8F:				//; CODE XREF: AtexSubCode3+275j
				mov	edx, [ebp+0x14]
				mov	eax, [ebp+-0x4]
				inc	ebx
				lea	ecx, [eax+edx*4]
				mov	eax, [ebp+0x0c]
				test	eax, eax
				mov	[ebp+-0x4], ecx
				jnz	short loc_60FB42

				loc_60FBA3:				//; CODE XREF: AtexSubCode3+250j
				mov	eax, [ebp+0x10]
				cmp	ebx, eax
				jz	short loc_60FBDE

				loc_60FBAA:				//; CODE XREF: AtexSubCode3+2E4j
				mov	edi, [ebp+0x08]
				mov	ecx, ebx
				and	ecx, 1Fh
				mov	edx, 1
				shl	edx, cl
				mov	ecx, ebx
				shr	ecx, 5
				test	[edi+ecx*4], edx
				jz	short loc_60FBD6
				mov	edx, [ebp+0x14]
				mov	ecx, [ebp+-0x4]
				inc	ebx
				lea	edx, [ecx+edx*4]
				cmp	ebx, eax
				mov	[ebp+-0x4], edx
				jz	short loc_60FBDE
				jmp	short loc_60FBAA
				; ---------------------------------------------------------------------------

				loc_60FBD6:				//; CODE XREF: AtexSubCode3+2D1j
				cmp	ebx, eax
				jnz	loc_60F99F

				loc_60FBDE:				//; CODE XREF: AtexSubCode3+A9j
				; AtexSubCode3+255j ...
				pop	edi
				pop	esi
				pop	ebx
				mov	esp, ebp
				pop	ebp
				retn	10h
			}
}

void AtexSubCode3_(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, unsigned int f)
{
	InlineAsm
	{
		mov ecx, a
		mov edx, b
		push f
		push e
		push d
		push c
		call AtexSubCode3
	}
}
void NakedCall AtexSubCode4()
{
	InlineAsm{

push	ebp
mov	ebp, esp
sub	esp, 2Ch
push	ebx
push	esi
mov	esi, [ebp+0x0c]
mov	[ebp+-0x4], ecx
mov	[ebp+-0x14], edx
push	edi
mov	ecx, [esi+10h]
mov	eax, [esi+0Ch]
mov	edx, ecx
mov	edi, eax
shr	edx, 18h
shl	eax, 8
or	edx, eax
mov	eax, [esi+8]
shr	edi, 18h
cmp	eax, 8
mov	[esi+0Ch], edx
jb	short loc_60FC2D
shl	ecx, 8
mov	[esi+10h], ecx
add	eax, 0FFFFFFF8h
jmp	short loc_60FC68
; ---------------------------------------------------------------------------

loc_60FC2D:				//; CODE XREF: AtexSubCode4+30j
mov	ecx, [esi]
mov	edx, [esi+4]
cmp	ecx, edx
jz	short loc_60FC63
mov	edx, [ecx]
add	ecx, 4
mov	[esi], ecx
lea	ecx, [eax+18h]
mov	ebx, edx
mov	[ebp+0x0c], ecx
shr	ebx, cl
mov	ecx, [esi+0Ch]
mov	[esi+10h], edx
or	ecx, ebx
mov	[esi+0Ch], ecx
mov	ecx, 8
sub	ecx, eax
mov	eax, [ebp+0x0c]
shl	edx, cl
mov	[esi+10h], edx
jmp	short loc_60FC68
; ---------------------------------------------------------------------------

loc_60FC63:				//; CODE XREF: AtexSubCode4+44j
xor	eax, eax
mov	[esi+10h], eax

loc_60FC68:				//; CODE XREF: AtexSubCode4+3Bj
; AtexSubCode4+71j
mov	ecx, edi
mov	[esi+8], eax
mov	eax, [ebp+0x10]
xor	ebx, ebx
shl	ecx, 8
or	ecx, edi
cmp	eax, ebx
mov	[ebp+-0x24], ebx
mov	[ebp+-0x20], ebx
mov	[ebp+-0x1C], ecx
mov	[ebp+-0x18], ebx
jz	loc_60FECA

loc_60FC8B:				//; CODE XREF: AtexSubCode4+2D4j
mov	eax, [esi+0Ch]
xor	edx, edx
shr	eax, 1Ah
xor	ecx, ecx
mov	dl, ds:byte_79053C[eax*2]
mov	cl, ds:byte_79053D[eax*2]
mov	edi, edx
inc	ecx
cmp	edi, 20h
mov	[ebp+0x0c], ecx
jb	short loc_60FCBF
//push	3Ch
//mov	edx, offset aPCodeBaseCompr ; "P:\\Code\\Base\\compress/CmpIo.h"
//mov	ecx, offset unk_838224
//call	near ptr ImageError

loc_60FCBF:				//; CODE XREF: AtexSubCode4+BCj
test	edi, edi
jz	short loc_60FCDB
mov	eax, [esi+10h]
mov	edx, [esi+0Ch]
mov	ecx, 20h
sub	ecx, edi
shr	eax, cl
mov	ecx, edi
shl	edx, cl
or	eax, edx
mov	[esi+0Ch], eax

loc_60FCDB:				//; CODE XREF: AtexSubCode4+D1j
mov	eax, [esi+8]
cmp	edi, eax
mov	[ebp+-0xC], eax
ja	short loc_60FCF3
mov	edx, [esi+10h]
mov	ecx, edi
shl	edx, cl
sub	eax, edi
mov	[esi+10h], edx
jmp	short loc_60FD2E
; ---------------------------------------------------------------------------

loc_60FCF3:				//; CODE XREF: AtexSubCode4+F3j
mov	ecx, [esi]
mov	edx, [esi+4]
cmp	ecx, edx
jz	short loc_60FD29
mov	edx, [ecx]
add	ecx, 4
sub	eax, edi
mov	[esi], ecx
mov	[esi+10h], edx
lea	ecx, [eax+20h]
mov	eax, edx
shr	edx, cl
mov	[ebp+-0x8], ecx
mov	ecx, [esi+0Ch]
or	ecx, edx
mov	[esi+0Ch], ecx
mov	ecx, edi
sub	ecx, [ebp+-0xC]
shl	eax, cl
mov	[esi+10h], eax
mov	eax, [ebp+-0x8]
jmp	short loc_60FD2E
; ---------------------------------------------------------------------------

loc_60FD29:				//; CODE XREF: AtexSubCode4+10Aj
xor	eax, eax
mov	[esi+10h], eax

loc_60FD2E:				//; CODE XREF: AtexSubCode4+101j
; AtexSubCode4+137j
mov	ecx, [esi+10h]
mov	[esi+8], eax
mov	eax, [esi+0Ch]
mov	edi, ecx
mov	edx, eax
add	eax, eax
shr	edi, 1Fh
or	edi, eax
mov	[esi+0Ch], edi
mov	edi, [esi+8]
shr	edx, 1Fh
cmp	edi, 1
mov	[ebp+-0xC], edx
jb	short loc_60FD5E
add	ecx, ecx
dec	edi
mov	[esi+10h], ecx
mov	[esi+8], edi
jmp	short loc_60FDA2
; ---------------------------------------------------------------------------

loc_60FD5E:				//; CODE XREF: AtexSubCode4+161j
mov	eax, [esi]
mov	ecx, [esi+4]
cmp	eax, ecx
jz	short loc_60FD9A
mov	edx, [eax]
add	eax, 4
mov	[esi], eax
lea	ecx, [edi+1Fh]
mov	[esi+10h], edx
mov	eax, edx
shr	edx, cl
mov	[ebp+-0x8], ecx
mov	ecx, [esi+0Ch]
or	ecx, edx
mov	edx, [ebp+-0xC]
mov	[esi+0Ch], ecx
mov	ecx, 1
sub	ecx, edi
shl	eax, cl
mov	[esi+10h], eax
mov	eax, [ebp+-0x8]
mov	[esi+8], eax
jmp	short loc_60FDA2
; ---------------------------------------------------------------------------

loc_60FD9A:				//; CODE XREF: AtexSubCode4+175j
xor	eax, eax
mov	[esi+10h], eax
mov	[esi+8], eax

loc_60FDA2:				//; CODE XREF: AtexSubCode4+16Cj
; AtexSubCode4+1A8j
mov	edi, edx
test	edx, edx
mov	[ebp+-0x8], edi
jz	short loc_60FE27
mov	eax, [esi+0Ch]
mov	edi, [esi+10h]
mov	ecx, eax
add	eax, eax
shr	edi, 1Fh
or	edi, eax
mov	[esi+0Ch], edi
mov	edi, [esi+8]
shr	ecx, 1Fh
cmp	edi, 1
mov	[ebp+-0x10], ecx
jb	short loc_60FDD9
mov	eax, [esi+10h]
add	eax, eax
dec	edi
mov	[esi+10h], eax
mov	[esi+8], edi
jmp	short loc_60FE21
; ---------------------------------------------------------------------------

loc_60FDD9:				//; CODE XREF: AtexSubCode4+1D9j
mov	eax, [esi]
cmp	eax, [esi+4]
jz	short loc_60FE19
mov	ecx, [eax]
add	eax, 4
mov	[esi+10h], ecx
mov	[esi], eax
mov	eax, [esi+10h]
lea	ecx, [edi+1Fh]
mov	edx, eax
mov	[ebp+-0x8], ecx
shr	edx, cl
mov	ecx, [esi+0Ch]
or	ecx, edx
mov	edx, [ebp+-0xC]
mov	[esi+0Ch], ecx
mov	ecx, 1
sub	ecx, edi
shl	eax, cl
mov	ecx, [ebp+-0x10]
mov	[esi+10h], eax
mov	eax, [ebp+-0x8]
mov	[esi+8], eax
jmp	short loc_60FE21
; ---------------------------------------------------------------------------

loc_60FE19:				//; CODE XREF: AtexSubCode4+1EEj
xor	eax, eax
mov	[esi+10h], eax
mov	[esi+8], eax

loc_60FE21:				//; CODE XREF: AtexSubCode4+1E7j
; AtexSubCode4+227j
lea	edi, [ecx+edx]
mov	[ebp+-0x8], edi

loc_60FE27:				//; CODE XREF: AtexSubCode4+1B9j
mov	eax, [ebp+0x0c]
test	eax, eax
jz	short loc_60FE8F

loc_60FE2E:				//; CODE XREF: AtexSubCode4+29Dj
cmp	ebx, [ebp+0x10]
jz	loc_60FECA
mov	ecx, ebx
mov	edx, 1
and	ecx, 1Fh
mov	eax, ebx
shl	edx, cl
mov	ecx, [ebp+0x08]
shr	eax, 5
shl	eax, 2
test	[eax+ecx], edx
jnz	short loc_60FE7B
test	edi, edi
jz	short loc_60FE78
mov	ecx, [ebp+-0x4]
mov	edi, [ebp+edi*8+-0x2C]
mov	[ecx], edi
mov	edi, [ebp+-0x8]
mov	edi, [ebp+edi*8+-0x28]
mov	[ecx+4], edi
mov	ecx, [ebp+-0x14]
mov	edi, [eax+ecx]
or	edi, edx
mov	[eax+ecx], edi
mov	edi, [ebp+-0x8]

loc_60FE78:				//; CODE XREF: AtexSubCode4+265j
dec	[ebp+0x0c]

loc_60FE7B:				//; CODE XREF: AtexSubCode4+261j
mov	edx, [ebp+0x14]
mov	eax, [ebp+-0x4]
inc	ebx
lea	ecx, [eax+edx*4]
mov	eax, [ebp+0x0c]
test	eax, eax
mov	[ebp+-0x4], ecx
jnz	short loc_60FE2E

loc_60FE8F:				//; CODE XREF: AtexSubCode4+23Cj
mov	eax, [ebp+0x10]
cmp	ebx, eax
jz	short loc_60FECA

loc_60FE96:				//; CODE XREF: AtexSubCode4+2D0j
mov	edi, [ebp+0x08]
mov	ecx, ebx
and	ecx, 1Fh
mov	edx, 1
shl	edx, cl
mov	ecx, ebx
shr	ecx, 5
test	[edi+ecx*4], edx
jz	short loc_60FEC2
mov	edx, [ebp+0x14]
mov	ecx, [ebp+-0x4]
inc	ebx
lea	edx, [ecx+edx*4]
cmp	ebx, eax
mov	[ebp+-0x4], edx
jz	short loc_60FECA
jmp	short loc_60FE96
; ---------------------------------------------------------------------------

loc_60FEC2:				//; CODE XREF: AtexSubCode4+2BDj
cmp	ebx, eax
jnz	loc_60FC8B

loc_60FECA:				//; CODE XREF: AtexSubCode4+95j
; AtexSubCode4+241j ...
pop	edi
pop	esi
pop	ebx
mov	esp, ebp
pop	ebp
retn	10h
}
}

void AtexSubCode4_(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, unsigned int f)
{
	InlineAsm
	{
		mov ecx, a
		mov edx, b
		push f
		push e
		push d
		push c
		call AtexSubCode4
	}
}
void NakedCall AtexSubCode5()
{
	InlineAsm{

push	ebp
mov	ebp, esp
sub	esp, 10h
push	ebx
push	esi
mov	esi, [ebp+0x0c]
mov	[ebp+-0x4], ecx
push	edi
mov	ecx, [esi+10h]
mov	eax, [esi+0Ch]
mov	edi, ecx
mov	edx, eax
shr	edi, 8
shl	eax, 18h
or	edi, eax
mov	[esi+0Ch], edi
mov	edi, [esi+8]
shr	edx, 8
cmp	edi, 18h
jb	short loc_60FF1D
shl	ecx, 18h
add	edi, 0FFFFFFE8h
mov	[esi+10h], ecx
mov	[esi+8], edi
jmp	short loc_60FF5E
; ---------------------------------------------------------------------------

loc_60FF1D:				//; CODE XREF: AtexSubCode5+2Dj
mov	eax, [esi]
mov	ecx, [esi+4]
cmp	eax, ecx
jz	short loc_60FF56
mov	ecx, [eax]
add	eax, 4
mov	[esi+10h], ecx
mov	[esi], eax
mov	eax, [esi+10h]
lea	ecx, [edi+8]
mov	ebx, eax
mov	[ebp+0x0c], ecx
shr	ebx, cl
mov	ecx, [esi+0Ch]
or	ecx, ebx
mov	[esi+0Ch], ecx
mov	ecx, 18h
sub	ecx, edi
shl	eax, cl
mov	[esi+10h], eax
mov	eax, [ebp+0x0c]
jmp	short loc_60FF5B
; ---------------------------------------------------------------------------

loc_60FF56:				//; CODE XREF: AtexSubCode5+44j
xor	eax, eax
mov	[esi+10h], eax

loc_60FF5B:				//; CODE XREF: AtexSubCode5+74j
mov	[esi+8], eax

loc_60FF5E:				//; CODE XREF: AtexSubCode5+3Bj
mov	ecx, [ebp+0x18]
or	edx, 0FF000000h
push	ecx
lea	ecx, [ebp+-0x10]
call	AtexSubCode6
mov	eax, [ebp+0x10]
xor	edi, edi
test	eax, eax
jz	loc_610133

loc_60FF7D:				//; CODE XREF: AtexSubCode5+24Dj
mov	eax, [esi+0Ch]
xor	ecx, ecx
shr	eax, 1Ah
xor	ebx, ebx
mov	cl, ds:byte_79053D[eax*2]
mov	bl, ds:byte_79053C[eax*2]
inc	ecx
cmp	ebx, 20h
mov	[ebp+0x0c], ecx
jb	short loc_60FFAF
//push	3Ch
//mov	edx, offset aPCodeBaseCompr ; "P:\\Code\\Base\\compress/CmpIo.h"
//mov	ecx, offset unk_838224
//call	near ptr ImageError

loc_60FFAF:				//; CODE XREF: AtexSubCode5+BCj
test	ebx, ebx
jz	short loc_60FFCB
mov	edx, [esi+10h]
mov	eax, [esi+0Ch]
mov	ecx, 20h
sub	ecx, ebx
shr	edx, cl
mov	ecx, ebx
shl	eax, cl
or	edx, eax
mov	[esi+0Ch], edx

loc_60FFCB:				//; CODE XREF: AtexSubCode5+D1j
mov	eax, [esi+8]
cmp	ebx, eax
mov	[ebp+0x18], eax
ja	short loc_60FFE3
mov	edx, [esi+10h]
mov	ecx, ebx
shl	edx, cl
sub	eax, ebx
mov	[esi+10h], edx
jmp	short loc_61001E
; ---------------------------------------------------------------------------

loc_60FFE3:				//; CODE XREF: AtexSubCode5+F3j
mov	ecx, [esi]
mov	edx, [esi+4]
cmp	ecx, edx
jz	short loc_610019
mov	edx, [ecx]
add	ecx, 4
sub	eax, ebx
mov	[esi], ecx
mov	[esi+10h], edx
lea	ecx, [eax+20h]
mov	eax, edx
shr	edx, cl

loc_60FFFF:				; DATA XREF: .data:off_8242D2o
; .data:off_8242D6o
mov	[ebp+-0x8], ecx
mov	ecx, [esi+0Ch]
or	ecx, edx
mov	[esi+0Ch], ecx
mov	ecx, ebx
sub	ecx, [ebp+0x18]
shl	eax, cl
mov	[esi+10h], eax
mov	eax, [ebp+-0x8]
jmp	short loc_61001E
; ---------------------------------------------------------------------------

loc_610019:				//; CODE XREF: AtexSubCode5+10Aj
xor	eax, eax
mov	[esi+10h], eax

loc_61001E:				//; CODE XREF: AtexSubCode5+101j
; AtexSubCode5+137j
; DATA XREF: ...
mov	ecx, [esi+10h]
mov	[esi+8], eax
mov	eax, [esi+0Ch]
mov	edx, ecx
mov	ebx, eax
add	eax, eax

loc_61002D:				; DATA XREF: .rdata:0079C394o
shr	edx, 1Fh
or	edx, eax
mov	[esi+0Ch], edx
mov	edx, [esi+8]
shr	ebx, 1Fh
cmp	edx, 1
mov	[ebp+0x18], ebx

loc_610041:				; DATA XREF: .data:off_7F3C5Co
; .data:007F4D44o ...
jb	short loc_61004E

loc_610043:				; DATA XREF: .rdata:off_78B7D4o
; .data:off_7F69D4o ...
add	ecx, ecx
dec	edx

loc_610046:				; DATA XREF: .data:off_7F34A0o
; .data:off_7F6270o ...
mov	[esi+10h], ecx
mov	[esi+8], edx

loc_61004C:				; DATA XREF: .data:off_7F3178o
; .data:007F55B8o ...
jmp	short loc_610095
; ---------------------------------------------------------------------------

loc_61004E:				//; CODE XREF: AtexSubCode5:loc_610041j
; DATA XREF: .data:off_7F5578o	...
mov	eax, [esi]

loc_610050:				; DATA XREF: .rdata:off_7859E8o
; .data:off_7F46E0o ...
mov	ecx, [esi+4]

loc_610053:				; DATA XREF: .data:007F6F24o
; .data:007F8D28o ...
cmp	eax, ecx

loc_610055:				; DATA XREF: .data:007F96A8o
; .data:007FD910o
jz	short loc_61008D

loc_610057:				; DATA XREF: .rdata:007A2640o
; .data:off_8117E8o
mov	ecx, [eax]
add	eax, 4
mov	[esi+10h], ecx
mov	[esi], eax

loc_610061:				; DATA XREF: .data:007FF6D8o
; .data:007FFDBCo ...
mov	eax, [esi+10h]

loc_610064:				; DATA XREF: .data:off_824408o
; .data:00826D14o ...
lea	ecx, [edx+1Fh]

loc_610067:				; DATA XREF: .data:007F467Co
; .data:007F64B4o ...
mov	ebx, eax

loc_610069:				; DATA XREF: .data:007F62ECo
; .data:007F794Co ...
mov	[ebp+-0x8], ecx

loc_61006C:				; DATA XREF: .data:007F4FA8o
; .data:007F5554o ...
shr	ebx, cl

loc_61006E:				; DATA XREF: .rdata:007859D0o
; .data:007F7AC0o ...
mov	ecx, [esi+0Ch]

loc_610071:				; DATA XREF: .data:007F50F8o
; .data:007F7068o ...
or	ecx, ebx

loc_610073:				; DATA XREF: .data:007F9114o
; .data:0080F898o ...
mov	ebx, [ebp+0x18]

loc_610076:				; DATA XREF: .data:00809100o
; .data:0080912Co ...
mov	[esi+0Ch], ecx
mov	ecx, 1
sub	ecx, edx
mov	edx, [ebp+-0x8]
shl	eax, cl
mov	[esi+8], edx
mov	[esi+10h], eax
jmp	short loc_610095
; ---------------------------------------------------------------------------

loc_61008D:				//; CODE XREF: AtexSubCode5:loc_610055j
xor	eax, eax
mov	[esi+10h], eax
mov	[esi+8], eax

loc_610095:				//; CODE XREF: AtexSubCode5:loc_61004Cj
; AtexSubCode5+1ABj
mov	eax, [ebp+0x0c]
test	eax, eax
jz	short loc_6100F8

loc_61009C:				//; CODE XREF: AtexSubCode5+216j
cmp	edi, [ebp+0x10]
jz	loc_610133
mov	ecx, edi
mov	edx, 1
and	ecx, 1Fh
mov	eax, edi
shl	edx, cl
mov	ecx, [ebp+0x08]
shr	eax, 5
lea	eax, [ecx+eax*4]
mov	[ebp+-0x8], eax
mov	eax, [eax]
test	edx, eax
jnz	short loc_6100E4
test	ebx, ebx
jz	short loc_6100E1
mov	ecx, [ebp+-0x4]
mov	ebx, [ebp+-0x10]
or	eax, edx
mov	edx, [ebp+-0x8]
mov	[ecx], ebx
mov	ebx, [ebp+-0xC]
mov	[ecx+4], ebx
mov	ebx, [ebp+0x18]
mov	[edx], eax

loc_6100E1:				//; CODE XREF: AtexSubCode5+1E7j
dec	[ebp+0x0c]

loc_6100E4:				//; CODE XREF: AtexSubCode5+1E3j
mov	eax, [ebp+0x14]
mov	ecx, [ebp+-0x4]
inc	edi
lea	edx, [ecx+eax*4]
mov	eax, [ebp+0x0c]
test	eax, eax
mov	[ebp+-0x4], edx
jnz	short loc_61009C

loc_6100F8:				//; CODE XREF: AtexSubCode5+1BAj
mov	eax, [ebp+0x10]
cmp	edi, eax
jz	short loc_610133

loc_6100FF:				//; CODE XREF: AtexSubCode5+249j
mov	ebx, [ebp+0x08]
mov	ecx, edi
and	ecx, 1Fh
mov	edx, 1
shl	edx, cl
mov	ecx, edi
shr	ecx, 5
test	[ebx+ecx*4], edx
jz	short loc_61012B
mov	edx, [ebp+-0x4]
mov	ecx, [ebp+0x14]
inc	edi
lea	edx, [edx+ecx*4]
cmp	edi, eax
mov	[ebp+-0x4], edx
jz	short loc_610133
jmp	short loc_6100FF
; ---------------------------------------------------------------------------

loc_61012B:				//; CODE XREF: AtexSubCode5+236j
cmp	edi, eax
jnz	loc_60FF7D

loc_610133:				//; CODE XREF: AtexSubCode5+97j
; AtexSubCode5+1BFj ...
pop	edi
pop	esi
pop	ebx
mov	esp, ebp
pop	ebp
retn	14h

}
}

void AtexSubCode5_(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, unsigned int f, unsigned int g)
{
	InlineAsm
	{
		mov ecx, a
		mov edx, b
		push g
		push f
		push e
		push d
		push c
		call AtexSubCode5
	}
}

void NakedCall AtexSubCode1()
{
	InlineAsm{
push	ebp
mov	ebp, esp
mov	eax, [ebp+0x08]
push	ebx
push	edi
xor	edi, edi
test	eax, eax
mov	ebx, ecx
jbe	short loc_61019C
push	esi

loc_610151:				//; CODE XREF: AtexSubCode1+59j
mov	ecx, edi
mov	esi, 1
and	ecx, 1Fh
shl	esi, cl
test	esi, 0C0000003h
jnz	short loc_61017B
mov	ecx, edi
mov	eax, 1
shr	ecx, 6
and	ecx, 1Fh
shl	eax, cl
test	eax, 0C0000003h
jz	short loc_610193

loc_61017B:				//; CODE XREF: AtexSubCode1+23j
mov	eax, edi
shr	eax, 5
shl	eax, 2
mov	ecx, [eax+ebx]
or	ecx, esi
mov	[eax+ebx], ecx
mov	ecx, [eax+edx]
or	ecx, esi
mov	[eax+edx], ecx

loc_610193:				//; CODE XREF: AtexSubCode1+39j
mov	eax, [ebp+0x08]
inc	edi
cmp	edi, eax
jb	short loc_610151
pop	esi

loc_61019C:				//; CODE XREF: AtexSubCode1+Ej
pop	edi
pop	ebx
pop	ebp
retn	4
	}
}

void AtexSubCode1_(unsigned int a, unsigned int b, unsigned int c)
{
	InlineAsm
	{
		mov ecx, a
		mov edx, b
		push c
		call AtexSubCode1
	}
}
void NakedCall AtexSubCode7()
{
	InlineAsm{

push	ebp
mov	ebp, esp
sub	esp, 14h
mov	eax, edx
mov	[ebp+-0x8], ecx
xor	edx, edx
mov	[ebp+-0x14], eax
test	eax, eax
mov	[ebp+-0x10], ecx
mov	[ebp+-0xC], edx
jbe	loc_610392
push	ebx
push	esi
push	edi

loc_6101D1:				//; CODE XREF: AtexSubCode7+1D9j
mov	eax, edx
mov	esi, edx
and	eax, 3Fh
mov	edi, 1
mov	ecx, eax
mov	ebx, 1
and	ecx, 1Fh
shr	esi, 6
shl	edi, cl
mov	ecx, esi
and	ecx, 1Fh
shl	ebx, cl
and	edi, 0C0000003h
and	ebx, 0C0000003h
test	edi, edi
jnz	short loc_61020D
test	ebx, ebx
jz	loc_610377
jmp	short loc_610210
; ---------------------------------------------------------------------------

loc_61020D:				//; CODE XREF: AtexSubCode7+51j
xor	eax, 3

loc_610210:				//; CODE XREF: AtexSubCode7+5Bj
test	ebx, ebx
mov	[ebp+-0x4], eax
jz	short loc_61021A
xor	esi, 3

loc_61021A:				//; CODE XREF: AtexSubCode7+65j
mov	ecx, [ebp+-0x4]
mov	eax, 1
and	ecx, 1Fh
shl	eax, cl
test	eax, 0C0000003h
jz	short loc_610242
//push	381h
//mov	edx, offset aImgatex_cpp_5 ; "ImgAtex.cpp"
//mov	ecx, offset unk_8573F8
//call	near ptr ImageError

loc_610242:				//; CODE XREF: AtexSubCode7+7Cj
mov	ecx, esi
mov	edx, 1
and	ecx, 1Fh
shl	edx, cl
test	edx, 0C0000003h
jz	short loc_61026A
//push	382h
//mov	edx, offset aImgatex_cpp_6 ; "ImgAtex.cpp"
//mov	ecx, offset unk_8573FC
//call	near ptr ImageError

loc_61026A:				//; CODE XREF: AtexSubCode7+A4j
mov	edx, [ebp+-0x4]
mov	eax, [ebp+-0x10]
shl	esi, 6
add	esi, edx
shl	esi, 4
add	esi, eax
test	edi, edi
mov	edx, [esi+8]
mov	eax, [esi]
mov	ecx, [esi+4]
mov	[ebp+-0x4], edx
mov	edx, [esi+0Ch]
jz	loc_610323
mov	ecx, eax
mov	esi, eax
shr	ecx, 8
and	ecx, 0F000F0h
and	esi, 0F000F00h
or	ecx, esi
mov	esi, eax
and	esi, 0FFFF000Fh
and	eax, 0F000F0h
shl	esi, 8
or	esi, eax
shr	ecx, 4
shl	esi, 4
or	ecx, esi
mov	eax, ecx
shr	ecx, 8
mov	esi, eax
and	ecx, 0F000F0h
and	esi, 0F000F00h
mov	edi, eax
or	ecx, esi
mov	esi, eax
and	esi, 0FFFF000Fh
and	edi, 0F000F0h
shl	esi, 8
or	esi, edi
mov	edi, edx
shr	ecx, 4
shl	esi, 4
or	ecx, esi
mov	esi, edx
and	esi, 0FF030303h
and	edi, 0C0C0C0Ch
shl	esi, 4
or	esi, edi
mov	edi, edx
shr	edi, 4
and	edi, 0C0C0C0Ch
and	edx, 30303030h
or	edi, edx
shl	esi, 2
shr	edi, 2
or	esi, edi
mov	edx, esi

loc_610323:				//; CODE XREF: AtexSubCode7+DAj
test	ebx, ebx
jz	short loc_610363
mov	esi, eax
mov	eax, ecx
shr	eax, 10h
shl	ecx, 10h
or	eax, ecx
mov	ecx, esi
shr	ecx, 10h
shl	esi, 10h
or	ecx, esi
mov	esi, edx
mov	edi, edx
and	esi, 0FF0000h
shr	edi, 10h
or	esi, edi
mov	edi, edx
shl	edi, 10h
and	edx, 0FF00h
or	edi, edx
shr	esi, 8
shl	edi, 8
or	esi, edi
mov	edx, esi

loc_610363:				//; CODE XREF: AtexSubCode7+175j
mov	esi, [ebp+-0x8]
mov	[esi], eax
mov	eax, [ebp+-0x4]
mov	[esi+0Ch], edx
mov	edx, [ebp+-0xC]
mov	[esi+4], ecx
mov	[esi+8], eax

loc_610377:				//; CODE XREF: AtexSubCode7+55j
mov	ecx, [ebp+-0x8]
mov	eax, [ebp+-0x14]
inc	edx
add	ecx, 10h
cmp	edx, eax
mov	[ebp+-0xC], edx
mov	[ebp+-0x8], ecx
jb	loc_6101D1
pop	edi
pop	esi
pop	ebx

loc_610392:				//; CODE XREF: AtexSubCode7+18j
mov	esp, ebp
pop	ebp
retn
}
}

void AtexSubCode7_(unsigned int a, unsigned int b)
{
	InlineAsm
	{
		mov ecx, a
		mov edx, b
		call AtexSubCode7
	}
}
void NakedCall AtexSubCode2()
{
	InlineAsm{

push	ebp
mov	ebp, esp
sub	esp, 10h
mov	eax, [ebp+0x10]
push	ebx
push	esi
push	edi
xor	edi, edi
mov	[ebp+-0x10], edx
test	eax, eax
mov	[ebp+-0x4], ecx
jz	loc_610581
mov	esi, [ebp+0x0c]

loc_6103BF:				//; CODE XREF: AtexSubCode2+1DBj
mov	eax, [esi+0Ch]
xor	ecx, ecx
shr	eax, 1Ah
xor	ebx, ebx
mov	cl, ds:byte_79053D[eax*2]
mov	bl, ds:byte_79053C[eax*2]
inc	ecx
cmp	ebx, 20h
mov	[ebp+0x0c], ecx
jb	short loc_6103F1
//push	3Ch
//mov	edx, offset aPCodeBaseCompr ; "P:\\Code\\Base\\compress/CmpIo.h"
//mov	ecx, offset unk_838224
//call	near ptr ImageError

loc_6103F1:				//; CODE XREF: AtexSubCode2+3Ej
test	ebx, ebx
jz	short loc_61040D
mov	eax, [esi+10h]
mov	edx, [esi+0Ch]
mov	ecx, 20h
sub	ecx, ebx
shr	eax, cl
mov	ecx, ebx
shl	edx, cl
or	eax, edx
mov	[esi+0Ch], eax

loc_61040D:				//; CODE XREF: AtexSubCode2+53j
mov	eax, [esi+8]
cmp	ebx, eax
mov	[ebp+-0x8], eax
ja	short loc_610425
mov	edx, [esi+10h]
mov	ecx, ebx
shl	edx, cl
sub	eax, ebx
mov	[esi+10h], edx
jmp	short loc_610460
; ---------------------------------------------------------------------------

loc_610425:				//; CODE XREF: AtexSubCode2+75j
mov	ecx, [esi]
mov	edx, [esi+4]
cmp	ecx, edx
jz	short loc_61045B
mov	edx, [ecx]
add	ecx, 4
sub	eax, ebx
mov	[esi], ecx
mov	[esi+10h], edx
lea	ecx, [eax+20h]
mov	eax, edx
shr	edx, cl
mov	[ebp+-0xC], ecx
mov	ecx, [esi+0Ch]
or	ecx, edx
mov	[esi+0Ch], ecx
mov	ecx, ebx
sub	ecx, [ebp+-0x8]
shl	eax, cl
mov	[esi+10h], eax
mov	eax, [ebp+-0xC]
jmp	short loc_610460
; ---------------------------------------------------------------------------

loc_61045B:				//; CODE XREF: AtexSubCode2+8Cj
xor	eax, eax
mov	[esi+10h], eax

loc_610460:				//; CODE XREF: AtexSubCode2+83j
; AtexSubCode2+B9j
mov	ecx, [esi+10h]
mov	[esi+8], eax
mov	eax, [esi+0Ch]
mov	edx, ecx
mov	ebx, eax
add	eax, eax
shr	edx, 1Fh
or	edx, eax
mov	[esi+0Ch], edx
mov	edx, [esi+8]
shr	ebx, 1Fh
cmp	edx, 1
mov	[ebp+-0x8], ebx
jb	short loc_610490
add	ecx, ecx
dec	edx
mov	[esi+10h], ecx
mov	[esi+8], edx
jmp	short loc_6104D7
; ---------------------------------------------------------------------------

loc_610490:				//; CODE XREF: AtexSubCode2+E3j
mov	eax, [esi]
mov	ecx, [esi+4]
cmp	eax, ecx
jz	short loc_6104CF
mov	ecx, [eax]
add	eax, 4
mov	[esi+10h], ecx
mov	[esi], eax
mov	eax, [esi+10h]
lea	ecx, [edx+1Fh]
mov	ebx, eax
mov	[ebp+-0xC], ecx
shr	ebx, cl
mov	ecx, [esi+0Ch]
or	ecx, ebx
mov	ebx, [ebp+-0x8]
mov	[esi+0Ch], ecx
mov	ecx, 1
sub	ecx, edx
mov	edx, [ebp+-0xC]
shl	eax, cl
mov	[esi+8], edx
mov	[esi+10h], eax
jmp	short loc_6104D7
; ---------------------------------------------------------------------------

loc_6104CF:				//; CODE XREF: AtexSubCode2+F7j
xor	eax, eax
mov	[esi+10h], eax
mov	[esi+8], eax

loc_6104D7:				//; CODE XREF: AtexSubCode2+EEj
; AtexSubCode2+12Dj
mov	eax, [ebp+0x0c]
test	eax, eax
jz	short loc_610546

loc_6104DE:				//; CODE XREF: AtexSubCode2+1A4j
cmp	edi, [ebp+0x10]
jz	loc_610581
mov	ecx, edi
mov	eax, edi
and	ecx, 1Fh
mov	edx, 1
shr	eax, 5
shl	edx, cl
mov	ecx, [ebp+0x08]
shl	eax, 2
mov	ecx, [eax+ecx]
test	edx, ecx
jnz	short loc_610532
test	ebx, ebx
jz	short loc_61052F
mov	ebx, [ebp+-0x4]
or	ecx, edx
mov	dword ptr [ebx], 0FFFFFFFEh
mov	dword ptr [ebx+4], 0FFFFFFFFh
mov	ebx, [ebp+0x08]
mov	[eax+ebx], ecx
mov	ecx, [ebp+-0x10]
mov	ebx, [eax+ecx]
or	ebx, edx
mov	[eax+ecx], ebx
mov	ebx, [ebp+-0x8]

loc_61052F:				//; CODE XREF: AtexSubCode2+167j
dec	[ebp+0x0c]

loc_610532:				//; CODE XREF: AtexSubCode2+163j
mov	edx, [ebp+-0x4]
mov	eax, [ebp+0x14]
inc	edi
lea	ecx, [edx+eax*4]
mov	eax, [ebp+0x0c]
test	eax, eax
mov	[ebp+-0x4], ecx
jnz	short loc_6104DE

loc_610546:				//; CODE XREF: AtexSubCode2+13Cj
mov	eax, [ebp+0x10]
cmp	edi, eax
jz	short loc_610581

loc_61054D:				//; CODE XREF: AtexSubCode2+1D7j
mov	ebx, [ebp+0x08]
mov	ecx, edi
and	ecx, 1Fh
mov	edx, 1
shl	edx, cl
mov	ecx, edi
shr	ecx, 5
test	[ebx+ecx*4], edx
jz	short loc_610579
mov	edx, [ebp+0x14]
mov	ecx, [ebp+-0x4]
inc	edi
lea	edx, [ecx+edx*4]
cmp	edi, eax
mov	[ebp+-0x4], edx
jz	short loc_610581
jmp	short loc_61054D
; ---------------------------------------------------------------------------

loc_610579:				//; CODE XREF: AtexSubCode2+1C4j
cmp	edi, eax
jnz	loc_6103BF

loc_610581:				//; CODE XREF: AtexSubCode2+16j
; AtexSubCode2+141j ...
pop	edi
pop	esi
pop	ebx
mov	esp, ebp
pop	ebp
retn	10h
}
}

#pragma warning(pop)

void AtexSubCode2_(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int e, unsigned int f)
{
	InlineAsm
	{
		mov ecx, a
		mov edx, b
		push f
		push e
		push d
		push c
		call AtexSubCode2
	}
}

struct SImageData
{
	unsigned int *DataPos,*EndPos,_44,_40,_3C,xres,yres;
};

bool AtexDecompress(unsigned int *InputBuffer, unsigned int BufferSize, unsigned int ImageFormat, SImageDescriptor ImageDescriptor, unsigned int *OutBuffer)
{
	unsigned int HeaderSize=12;

	SImageData ImageData;

	int AlphaDataSize2=((ImageFormat && 21)-1)&2;

	int ColorDataSize=ImgFmt(ImageFormat);
    if (ColorDataSize == -1) {
        return false;
    }

	int AlphaDataSize=ColorDataSize;
	
	AlphaDataSize&=640;
	if (AlphaDataSize) AlphaDataSize=2;

	ColorDataSize&=528;
	if (ColorDataSize) ColorDataSize=2;

	int BlockSize=ColorDataSize+AlphaDataSize2+AlphaDataSize;

	int BlockCount=ImageDescriptor.xres*ImageDescriptor.yres/16;

	if (!BlockCount)
	{
		return false;
	}

	unsigned int *DcmpBuffer1=(unsigned int*)::malloc(BlockCount * sizeof(unsigned int));
	unsigned int *DcmpBuffer2=DcmpBuffer1+BlockCount/2;
	memset(DcmpBuffer1,0,BlockCount*4);

	ImageData.xres=ImageDescriptor.xres;
	ImageData.yres=ImageDescriptor.yres;

	unsigned int DataSize=InputBuffer[HeaderSize>>2];
		
	/*if (HeaderSize+8>=BufferSize) printf("Error 567h\n");
	if (DataSize<=8) printf("Error 569h\n");
	if (DataSize+HeaderSize>BufferSize) printf("Error 56Ah\n");*/

	int CompressionCode=InputBuffer[(HeaderSize+4)>>2];

	ImageData.DataPos=InputBuffer+((HeaderSize+8)>>2);

	if (CompressionCode)
	{
		ImageData._40=ImageData._44=ImageData._3C=0;
		ImageData.EndPos=ImageData.DataPos+((DataSize-8)>>2);

		if (ImageData.DataPos!=ImageData.EndPos)
		{
			ImageData._40=ImageData.DataPos[0];
			ImageData.DataPos++;
		}

		if (CompressionCode&0x10 && ImageData.xres==256 && ImageData.yres==256 && (ImageFormat==0x11 || ImageFormat==0x10)) 
			AtexSubCode1_((unsigned int)DcmpBuffer1,(unsigned int)DcmpBuffer2,BlockCount);

		if (CompressionCode&1 && ColorDataSize && !AlphaDataSize && !AlphaDataSize2) 
			AtexSubCode2_((unsigned int)OutBuffer,(unsigned int)DcmpBuffer1,(unsigned int)DcmpBuffer2,(unsigned int)&ImageData,BlockCount,BlockSize);

		if (CompressionCode&2 && ImageFormat>=0x10 && ImageFormat<=0x11)
			AtexSubCode3_((unsigned int)OutBuffer,(unsigned int)DcmpBuffer1,(unsigned int)DcmpBuffer2,(unsigned int)&ImageData,BlockCount,BlockSize);

		if (CompressionCode&4 && ImageFormat>=0x12 && ImageFormat<=0x15) 
			AtexSubCode4_((unsigned int)OutBuffer,(unsigned int)DcmpBuffer1,(unsigned int)DcmpBuffer2,(unsigned int)&ImageData,BlockCount,BlockSize);

		if (CompressionCode&8 && ColorDataSize)
			AtexSubCode5_((unsigned int)OutBuffer+AlphaDataSize2+AlphaDataSize*4,(unsigned int)DcmpBuffer1,(unsigned int)DcmpBuffer2,(unsigned int)&ImageData,BlockCount,BlockSize,ImageFormat==0xf);

		ImageData.DataPos--;
	}

	unsigned int *DataEnd=InputBuffer+((HeaderSize+DataSize)>>2);

	if ((AlphaDataSize || AlphaDataSize2) && BlockCount)
	{
		unsigned int *BufferVar=OutBuffer;

		for (int x=0; x<BlockCount; x++)
		{
			if (! (DcmpBuffer1[x>>5] & 1<<x) )
			{
				BufferVar[0]=ImageData.DataPos[0];
				BufferVar[1]=ImageData.DataPos[1];
				ImageData.DataPos+=2;
			}
			BufferVar+=BlockSize;
		}
	}

	if (ColorDataSize && BlockCount)
	{
		unsigned int *BufferVar=OutBuffer+AlphaDataSize2+AlphaDataSize;

		for (int x=0; x<BlockCount; x++)
		{
			if (! (DcmpBuffer2[x>>5] & 1<<x) )
			{
				BufferVar[0]=ImageData.DataPos[0];
				ImageData.DataPos++;
			}
			BufferVar+=BlockSize;
		}

		BufferVar=OutBuffer+AlphaDataSize2+AlphaDataSize+1;

		for (int x=0; x<BlockCount; x++)
		{
			if (! (DcmpBuffer2[x>>5] & 1<<x) )
			{
				BufferVar[0]=ImageData.DataPos[0];
				ImageData.DataPos++;
			}
			BufferVar+=BlockSize;
		} 
	}

	if (CompressionCode&0x10 && ImageData.xres==256 && ImageData.yres==256 && (ImageFormat==0x10 || ImageFormat==0x11))
		AtexSubCode7_((unsigned int)OutBuffer,BlockCount);

	gw2b::FreePointer(DcmpBuffer1);
    return true;
}
