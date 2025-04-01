


#include "idlib/precompiled.h"
#pragma hdrstop

#include <SDL3/SDL_video.h>
#include "SDL3/SDL_window.hpp"

#include "sys/sys_public.h"
#include "sys_public.h"

static struct videoInfo_t
{
    uint32_t            windowID;
    SDLWindow			windowHandle;
    idList<vidmode_t>	vidModes;
	uint16_t			storedGammaRamp[3][256];
}video;

/*
==============
ListVideoModes
==============
*/
static void ListVideoModes( void )
{
    int count = 0;
	int modeCount = 0;
	int displayCount = 0;
	SDL_Rect displayBounds{};
	vidmode_t vidMode;

	// get the number of displays
	SDL_DisplayID* displayModes = SDL_GetDisplays( &displayCount );
	for ( int i = 0; i < displayCount; i++)
	{
		SDL_DisplayID display = displayModes[i];
		
		// get the display bounds
		if ( !SDL_GetDisplayBounds( display, &displayBounds ) )
			continue;
		
		// get the display name
		const char* displayName = SDL_GetDisplayName( display );

		SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes( display, &modeCount );
		for ( int  j = 0; j < modeCount; j++)
		{
			SDL_DisplayMode* mode = modes[j];
			if ( !mode )
				continue;

			vidMode.display = display;
			vidMode.width = mode->w;
			vidMode.height = mode->h;
			vidMode.refreshRate = mode->refresh_rate;
			SDL_snprintf( vidMode.description, 256, "Display %i: %s - %ix%i", count++, displayName, vidMode.width, vidMode.height );
			video.vidModes.Append( vidMode );
		}
	}
}

/*
==============
DestroyWindow
==============
*/
static void DestroyWindow( void )
{
	// restore gamma
    video.windowHandle.Destroy();
}

/*
==============
CreateWindow
==============
*/
static void CreateWindow( void )
{
	DestroyWindow();

	if ( !video.windowHandle.Create( GAME_NAME, 640, 480, SDL_WINDOW_HIDDEN ) )
		Sys_Error( "SDL_CreateWindow, SDL error: %s\n", SDL_GetError() );

	video.windowID = video.windowHandle.GetID();

	// Store defalt window gamma
//	video.windowHandle->GetGamma( video.storedGammaRamp[0], video.storedGammaRamp[1], video.storedGammaRamp[2] );
}

/*
==============
Sys_videoStartUp
==============
*/
void Sys_videoStartUp( void )
{
    ListVideoModes();
    CreateWindow();
}

/*
==============
Sys_videoShutDown
==============
*/
void Sys_videoShutDown( void )
{
    DestroyWindow();
    video.vidModes.Clear();
}

/*
==============
Sys_videoWindowHandler
==============
*/
void *Sys_videoWindowHandler(void)
{
    return static_cast<void*>( video.windowHandle.GetHandle() );
}

/*
==============
Sys_videoWindowID
==============
*/
uint32_t Sys_videoWindowID(void)
{
    return video.windowID;
}

/*
==============
Sys_videoFullScreen
==============
*/
void Sys_videoFullScreen( const int mode )
{
    Uint32 flags = 0;
	
	switch ( mode )
	{
	case 1:
		flags = SDL_WINDOW_FULLSCREEN; 
		break;
	case 2:
		flags = SDL_WINDOW_BORDERLESS;
		break;
	default:
		flags = 0;
		break;
	}

	//video.windowHandle.
	//video.windowHandle->Fullscreen( flags );
}

/*
==============
Sys_videoWindowHandler
==============
*/
void Sys_videoWindowSize( const int display, const uint32_t width, const uint32_t height )
{
	video.windowHandle.SetSize( width, height );
	video.windowHandle.SetPosition( SDL_WINDOWPOS_CENTERED_DISPLAY( display ), SDL_WINDOWPOS_CENTERED_DISPLAY( display ) );
}

/*
==============
Sys_videoWindowHandler
==============
*/
uint32_t Sys_videoNumModes(void)
{
    return video.vidModes.Num();
}

vidmode_t *Sys_videoSuportedModes(void)
{
    return video.vidModes.Ptr();
}

/*
==============
Sys_ShowWindow
==============
*/
void Sys_videoShowWindow( bool show )
{
    if ( show )
		video.windowHandle.Show();
	else
		video.windowHandle.Hide();
}

/*
==============
Sys_IsWindowVisible
==============
*/
bool Sys_videoIsWindowVisible( void )
{
	// check if window is visible
    return ( video.windowHandle.GetFlags()  & SDL_WINDOW_HIDDEN ) == 0;
}

/*
==============
Sys_videoSetWindowFocus
==============
*/
void Sys_videoSetWindowFocus( void )
{
	const bool hasFocus = video.windowHandle.GetFlags() & SDL_WINDOW_INPUT_FOCUS;
	if( !hasFocus ) 
		video.windowHandle.Raise();
}