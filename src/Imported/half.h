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

#pragma once
#ifndef neo_math_half_h
#define neo_math_half_h

#include <cstdint>

/*! \file math/half.h
    16-bit floating point numbers */

namespace neo {
namespace math {

/*! \brief 16-bit floating point number

	Type HalfFloat can represent positive and negative numbers, whose
	magnitude is between roughly 6.1e-5 and 6.5e+4, with a relative
	error of 9.8e-4; numbers smaller than 6.1e-5 can be represented
	with an absolute error of 6.0e-8.  All integers from -2048 to
	+2048 can be represented exactly.

	Type HalfFloat behaves (almost) like the built-in C++ floating point
	types.  In arithmetic expressions, HalfFloat, float and double can be
	mixed freely.  Here are a few examples:

	    HalfFloat a (3.5);
	    float b (a + sqrt (a));
	    a += b;
	    b += a;
	    b = a + 7;

	Conversions from HalfFloat to float are lossless; all HalfFloat numbers
	are exactly representable as floats.

	Conversions from float to HalfFloat may not preserve the float's
	value exactly.  If a float is not representable as a HalfFloat, the
	float value is rounded to the nearest representable HalfFloat.  If
	a float value is exactly in the middle between the two closest
	representable HalfFloat values, then the float value is rounded to
	the HalfFloat with the greater magnitude.

	Overflows during float-to-HalfFloat conversions cause arithmetic
	exceptions.  An overflow occurs when the float value to be
	converted is too large to be represented as a HalfFloat, or if the
	float value is an infinity or a NAN.

	The implementation of type HalfFloat makes the following assumptions
	about the implementation of the built-in C++ types:

	    float is an IEEE 754 single-precision number
	    sizeof (float) == 4
	    alignof (uint32_t) == alignof (float) */
class HalfFloat
{
	public:

	//! Construct uninitialized number
	inline                                                 HalfFloat();

	//! Construct from 32-bit floating point number
	/*! \param val                                         Value */
	inline                                                 HalfFloat( float val );

	//! Convert to 32-bit floating point number
	inline operator                                        float () const;


	inline HalfFloat                                       operator - () const;

	inline HalfFloat&                                      operator = ( HalfFloat h );
	inline HalfFloat&                                      operator = ( float f );

	inline HalfFloat&                                      operator += ( HalfFloat h );
	inline HalfFloat&                                      operator += ( float f );

	inline HalfFloat&                                      operator -= ( HalfFloat h );
	inline HalfFloat&                                      operator -= ( float f );

	inline HalfFloat&                                      operator *= ( HalfFloat h );
	inline HalfFloat&                                      operator *= ( float f );

	inline HalfFloat&                                      operator /= ( HalfFloat h );
	inline HalfFloat&                                      operator /= ( float f );

	//---------------------------------------------------------
	// Round to n-bit precision (n should be between 0 and 10).
	// After rounding, the significand's 10-n least significant
	// bits will be zero.
	//---------------------------------------------------------
	inline HalfFloat                                       round( unsigned int n ) const;

	//--------------------------------------------------------------------
	// Classification:
	//
	//	h.isFinite()		returns true if h is a normalized number,
	//				a denormalized number or zero
	//
	//	h.isNormalized()	returns true if h is a normalized number
	//
	//	h.isDenormalized()	returns true if h is a denormalized number
	//
	//	h.isZero()		returns true if h is zero
	//
	//	h.isNan()		returns true if h is a NAN
	//
	//	h.isInfinity()		returns true if h is a positive
	//				or a negative infinity
	//
	//	h.isNegative()		returns true if the sign bit of h
	//				is set (negative)
	//--------------------------------------------------------------------
	inline bool                                            isFinite() const;
	inline bool                                            isNormalized() const;
	inline bool                                            isDenormalized() const;
	inline bool                                            isZero() const;
	inline bool                                            isNaN() const;
	inline bool                                            isInfinity() const;
	inline bool                                            isNegative() const;

	//--------------------------------------------
	// Special values
	//
	//	posInf()	returns +infinity
	//
	//	negInf()	returns +infinity
	//
	//	qNan()		returns a NAN with the bit
	//			pattern 0111111111111111
	//
	//	sNan()		returns a NAN with the bit
	//			pattern 0111110111111111
	//--------------------------------------------
	static inline HalfFloat                                posInf();
	static inline HalfFloat                                negInf();
	static inline HalfFloat                                qNaN();
	static inline HalfFloat                                sNaN();

	//--------------------------------------
	// Access to the internal representation
	//--------------------------------------
	inline uint16_t                                        bits() const;
	inline void                                            setBits( uint16_t bits );

	private:

	union uif
	{
		uint32_t i;
		float f;
	};

	float                                                  toFloat() const;

	static int16_t                                         convert( int32_t i );
	static float                                           overflow ();

	uint16_t                                               _h;

	static const uint16_t                                  _eLut[ 1 << 9 ];
};

#include "half.inl"

typedef HalfFloat float16_t;

}
}

#endif