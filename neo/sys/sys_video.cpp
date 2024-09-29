


#include "idlib/precompiled.h"
#pragma hdrstop

#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>

#include "sys/sys_public.h"
#include "sys_public.h"

class crWindow
{
public:
	crWindow( const char* name, const uint32_t width, const uint32_t height ) : m_window( nullptr )
	{
#if CR_USE_VULKAN 
		uint32_t windowFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_VULKAN;
#else
		uint32_t windowFlags = SDL_WINDOW_HIDDEN | SDL_WINDOW_MOUSE_CAPTURE | SDL_WINDOW_OPENGL;
#endif // OpenGL

		m_window = SDL_CreateWindow( GAME_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags );
		if( !m_window  )
		{
			Sys_Error( SDL_GetError() );
			delete this;
		}
	}
	
	inline ~crWindow( void )
	{
		if( m_window != nullptr )
		{
			SDL_DestroyWindow( m_window );
			m_window = nullptr;
		}
	}

	inline uint32_t GetID( void ) const { return SDL_GetWindowID( m_window ); }
	inline uint32_t Flags( void ) const { return SDL_GetWindowFlags( m_window ); }
	inline void 	Show( void ) { SDL_ShowWindow( m_window ); }
	inline void 	Hide( void ){ SDL_HideWindow( m_window ); }
	inline void 	Raise( void ) { SDL_RaiseWindow( m_window ); }
	inline void		Fullscreen( uint32_t flags ) { SDL_SetWindowFullscreen( m_window, flags ); }
	inline void		GetGamma( uint16_t* red, uint16_t *green, uint16_t *blue ) { SDL_GetWindowGammaRamp( m_window, red, green, blue ); }
	inline void		SetGamma( uint16_t* red, uint16_t *green, uint16_t *blue ) { SDL_SetWindowGammaRamp( m_window, red, green, blue ); }
    
	inline operator SDL_Window*( void ) const { return m_window; }
private:
	SDL_Window* m_window;
};

static struct videoInfo_t
{
    uint32_t            windowID;
    crWindow*			windowHandle;
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
	displayCount = SDL_GetNumVideoDisplays();
	for ( int display = 0; display < displayCount; display++)
	{
		const char* displayName = SDL_GetDisplayName( display );
		if ( SDL_GetDisplayBounds( display, &displayBounds ) != 0 )
			continue;
		
		modeCount = SDL_GetNumDisplayModes( display );
		for ( int mode = 0; mode < modeCount; mode++)
		{
			SDL_DisplayMode SDLmode;
			if( SDL_GetDisplayMode( display, mode, &SDLmode ) != 0 )
				continue;

			vidMode.display = display;
			vidMode.width = SDLmode.w;
			vidMode.height = SDLmode.h;
			vidMode.refreshRate = SDLmode.refresh_rate;

			SDL_snprintf( vidMode.description, 256, "Mode  %i: display %i - %ix%i", count, vidMode.display, vidMode.width, vidMode.height );

			video.vidModes.Append( vidMode );

			count++;
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
    if ( video.windowHandle != nullptr )
	{
		delete video.windowHandle;	
		video.windowHandle = nullptr;
	}
}

/*
==============
CreateWindow
==============
*/
static void CreateWindow( void )
{
	if ( video.windowHandle != nullptr )
		DestroyWindow();

	video.windowHandle = new crWindow( GAME_NAME, 640, 480 ); 
	if ( !video.windowHandle )
		Sys_Error( "SDL_CreateWindow, SDL error: %s\n", SDL_GetError() );

	video.windowID = video.windowHandle->GetID();

	// Store defalt window gamma
	video.windowHandle->GetGamma( video.storedGammaRamp[0], video.storedGammaRamp[1], video.storedGammaRamp[2] );
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
    return static_cast<void*>( *video.windowHandle );
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
		flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
		break;
	default:
		flags = 0;
		break;
	}

	video.windowHandle->Fullscreen( flags );
}

/*
==============
Sys_videoWindowHandler
==============
*/
void Sys_videoWindowSize( const int display, const uint32_t width, const uint32_t height )
{
    SDL_SetWindowPosition( *video.windowHandle, SDL_WINDOWPOS_CENTERED_DISPLAY( display ), SDL_WINDOWPOS_CENTERED_DISPLAY( display ) );
	SDL_SetWindowSize( *video.windowHandle, width, height );
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
		video.windowHandle->Show();
	else
		video.windowHandle->Hide();
}

/*
==============
Sys_IsWindowVisible
==============
*/
bool Sys_videoIsWindowVisible( void )
{
    return ( video.windowHandle->Flags()  & SDL_WINDOW_SHOWN );
}

/*
==============
Sys_videoSetWindowFocus
==============
*/
void Sys_videoSetWindowFocus( void )
{
	const bool hasFocus = video.windowHandle->Flags() & SDL_WINDOW_INPUT_FOCUS;
	if( !hasFocus ) 
		video.windowHandle->Raise();
}