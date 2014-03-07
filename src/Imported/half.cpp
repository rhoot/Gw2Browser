///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002, Industrial Light & Magic, a division of Lucas
// Digital Ltd. LLC
// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// Primary authors:
//     Florian Kainz <kainz@ilm.com>
//     Rod Bogart <rgb@ilm.com>
// Modified for NeoEngine 2 - Evolution by
//     Mattias Jansson <mattias@realityrift.com>
// Modified for Gw2Browser - using cstdint rather than assumptions
//     Rhoot <https://github.com/rhoot>

#include "stdafx.h"
#include "half.h"

namespace neo {
	namespace math {


		const uint16_t HalfFloat::_eLut[1 << 9] =
		{
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 1024, 2048, 3072, 4096, 5120, 6144, 7168,
			8192, 9216, 10240, 11264, 12288, 13312, 14336, 15360,
			16384, 17408, 18432, 19456, 20480, 21504, 22528, 23552,
			24576, 25600, 26624, 27648, 28672, 29696, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 33792, 34816, 35840, 36864, 37888, 38912, 39936,
			40960, 41984, 43008, 44032, 45056, 46080, 47104, 48128,
			49152, 50176, 51200, 52224, 53248, 54272, 55296, 56320,
			57344, 58368, 59392, 60416, 61440, 62464, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
		};


		float HalfFloat::toFloat( ) const {
			uint16_t y = _h;
			uif fl;
			int32_t s = ( y >> 15 ) & 0x00000001;
			int32_t e = ( y >> 10 ) & 0x0000001f;
			int32_t m = y & 0x000003ff;

			if ( e == 0 ) {
				if ( m == 0 ) {
					//
					// Plus or minus zero
					//
					fl.i = s << 31;
					return fl.f;
				} else {
					//
					// Denormalized number -- renormalize it
					//
					while ( !( m & 0x00000400 ) ) {
						m <<= 1;
						e -= 1;
					}

					e += 1;
					m &= ~0x00000400;
				}
			} else if ( e == 31 ) {
				if ( m == 0 ) {
					//
					// Positive or negative infinity
					//
					fl.i = ( s << 31 ) | 0x7f800000;
					return fl.f;
				} else {
					//
					// Nan -- preserve sign and significand bits
					//
					fl.i = ( s << 31 ) | 0x7f800000 | ( m << 13 );
					return fl.f;
				}
			}

			//
			// Normalized number
			//
			e = e + ( 127 - 15 );
			m = m << 13;

			//
			// Assemble s, e and m.
			//
			fl.i = ( s << 31 ) | ( e << 23 ) | m;
			return fl.f;
		}


		//-----------------------------------------------
		// Overflow handler for float-to-HalfFloat conversion;
		// generates a hardware floating-point overflow,
		// which may be trapped by the operating system.
		//-----------------------------------------------
		float HalfFloat::overflow( ) {
			volatile float f = 1e10;

			for ( int i = 0; i < 10; i++ )
				f *= f;				// this will overflow before
			// the forloop terminates
			return f;
		}


		//-----------------------------------------------------
		// Float-to-HalfFloat conversion -- general case, including
		// zeroes, denormalized numbers and exponent overflows.
		//-----------------------------------------------------

		short HalfFloat::convert( int32_t i ) {
			//
			// Our floating point number, f, is represented by the bit
			// pattern in integer i.  Disassemble that bit pattern into
			// the sign, s, the exponent, e, and the significand, m.
			// Shift s into the position where it will go in in the
			// resulting HalfFloat number.
			// Adjust e, accounting for the different exponent bias
			// of float and HalfFloat (127 versus 15).
			//
			register int32_t s = ( i >> 16 ) & 0x00008000;
			register int32_t e = ( ( i >> 23 ) & 0x000000ff ) - ( 127 - 15 );
			register int32_t m = i & 0x007fffff;

			//
			// Now reassemble s, e and m into a HalfFloat:
			//

			if ( e <= 0 ) {
				if ( e < -10 ) {
					//
					// E is less than -10.  The absolute value of f is
					// less than HALF_MIN (f may be a small normalized
					// float, a denormalized float or a zero).
					//
					// We convert f to a HalfFloat zero.
					//

					return 0;
				}

				//
				// E is between -10 and 0.  F is a normalized float,
				// whose magnitude is less than HALF_NRM_MIN.
				//
				// We convert f to a denormalized HalfFloat.
				// 

				m = ( m | 0x00800000 ) >> ( 1 - e );

				//
				// Round to nearest, round "0.5" up.
				//
				// Rounding may cause the significand to overflow and make
				// our number normalized.  Because of the way a HalfFloat's bits
				// are laid out, we don't have to treat this case separately;
				// the code below will handle it correctly.
				// 

				if ( m & 0x00001000 )
					m += 0x00002000;

				//
				// Assemble the HalfFloat from s, e (zero) and m.
				//

				return s | ( m >> 13 );
			} else if ( e == 0xff - ( 127 - 15 ) ) {
				if ( m == 0 ) {
					//
					// F is an infinity; convert f to a HalfFloat
					// infinity with the same sign as f.
					//

					return s | 0x7c00;
				} else {
					//
					// F is a NAN; we produce a HalfFloat NAN that preserves
					// the sign bit and the 10 leftmost bits of the
					// significand of f, with one exception: If the 10
					// leftmost bits are all zero, the NAN would turn 
					// into an infinity, so we have to set at least one
					// bit in the significand.
					//

					m >>= 13;
					return s | 0x7c00 | m | ( m == 0 );
				}
			} else {
				//
				// E is greater than zero.  F is a normalized float.
				// We try to convert f to a normalized HalfFloat.
				//

				//
				// Round to nearest, round "0.5" up
				//

				if ( m & 0x00001000 ) {
					m += 0x00002000;

					if ( m & 0x00800000 ) {
						m = 0;		// overflow in significand,
						e += 1;		// adjust exponent
					}
				}

				//
				// Handle exponent overflow
				//

				if ( e > 30 ) {
					overflow( );	// Cause a hardware floating point overflow;
					return s | 0x7c00;	// if this returns, the HalfFloat becomes an
				}   			// infinity with the same sign as f.

				//
				// Assemble the HalfFloat from s, e and m.
				//

				return s | ( e << 10 ) | ( m >> 13 );
			}
		}

	}
}