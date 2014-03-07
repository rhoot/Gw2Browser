/* \file       InitOpenGL.cpp
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

#include "stdafx.h"

#include "InitOpenGL.h"

namespace gw2b {

	void InitOpenGL( ) {

#if defined(_WIN32) || !defined(__APPLE__) || defined(GLEW_APPLE_GLX)
		char* display = NULL;
		int visual = -1;
#endif

#if defined(_WIN32)
		if ( GL_TRUE == glewCreateContext( &visual ) )
#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)
		if ( GL_TRUE == glewCreateContext( ) )
#else
		if ( GL_TRUE == glewCreateContext( display, &visual ) )
#endif
		{
			wxMessageBox( wxString( "Error: glewCreateContext failed" ), _( "ERROR" ), wxOK | wxICON_EXCLAMATION );
			glewDestroyContext( );
		}

		//glewExperimental = GL_TRUE;
		// Init GLEW
		GLenum glewError = glewInit( );
		if ( glewError != GLEW_OK ) {
			wxMessageBox( wxString( "GLEW Error: " ) + wxString( glewGetErrorString( glewError ) ), _( "ERROR" ), wxOK | wxICON_EXCLAMATION );
		}

	}

	//============================================================================/

#if defined(_WIN32)

	HWND hwnd = NULL; // Window identifier
	HDC hdc = NULL; // Device context
	HGLRC hrc = NULL; // Rendering context

	GLboolean glewCreateContext( int* pixelformat ) {
		//WNDCLASS wc;
		PIXELFORMATDESCRIPTOR pfd;

		/* get the device context */
		hdc = GetDC( hwnd );
		if ( NULL == hdc ) {
			return GL_TRUE;
		}

		/* find pixel format */
		ZeroMemory( &pfd, sizeof( PIXELFORMATDESCRIPTOR ) );
		//memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
		if ( *pixelformat == -1 ) /* find default */
		{
			pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );

			pfd.nVersion = 1;

			pfd.dwFlags = /*PFD_DOUBLEBUFFER |*/ PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
			pfd.iPixelType = PFD_TYPE_RGBA;
			//pfd.cColorBits = 32;
			//pfd.cDepthBits = 32;
			//pfd.iLayerType = PFD_MAIN_PLANE;

			*pixelformat = ChoosePixelFormat( hdc, &pfd );
			if ( *pixelformat == 0 ) return GL_TRUE;
		}

		/* set the pixel format for the dc */
		if ( FALSE == SetPixelFormat( hdc, *pixelformat, &pfd ) ) {
			return GL_TRUE;
		}

		/* create rendering context */
		hrc = wglCreateContext( hdc );
		if ( NULL == hrc ) {
			return GL_TRUE;
		}
		if ( FALSE == wglMakeCurrent( hdc, hrc ) ) {
			return GL_TRUE;
		}

		return GL_FALSE;
	}

	void glewDestroyContext( ) {
		if ( NULL != hrc ) wglMakeCurrent( NULL, NULL );
		if ( NULL != hrc ) wglDeleteContext( hrc );
		if ( NULL != hwnd && NULL != hdc ) ReleaseDC( hwnd, hdc );
	}

	/* ------------------------------------------------------------------------ */

#elif defined(__APPLE__) && !defined(GLEW_APPLE_GLX)

	AGLContext ctx, octx;

	GLboolean glewCreateContext( ) {
		int attrib[] = { AGL_RGBA, AGL_NONE };
		AGLPixelFormat pf;
		/*int major, minor;
		SetPortWindowPort(wnd);
		aglGetVersion(&major, &minor);
		fprintf(stderr, "GL %d.%d\n", major, minor);*/
		pf = aglChoosePixelFormat( NULL, 0, attrib );
		if ( NULL == pf ) return GL_TRUE;
		ctx = aglCreateContext( pf, NULL );
		if ( NULL == ctx || AGL_NO_ERROR != aglGetError( ) ) return GL_TRUE;
		aglDestroyPixelFormat( pf );
		/*aglSetDrawable(ctx, GetWindowPort(wnd));*/
		octx = aglGetCurrentContext( );
		if ( GL_FALSE == aglSetCurrentContext( ctx ) ) return GL_TRUE;
		/* Needed for Regal on the Mac */
#if defined(GLEW_REGAL) && defined(__APPLE__)
		RegalMakeCurrent( octx );
#endif
		return GL_FALSE;
	}

	void glewDestroyContext( ) {
		aglSetCurrentContext( octx );
		if ( NULL != ctx ) aglDestroyContext( ctx );
	}

	/* ------------------------------------------------------------------------ */

#else /* __UNIX || (__APPLE__ && GLEW_APPLE_GLX) */

	Display* dpy = NULL;
	XVisualInfo* vi = NULL;
	XVisualInfo* vis = NULL;
	GLXContext ctx = NULL;
	Window wnd = 0;
	Colormap cmap = 0;

	GLboolean glewCreateContext( const char* display, int* visual ) {
		int attrib[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
		int erb, evb;
		XSetWindowAttributes swa;
		/* open display */
		dpy = XOpenDisplay( display );
		if ( NULL == dpy ) return GL_TRUE;
		/* query for glx */
		if ( !glXQueryExtension( dpy, &erb, &evb ) ) return GL_TRUE;
		/* choose visual */
		if ( *visual == -1 ) {
			vi = glXChooseVisual( dpy, DefaultScreen( dpy ), attrib );
			if ( NULL == vi ) return GL_TRUE;
			*visual = ( int ) XVisualIDFromVisual( vi->visual );
		} else {
			int n_vis, i;
			vis = XGetVisualInfo( dpy, 0, NULL, &n_vis );
			for ( i = 0; i<n_vis; i++ ) {
				if ( ( int ) XVisualIDFromVisual( vis[i].visual ) == *visual )
					vi = &vis[i];
			}
			if ( vi == NULL ) return GL_TRUE;
		}
		/* create context */
		ctx = glXCreateContext( dpy, vi, None, True );
		if ( NULL == ctx ) return GL_TRUE;
		/* create window */
		/*wnd = XCreateSimpleWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 1, 1, 1, 0, 0);*/
		cmap = XCreateColormap( dpy, RootWindow( dpy, vi->screen ), vi->visual, AllocNone );
		swa.border_pixel = 0;
		swa.colormap = cmap;
		wnd = XCreateWindow( dpy, RootWindow( dpy, vi->screen ),
			0, 0, 1, 1, 0, vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWColormap, &swa );
		/* make context current */
		if ( !glXMakeCurrent( dpy, wnd, ctx ) ) return GL_TRUE;
		return GL_FALSE;
	}

	void glewDestroyContext( ) {
		if ( NULL != dpy && NULL != ctx ) glXDestroyContext( dpy, ctx );
		if ( NULL != dpy && 0 != wnd ) XDestroyWindow( dpy, wnd );
		if ( NULL != dpy && 0 != cmap ) XFreeColormap( dpy, cmap );
		if ( NULL != vis )
			XFree( vis );
		else if ( NULL != vi )
			XFree( vi );
		if ( NULL != dpy ) XCloseDisplay( dpy );
	}

#endif /* __UNIX || (__APPLE__ && GLEW_APPLE_GLX) */

}; // namespace gw2b
