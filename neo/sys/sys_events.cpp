/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2022 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

This file is part of the Beato idTech 4  GPL Source Code (?Beato idTech 4  Source Code?).

Beato idTech 4  Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato idTech 4  Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato idTech 4  Source Code.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#include "idlib/precompiled.h"
#pragma hdrstop

//
//
//
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_gamecontroller.h>

#include "sdl2_scancode_mappings.h"


//#include "Console.h"
//#include "EventIput.h"

//			SDL_GameControllerNameForIndex();
//			SDL_GameControllerTypeForIndex();
//			SDL_GameControllerOpen();
//			SDL_GameControllerClose();
//			SDL_GameControllerEventState();
//			SDL_GameControllerUpdate();
//			SDL_GameControllerRumble();
//			SDL_GameControllerRumbleTriggers();
//			SDL_GameControllerHasLED();
//			SDL_GameControllerHasRumble();
//			SDL_GameControllerHasRumbleTriggers();
//			SDL_GameControllerSetLED();

class crGameControler
{
public:
	crGameControler( void );
	~crGameControler( void );

	void	Init( int index );
	void	ShutDown( void );
	void	SetRumble( const uint16_t low, const uint16_t hig, const uint32_t duration );
	void	SetLedColors( const uint8_t red, const uint8_t green, const uint8_t blue );

private:
	bool				m_rumbleSuport;
	bool				m_ledSuport;
	int					m_controlerIndex;
	SDL_GameController*	m_controlerHandle;
};

crGameControler::crGameControler( void ) :
	m_controlerIndex( 0 ),
	m_controlerHandle( nullptr )
{
}

crGameControler::~crGameControler( void )
{
}

void crGameControler::Init(int index)
{
	m_controlerIndex = index;
	m_controlerHandle = SDL_GameControllerOpen( m_controlerIndex );
	m_rumbleSuport = SDL_GameControllerHasRumble( m_controlerHandle ) == SDL_TRUE;
	m_ledSuport	= SDL_GameControllerHasLED( m_controlerHandle ) == SDL_TRUE;
}

void crGameControler::ShutDown(void)
{
	if ( m_controlerHandle == nullptr )
	{
		SDL_GameControllerClose( m_controlerHandle );
		m_controlerHandle = nullptr;
	}
	
}

void crGameControler::SetRumble( const uint16_t low, const uint16_t hig, const uint32_t duration )
{
	if ( !m_rumbleSuport )
		return;

	SDL_GameControllerRumble( m_controlerHandle, low, hig, duration );
}

void crGameControler::SetLedColors( const uint8_t red, const uint8_t green, const uint8_t blue )
{
	SDL_GameControllerSetLED( m_controlerHandle, red, green, blue );
}

// DG Begin:
static int SDLScanCodeToKeyNum( SDL_Scancode sc )
{
	int idx = int( sc );
	assert( idx >= 0 && idx < SDL_NUM_SCANCODES );

	return scanCodeToKeyNum[idx];
}

static const int k_MAX_KEYBOART_EVENT = 64; // std::max 64 events per frame 
static const int k_MAX_MOUSE_EVENT = 64;  // std::max 64 events per frame 
static const int k_MAX_JOYSTIC_EVENT = 128; // max 128 events
static const int k_MAX_QUED_EVENTS = 256;
static const int k_MASK_QUED_EVENTS	= ( k_MAX_QUED_EVENTS - 1 );

struct perifericalEvent_t
{
	int action;	// action id
	int value;	// action valude
};

static struct sys_events_s
{
	uint32_t			eventHead;
	uint32_t			eventTail;
	uint32_t			gameControllersCount;
	uint32_t			keyboardEventCount;
	uint32_t			mouseEventCount;
	uint32_t			joysticEventCount;
	uint32_t			consoleKeyMappingIdx;
	sysEvent_t			eventQueue[k_MAX_QUED_EVENTS];
	perifericalEvent_t	keyboardEventList[k_MAX_KEYBOART_EVENT];
	perifericalEvent_t	mouseEventList[k_MAX_MOUSE_EVENT];
	perifericalEvent_t	joysticEventList[k_MAX_JOYSTIC_EVENT];
}inputEvents;

static struct consoleKeyMapping_t
{
	const char* langName;
	unsigned char key;
	unsigned char keyShifted;
} s_consoleKeyMappings[] = 
{
	{ "auto",   	 0 ,	0   }, // special case: set current keycode for SDL_SCANCODE_GRAVE (no shifted keycode, though)
	{ "english",	'`',	'~' },
	{ "french", 	'<',	'>' },
	{ "german", 	'^',	176 }, // °
	{ "italian",	'\\',	'|' },
	{ "spanish",	186,	170 }, // º ª
	{ "turkish",	'"',	233 }, // é
	{ "norwegian",	124,	167 }, // | §
	{ "brazilian",	'\'',	'"' }
};

// NOTE: g++-4.7 doesn't like when this is static (for idCmdSystem::ArgCompletion_String<kbdNames>)
const char *_in_kbdNames[] = 
{
	"auto", "english", "french", "german", "italian", "spanish", "turkish", "norwegian", "brazilian", nullptr
};

static idCVar in_ignoreConsoleKey("in_ignoreConsoleKey", "0", CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_NOCHEAT | CVAR_BOOL, "Console only opens with Shift+Esc, not ` or ^ etc");
static idCVar in_kbd("in_kbd", _in_kbdNames[0], CVAR_SYSTEM | CVAR_ARCHIVE | CVAR_NOCHEAT, "keyboard layout", _in_kbdNames, idCmdSystem::ArgCompletion_String<_in_kbdNames> );
// DG End

static void Mouse( const SDL_Event *evt );
static void Keyboard( const SDL_Event *evt );
static void Controler( const SDL_Event *evt );
static void AppendKeyboardEvent( const int ch, const int state );
static void AppendJoysticEvent( const int action, const int value );
static void AppendMouseEvent( const int action, const int value );
static void QueEvent( sysEventType_t type, int value, int value2, int ptrLength, void *ptr );
static void InitConsoleKeyMapping( void );

void Sys_InitInput(void)
{
	common->Printf( "\n------- Input Initialization -------\n" );
	// list controler input
	int numJoysticks = SDL_NumJoysticks();
	for ( int i = 0; i < numJoysticks; i++)
	{
		if (SDL_IsGameController(i))
			inputEvents.gameControllersCount++;
	}
}

void Sys_ShutdownInput(void)
{
}

void Sys_GenerateEvents(void)
{
	SDL_Event evt;
	const char *s = nullptr;

	if ( ( s = Sys_ConsoleInput() ) != nullptr )
	{
		char *b;
		int len;

		len = strlen( s ) + 1;
		b = (char *)Mem_Alloc( len );
		strcpy( b, s );
		QueEvent( SE_CONSOLE, 0, 0, len, b );
	}

	while ( SDL_PollEvent( &evt ) )
	{
		switch (evt.type)
		{
		case SDL_QUIT:
		case SDL_APP_TERMINATING:
		{	
			char* b = (char*)Mem_ClearedAlloc( 5 );
			b = (char*)"quit";
			QueEvent( SE_CONSOLE, 0, 0, 5, (void*)b );
		} break;

		// Window event input 
		case SDL_WINDOWEVENT:
		{
		}break;

		// Keyboard input 
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			Keyboard( &evt );
		}break;

		// Text input
		case SDL_TEXTEDITING:
		case SDL_TEXTINPUT:
		{

		}break;

		// Mouse Input
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEWHEEL:
		{
			Mouse( &evt );
		}break;

//		case SDL_CONTROLLERDEVICEADDED:
//		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERAXISMOTION:
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERTOUCHPADDOWN:
		case SDL_CONTROLLERTOUCHPADMOTION:
		case SDL_CONTROLLERTOUCHPADUP:
		case SDL_CONTROLLERSENSORUPDATE:
		{
			Controler( &evt );
		}break;
		default:
			break;
		}
	}
	
}

sysEvent_t Sys_GetEvent( void )
{
	sysEvent_t ev;

	// return if we have data
	if ( inputEvents.eventHead > inputEvents.eventTail) 
	{
		inputEvents.eventTail++;
		return inputEvents.eventQueue[ ( inputEvents.eventTail - 1 ) & k_MASK_QUED_EVENTS ];
	}
	// return the empty event with the current time
	memset(&ev, 0, sizeof(ev));

	return ev;
}

void Sys_ClearEvents( void )
{
	inputEvents.eventHead = 0;
	inputEvents.eventTail = 0;
}

void Sys_InitScanTable( void )
{
#if 0 //TODO: 
	idStr lang = cvarSystem->GetCVarString( "sys_lang" );
	if ( lang.Length() == 0 ) 
	{
		lang = "english";
	}
	
	if ( lang.Icmp( "english" ) == 0 ) 
	{
		keyScanTable = s_scantokey;
		// the only reason that english right alt binds as K_ALT is so that 
		// users who were using right-alt before the patch don't suddenly find
		// that only left-alt is working.
		rightAltKey = K_ALT;
	} 
	else if ( lang.Icmp( "spanish" ) == 0 ) 
	{
		keyScanTable = s_scantokey_spanish;
		rightAltKey = K_RIGHT_ALT;
	} 
	else if ( lang.Icmp( "french" ) == 0 ) 
	{
		keyScanTable = s_scantokey_french;
		rightAltKey = K_RIGHT_ALT;
	} 
	else if ( lang.Icmp( "german" ) == 0 ) 
	{
		keyScanTable = s_scantokey_german;
		rightAltKey = K_RIGHT_ALT;
	} 
	else if ( lang.Icmp( "italian" ) == 0 ) 
	{
		keyScanTable = s_scantokey_italian;
		rightAltKey = K_RIGHT_ALT;
	}
#endif
}


const unsigned char*	Sys_GetScanTable( void )
{
    return nullptr; //TODO:
}

unsigned char Sys_GetConsoleKey( bool shifted )
{
    if ( in_ignoreConsoleKey.GetBool() )
        return 0;

    if ( in_kbd.IsModified() ) 
    {
		InitConsoleKeyMapping();
		in_kbd.ClearModified();
	}

	return shifted ? s_consoleKeyMappings[inputEvents.consoleKeyMappingIdx].keyShifted : s_consoleKeyMappings[inputEvents.consoleKeyMappingIdx].key;
}

unsigned char Sys_MapCharForKey( int key )
{
    return key & 0xFF;
}

void Sys_GrabMouseCursor( bool grabIt )
{
    SDL_Window* window = static_cast<SDL_Window*>( Sys_videoWindowHandler() );

    SDL_ShowCursor( (grabIt) ? SDL_DISABLE : SDL_ENABLE );
//	SDL_SetRelativeMouseMode( (flags & GRAB_RELATIVEMOUSE) ? SDL_TRUE : SDL_FALSE );
	SDL_SetWindowGrab( window, grabIt ? SDL_TRUE : SDL_FALSE );
}

int Sys_PollKeyboardInputEvents(void)
{
    return inputEvents.keyboardEventCount;
}

int Sys_ReturnKeyboardInputEvent(const int n, int &ch, bool &state)
{
    if ( n >= inputEvents.keyboardEventCount )
		return 0;

	ch = inputEvents.keyboardEventList[n].action;
	state = (bool)inputEvents.keyboardEventList[n].value;
	return 1;
}

void Sys_EndKeyboardInputEvents(void)
{
    inputEvents.keyboardEventCount = 0;
}

int Sys_PollMouseInputEvents(void)
{
    return inputEvents.mouseEventCount;
}

int Sys_ReturnMouseInputEvent( const int n, int &action, int &value )
{
    if ( n >= inputEvents.mouseEventCount )
		return 0;

	action = inputEvents.mouseEventList[n].action;
	value = inputEvents.mouseEventList[n].value;
	return 1;
}

void Sys_EndMouseInputEvents(void)
{
    inputEvents.mouseEventCount = 0;
}

int Sys_PollJoystickInputEvents(void)
{
    return inputEvents.joysticEventCount;
}

int Sys_ReturnJoystickInputEvent( const int n, int &action, int &value )
{
    if ( n >= inputEvents.joysticEventCount )
		return 0;

	action = inputEvents.joysticEventList[n].action;
	value = inputEvents.joysticEventList[n].value;
	return 1;
}

void Sys_EndJoystickInputEvents(void)
{
    inputEvents.joysticEventCount = 0;
}

void Mouse( const SDL_Event *evt )
{
	switch ( evt->type )
	{
	case SDL_MOUSEMOTION:
	{
		// append a event 
		QueEvent( SE_MOUSE, evt->motion.xrel, evt->motion.yrel, 0, nullptr );

		//
		AppendMouseEvent( M_DELTAX, evt->motion.xrel );
		AppendMouseEvent( M_DELTAY, evt->motion.yrel );
	} break;
	case SDL_MOUSEWHEEL:
	{
		// append a event 
		QueEvent( SE_KEY, ( evt->wheel.y > 0 ) ? K_MWHEELUP : K_MWHEELDOWN, 0, 0, nullptr );

		//
		AppendMouseEvent( M_DELTAZ, evt->wheel.y );
	} break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	{
		int button = 0;
		int state = 0;
		
		// TODO: suport for more mouse buttons 
		switch( evt->button.button )
		{
			case SDL_BUTTON_LEFT:
				button = K_MOUSE1;
				break;
			case SDL_BUTTON_MIDDLE:
				button = K_MOUSE3;
				break;
			case SDL_BUTTON_RIGHT:
				button = K_MOUSE2;
				break;
		}

		state = evt->button.state == SDL_PRESSED ? 1 : 0;

		// append system common event 
		QueEvent( SE_KEY, button, state, 0, nullptr );
		
		// game input event
		AppendMouseEvent( button, state );
	}
	}
}

void Keyboard( const SDL_Event *evt )
{
	int key = SDLScanCodeToKeyNum( evt->key.keysym.scancode );
	int state = ( evt->key.state == SDL_PRESSED ) ? 1 : 0;
	if( key == 0 )
	{
		// SDL2 has no ev.key.keysym.unicode anymore.. but the scancode should work well enough for console
		if( evt->type == SDL_KEYDOWN ) // FIXME: don't complain if this was an ASCII char and the console is open?
			common->Warning( "unmapped SDL key %d scancode %d", evt->key.keysym.sym, evt->key.keysym.scancode );

		return; // just handle next event
	}

	// append system common event 
	QueEvent( SE_KEY, key, state, 0, nullptr );

	//
	AppendKeyboardEvent( key, state );
}

void Controler(const SDL_Event *evt)
{
}

void AppendKeyboardEvent(const int ch, const int state)
{
	if ( inputEvents.keyboardEventCount >= k_MAX_MOUSE_EVENT ) 
	{
		common->DPrintf("WARNING: reached MAX_MOUSE_EVENT keyboardEventCount\n");
		return;
	}

    inputEvents.keyboardEventList[inputEvents.keyboardEventCount].action = ch;
    inputEvents.keyboardEventList[inputEvents.keyboardEventCount].value = state;
    inputEvents.keyboardEventCount++;
}

void AppendMouseEvent( const int action, const int value )
{
	if ( inputEvents.mouseEventCount >= k_MAX_MOUSE_EVENT ) 
	{
		common->DPrintf("WARNING: reached MAX_MOUSE_EVENT mouseEventCount\n");
		return;
	}

    inputEvents.mouseEventList[inputEvents.mouseEventCount].action = action;
    inputEvents.mouseEventList[inputEvents.mouseEventCount].value = value;	
	inputEvents.mouseEventCount++;
}

void AppendJoysticEvent( const int action, const int value )
{
	if ( inputEvents.joysticEventCount >= k_MAX_JOYSTIC_EVENT ) 
	{
		common->DPrintf("WARNING: reached MAX_JOYSTIC_EVENT joysticEventCount\n");
		return;
	}

    inputEvents.joysticEventList[inputEvents.joysticEventCount].action = action;
    inputEvents.joysticEventList[inputEvents.joysticEventCount].value = value;
	inputEvents.joysticEventCount++;
}

void InitConsoleKeyMapping(void)
{
    const int numMappings = sizeof(s_consoleKeyMappings)/sizeof(s_consoleKeyMappings[0]);

	idStr lang = in_kbd.GetString();
	inputEvents.consoleKeyMappingIdx = 0;

	s_consoleKeyMappings[0].key = 0;
	if ( lang.Length() == 0 || lang.Icmp( "auto") == 0 ) 
    {
		// auto-detection (SDL2-only)
		int keycode = SDL_GetKeyFromScancode( SDL_SCANCODE_GRAVE );
		if ( keycode > 0 && keycode <= 0xFF ) 
        {
			// the SDL keycode and dhewm3 keycode should be identical for the mappings,
			// as it's ISO-8859-1 ("High ASCII") chars
			for( int i=1; i<numMappings; ++i ) 
            {
				if ( s_consoleKeyMappings[i].key == keycode ) 
                {
					inputEvents.consoleKeyMappingIdx = i;
					common->Printf( "Detected keyboard layout as \"%s\"\n", s_consoleKeyMappings[i].langName );
					break;
				}
			}

			if ( inputEvents.consoleKeyMappingIdx == 0 ) 
            { // not found in known mappings
				s_consoleKeyMappings[0].key = keycode;
			}
		}
	} 
    else
	{
		for( int i=1; i<numMappings; ++i ) 
        {
			if( lang.Icmp( s_consoleKeyMappings[i].langName ) == 0 ) 
            {
				inputEvents.consoleKeyMappingIdx = i;
				int keycode = SDL_GetKeyFromScancode( SDL_SCANCODE_GRAVE );
				if ( keycode && keycode != s_consoleKeyMappings[i].key ) 
                {
					common->Warning( "in_kbd is set to \"%s\", but the actual keycode of the 'console key' is %c (%d), not %c (%d), so this might not work that well..\n",
							lang.c_str(), (unsigned char)keycode, keycode, s_consoleKeyMappings[i].key, s_consoleKeyMappings[i].key );
				}
				break;
			}
		}
	}
}

/*
================
Posix_QueEvent

ptr should either be null, or point to a block of data that can be freed later
================
*/
void QueEvent(sysEventType_t type, int value, int value2, int ptrLength, void *ptr)
{
	sysEvent_t *ev;

	ev = &inputEvents.eventQueue[inputEvents.eventHead & k_MASK_QUED_EVENTS];
	if ( inputEvents.eventHead - inputEvents.eventTail >= k_MAX_QUED_EVENTS ) 
	{
		common->Printf( "Posix_QueEvent: overflow\n" );
		// we are discarding an event, but don't leak memory
		// TTimo: verbose dropped event types?
		if (ev->evPtr) 
		{
			Mem_Free(ev->evPtr);
			ev->evPtr = nullptr;
		}
		inputEvents.eventTail++;
	}

	inputEvents.eventHead++;

	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}