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

#include "idlib/precompiled.h"
#pragma hdrstop

#include <SDL_mouse.h>
#include <SDL_events.h>

#include "sys/sys_public.h"
#include "window.h"

#if 0
class idSort_VidMode : public idSort_Quick< vidMode_t, idSort_VidMode >
{
public:
	int Compare(const vidMode_t& a, const vidMode_t& b) const
	{
		int wd = a.width - b.width;
		int hd = a.height - b.height;
		int fd = a.displayHz - b.displayHz;
		return (hd != 0) ? hd : (wd != 0) ? wd : fd;
	}
};

// RB: resolutions supported by XreaL
static void FillStaticVidModes(idList<vidMode_t>& modeList)
{
	modeList.AddUnique(vidMode_t(320, 240, 60));
	modeList.AddUnique(vidMode_t(400, 300, 60));
	modeList.AddUnique(vidMode_t(512, 384, 60));
	modeList.AddUnique(vidMode_t(640, 480, 60));
	modeList.AddUnique(vidMode_t(800, 600, 60));
	modeList.AddUnique(vidMode_t(960, 720, 60));
	modeList.AddUnique(vidMode_t(1024, 768, 60));
	modeList.AddUnique(vidMode_t(1152, 864, 60));
	modeList.AddUnique(vidMode_t(1280, 720, 60));
	modeList.AddUnique(vidMode_t(1280, 768, 60));
	modeList.AddUnique(vidMode_t(1280, 800, 60));
	modeList.AddUnique(vidMode_t(1280, 1024, 60));
	modeList.AddUnique(vidMode_t(1360, 768, 60));
	modeList.AddUnique(vidMode_t(1440, 900, 60));
	modeList.AddUnique(vidMode_t(1680, 1050, 60));
	modeList.AddUnique(vidMode_t(1600, 1200, 60));
	modeList.AddUnique(vidMode_t(1920, 1080, 60));
	modeList.AddUnique(vidMode_t(1920, 1200, 60));
	modeList.AddUnique(vidMode_t(2048, 1536, 60));
	modeList.AddUnique(vidMode_t(2560, 1600, 60));

	modeList.SortWithTemplate(idSort_VidMode());
}

bool btWindowManager::GetModeListForDisplay(const int requestedDisplayNum, idList<vidMode_t> & modeList)
{
	assert(requestedDisplayNum >= 0);

	modeList.Clear();

	// DG: SDL2 implementation
	if (requestedDisplayNum >= SDL_GetNumVideoDisplays())
		return false; // requested invalid displaynum

	int numModes = SDL_GetNumDisplayModes(requestedDisplayNum);
	if (numModes > 0)
	{
		for (int i = 0; i < numModes; i++)
		{
			SDL_DisplayMode m;
			int ret = SDL_GetDisplayMode(requestedDisplayNum, i, &m);
			if (ret != 0)
			{
				common->Warning("Can't get video mode no %i, because of %s\n", i, SDL_GetError());
				continue;
			}

			vidMode_t mode;
			mode.width = m.w;
			mode.height = m.h;
			mode.displayHz = m.refresh_rate ? m.refresh_rate : 60; // default to 60 if unknown (0)
			modeList.AddUnique(mode);
		}

		if (modeList.Num() < 1)
		{
			common->Warning("Couldn't get a single video mode for display %i, using default ones..!\n", requestedDisplayNum);
			FillStaticVidModes(modeList);
		}

		// sort with lowest resolution first
		modeList.SortWithTemplate(idSort_VidMode());
	}
	else
	{
		common->Warning("Can't get Video Info, using default modes...\n");
		if (numModes < 0)
			common->Warning("Reason was: %s\n", SDL_GetError());
		
		FillStaticVidModes(modeList);
	}


	common->Printf("Suported view modes:");
	for (vidMode_t mode : modeList)
	{
		common->Printf("mode width %i height %i %i Hz.\n", mode.width, mode.height, mode.displayHz);
	}

	return true;
	// DG end
}

bool btWindowManager::CreateMainWindow(const screenParms_t parm)
{
	Uint32 flags = SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_SHOWN;
	int displayIdx = 0;

	if (!SDL_WasInit(SDL_INIT_VIDEO))
	{
		if (SDL_Init(SDL_INIT_VIDEO))
		{
			common->Error("Error while initializing SDL: %s", SDL_GetError());
			return false;
		}
	}

	if (parm.fullScreen > 0)
	{
		flags |= SDL_WINDOW_FULLSCREEN;

		if (parm.fullScreen > SDL_GetNumVideoDisplays())
		{
			common->Warning("Can't set fullscreen mode to display number %i, because SDL2 only knows about %i displays!, set defaut",
				parm.fullScreen, SDL_GetNumVideoDisplays());
			displayIdx = 0;
		}
		else
			displayIdx = parm.fullScreen - 1; // first display for SDL is 0, in parms it's 1
	}
	else if (parm.fullScreen < 0)
		flags |= SDL_WINDOW_BORDERLESS;
	else
		flags |= SDL_WINDOW_RESIZABLE;

	switch (parm.api)
	{
	case API_VULKAN: flags |= SDL_WINDOW_VULKAN; break;
	case API_OPENGL: flags |= SDL_WINDOW_OPENGL; break;
	case API_DX11: 
	case API_DX12:
	//	flags |= SDL_WINDOW_VULKAN; break; TODO ?
	default:
		break;
	}

	// DG: set display num for fullscreen
	int windowPos = SDL_WINDOWPOS_UNDEFINED;

	// -1 because SDL starts counting displays at 0, while parms.fullScreen starts at 1
	windowPos = SDL_WINDOWPOS_UNDEFINED_DISPLAY((displayIdx));

	m_window = new btWindowFrame(GAME_NAME, windowPos, windowPos,  parm.width, parm.height, flags);
	if (m_window == nullptr)
	{
		common->FatalError("Can't Create Main Window");
		return false;
	}

	SDL_GetWindowSize((SDL_Window*)m_window->GetHandler(),  &rbConfig.nativeScreenWidth, &rbConfig.nativeScreenHeight);

	if (SDL_SetWindowGammaRamp((SDL_Window*)m_window->GetHandler(), parm.red, parm.green, parm.blue))
		common->Warning("Couldn't set gamma ramp: %s", SDL_GetError());

	SDL_ShowCursor(SDL_DISABLE);

	return true;
}

int btWindowManager::ScreenParmsHandleDisplayIndex(screenParms_t parms)
{
	idassert(m_window);
	int displayIdx = 0;
	if (parms.fullScreen > 0)
	{
		displayIdx = parms.fullScreen - 1; // first display for SDL is 0, in parms it's 1
	}
	else // -2 == use current display
	{
		displayIdx = m_window->GetCurrDisplay();
		if (displayIdx < 0) // for some reason the display for the window couldn't be detected
		{
			displayIdx = 0;
		}
	}

	if (parms.fullScreen > SDL_GetNumVideoDisplays())
	{
		common->Warning("Can't set fullscreen mode to display number %i, because SDL2 only knows about %i displays!",
			parms.fullScreen, SDL_GetNumVideoDisplays());
		return -1;
	}

	if (parms.fullScreen != rbConfig.isFullscreen)
	{
		// we have to switch to another display
		if (rbConfig.isFullscreen)
		{
			// if we're already in fullscreen mode but want to switch to another monitor
			// we have to go to windowed mode first to move the window.. SDL-oddity.
			m_window->FullScreen(false);
		}

		m_window->SetDisplay(displayIdx);

	}

	return displayIdx;
}

bool btWindowManager::SetScreenParms(screenParms_t parms)
{
	idassert(m_window);
	if (parms.fullScreen > 0 || parms.fullScreen == -2)
	{
		SDL_DisplayMode m = { 0 };
		int displayIdx = ScreenParmsHandleDisplayIndex(parms);
		if (displayIdx < 0)
			return false;

		// get current mode of display the window should be full-screened on
		SDL_GetCurrentDisplayMode(displayIdx, &m);

		// change settings in that display mode according to parms
		// FIXME: check if refreshrate, width and height are supported?
		// m.refresh_rate = parms.displayHz;
		m.w = parms.width;
		m.h = parms.height;

		// set that displaymode
		if (SDL_SetWindowDisplayMode((SDL_Window*)m_window->GetHandler(), &m) < 0)
		{
			common->Warning("Couldn't set window mode for fullscreen, reason: %s", SDL_GetError());
			return false;
		}

		// if we're currently not in fullscreen mode, we need to switch to fullscreen
		if (!m_window->IsFullScreen())
		{
			if (m_window->FullScreen(true))
			{
				common->Warning("Couldn't switch to fullscreen mode, reason: %s!", SDL_GetError());
				return false;
			}
		}
	}
	else if (parms.fullScreen == 0) // windowed mode
	{
		m_window->Resise(parms.width, parms.height);
		SDL_SetWindowPosition((SDL_Window*)m_window->GetHandler(), parms.x, parms.y);

		// if we're currently in fullscreen mode, we need to disable that
		if (m_window->IsFullScreen())
		{
			if (m_window->FullScreen(false))
			{
				common->Warning("Couldn't switch to windowed mode, reason: %s!", SDL_GetError());
				return false;
			}
		}
	}
	else
	{
		common->Warning("GLimp_SetScreenParms: fullScreen -1 (borderless window for multiple displays) currently unsupported!");
		return false;
	}

	return true;
}
#endif

/*
==============
btWindow::btWindow
==============
*/
btWindow::btWindow( void ) : 
	m_mouseGrab( false ),
	m_handler( nullptr )
{
}

/*
==============
btWindow::~btWindow
==============
*/
btWindow::~btWindow( void )
{
	Destroy();
}

/*
==============
btWindow::Create
==============
*/
bool btWindow::Create( const char * title, const SDL_Rect windowBounds, Uint32 flags )
{
	m_handler = SDL_CreateWindow( title, windowBounds.x, windowBounds.y, windowBounds.w, windowBounds.h, flags );
	if (!m_handler)
	{
		common->Error( "Error on SDL_CreateWindow(%s)", SDL_GetError() );
		return false;
	}
	
	return true;
}

/*
==============
btWindow::Create
==============
*/
void btWindow::Destroy( void )
{
	if (m_handler != nullptr )
	{
		SDL_DestroyWindow( m_handler );
		m_handler = nullptr;
	}

	m_mouseGrab = false;
}

/*
==============
btWindow::UpdateBounds
==============
*/
void btWindow::UpdateBounds( const SDL_Rect windowBounds )
{
	assert( m_handler != nullptr );
	SDL_SetWindowPosition( m_handler, windowBounds.x, windowBounds.y );
	SDL_SetWindowSize( m_handler, windowBounds.w, windowBounds.h );
}

/*
==============
btWindow::UpdateBounds
==============
*/
void btWindow::GetBounds( SDL_Rect &windowBounds )
{
	assert( m_handler != nullptr );
	SDL_GetWindowSize( m_handler, &windowBounds.w, &windowBounds.h );
	SDL_GetWindowPosition( m_handler, &windowBounds.x, &windowBounds.y );
}

/*
==============
btWindow::GrabMouseCursor
==============
*/
void btWindow::GrabMouseCursor( const bool grabIt )
{
	assert( m_handler != nullptr );

	//free cursor whe show console
	if (grabIt)
		SDL_ShowCursor(SDL_DISABLE);
	else
		SDL_ShowCursor(SDL_ENABLE);

	// DG: check for GRAB_ENABLE instead of GRAB_HIDECURSOR because we always wanna hide it
	SDL_SetRelativeMouseMode(grabIt ? SDL_TRUE : SDL_FALSE);
	SDL_SetWindowGrab( m_handler, grabIt ? SDL_TRUE : SDL_FALSE);
}

/*
==============
btWindow::IsFlagActive
==============
*/
const bool btWindow::IsFlagActive( const Uint32 flag )
{
	assert( m_handler != nullptr );
	return (SDL_GetWindowFlags( m_handler ) & flag) == flag;
}

/*
==============
btWindow::TogleFullScreen
==============
*/
const bool btWindow::TogleFullScreen( const bool togle, const bool desktop )
{
	assert( m_handler != nullptr );
	const bool isFullScreen = false;
	const Uint32 fsMode = (desktop) ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;

	const bool isFullScreen = IsFlagActive( fsMode );
	if (togle != isFullScreen)
	{
		if (SDL_SetWindowFullscreen( m_handler, togle ? fsMode : SDL_FALSE ) != 0)
			common->Warning( "Can't togle window to fullscren" );
	}

	return isFullScreen;
}

/*
==============
btWindow::SetFocusWindow
==============
*/
const bool btWindow::SetFocus( void )
{
	assert( m_handler != nullptr );
	const bool hasFocus = IsFlagActive( SDL_WINDOW_INPUT_FOCUS );
	if( !hasFocus ) SDL_RaiseWindow( m_handler );
	return false;
}

/*
==============
btWindow::ShowWindow
==============
*/
const bool btWindow::Show( void )
{
	assert( m_handler != nullptr );
	const bool isShow = IsFlagActive( SDL_WINDOW_SHOWN );
	if (!isShow) SDL_ShowWindow( m_handler );

	return isShow;
}

/*
==============
btWindow::HideWindow
==============
*/
const bool btWindow::Hide( void )
{
	assert( m_handler != nullptr );
	const bool isHide = IsFlagActive( SDL_WINDOW_HIDDEN );
	if (!isHide) SDL_HideWindow( m_handler );
	return isHide;
}

/*
==============
btWindow::HideWindow
==============
*/
const int btWindow::GetDisplayID( void )
{
	assert( m_handler != nullptr );
	return SDL_GetWindowDisplayIndex( m_handler );
}

/*
==============
btWindow::HideWindow
==============
*/
const int btWindow::SetDisplay( const int index )
{
	assert( m_handler != nullptr );

	// select display ; SDL_WINDOWPOS_UNDEFINED_DISPLAY() doesn't work.
	int x = SDL_WINDOWPOS_CENTERED_DISPLAY( index );
	// move window to the center of selected display
	SDL_SetWindowPosition( m_handler, x, x );
	return 0;
}

const int btWindow::Id( void ) const
{
	assert( m_handler != nullptr );
	return SDL_GetWindowID( m_handler );
}

SDL_Window * btWindow::GetHandler( void ) const
{
	return m_handler;
}