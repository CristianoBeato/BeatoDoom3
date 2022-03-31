/*
===========================================================================

Beato Tech 4 Source Code
Copyright (C) 2017-2021 Cristiano Beato

This file is part of the Beato Tech 4 Source Code ("Beato Tech 4 Source Code").

Beato Tech 4 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato Tech 4 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato Tech 4 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Beato Tech 4 Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the terms and conditions of the
GNU General Public License which accompanied the Beato Tech 4 Source Code.

===========================================================================
*/

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <SDL_video.h>

class btWindow
{
public:
	btWindow( void );
	~btWindow( void );

	bool				Create( const char* title, const SDL_Rect windowBounds, Uint32 flags );
	void				Destroy( void );
	void				UpdateBounds( const SDL_Rect windowBounds );
	void				GetBounds( SDL_Rect &windowBounds );
	void				GrabMouseCursor( const bool grabIt );
	const bool			IsFlagActive( const Uint32 flag );
	const bool			TogleFullScreen( const bool togle, const bool desktop = false );
	const bool			SetFocus( void );
	const bool			Show( void );
	const int			GetDisplayID( void );
	const bool			Hide( void );
	const int			SetDisplay( const int index );
	const int			Id( void ) const;
	SDL_Window*			GetHandler( void ) const;

private:
	//Window dimensions
	bool		m_mouseGrab;
	SDL_Rect	m_bounds = { 0, 0, 0, 0 };
	SDL_Window*	m_handler = nullptr;
};


#endif // !__WINDOW_H__
