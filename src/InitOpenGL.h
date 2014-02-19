/* \file       InitOpenGL.h
*  \brief      <to do>
*  \author     Khral Steelforge
*/

/*
Copyright (C) 2014 Khral Steelforge <https://github.com/kytulendu>

GLEW info

Copyright (C) 2002-2008, Milan Ikits <milan ikits[]ieee org>
Copyright (C) 2002-2008, Marcelo E. Magallon <mmagallo[]debian org>
Copyright (C) 2002, Lev Povalahev

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

#ifndef INITOPENGL_H_INCLUDED
#define INITOPENGL_H_INCLUDED

#if defined( __APPLE__ ) && !defined( GLEW_APPLE_GLX )
#include <AGL/agl.h>
#endif

namespace gw2b {

		void InitOpenGL( );

#if defined(_WIN32)
		GLboolean glewCreateContext( int* pixelformat );
#elif !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
		GLboolean glewCreateContext( const char* display, int* visual );
#else
		GLboolean glewCreateContext( );
#endif
		void glewDestroyContext( );

}; // namespace gw2b

#endif // INITOPENGL_H_INCLUDED
