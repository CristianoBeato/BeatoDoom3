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

#ifndef __INPUT_EVENT_H__
#define __INPUT_EVENT_H__


#include <SDL_events.h>

class btInputEvent
{
public:
	struct mseEvt_t
	{

	};

	struct keyEvt_t
	{

	};

	struct ctrlEvt_t
	{

	};

	btInputEvent( void );
	~btInputEvent( void );

	void	StartUp( void );
	void	ShutDown( void );
	void	HandleEvents( void );

	void	ActivateKeyboard( const bool activate );
	void	ActivateMouse( const bool activate );

	const int MouseEventCount( void ) const;
	const int KeyboardEventsCount( void ) const;
	const int ControlerEventsCount( void ) const;

	const int MouseInputEvent( const int n, int &action, int &value ) const;
	const int KeyboardInputEvent( const int n, int &ch, bool &state ) const;
	const int ControlerInputEvent( const int device, const int n, int& action, int& value ) const;

	void	ClearMouseInput( void );
	void	ClearKeyboardInput( void );
	void	ClearControlerInput( void );

private:
	int		HandleToolsEvent( void );
	int		ProcessMouseEvent( void );
	int		ProcessKeyboardEvent( void );
	int		ProcessControlerEvent( void );
	int		ProcessWindowEvent( void );
	int		ProcessTextEvent( void );

	SDL_Event			m_event;
	idList<mseEvt_t>	m_mouseEventArray;
	idList<keyEvt_t>	m_keyboardEventArray;
	idList<ctrlEvt_t>	m_controlerEventArray;
};

#endif // !__INPUT_EVENT_H__
