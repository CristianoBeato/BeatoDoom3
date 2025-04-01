/*
===========================================================================

Beato idTech 4 Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2016-2025 Cristiano B. Santos <cristianobeato_dm@hotmail.com>.

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

#include "precompiled.h"
#pragma hdrstop

#include "paths.h"
#include <SDL3/SDL_filesystem.h>

#include <filesystem>
namespace fs = std::filesystem;

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#endif // __linux__

idCVar sys_basepath("sys_basepath", "", CVAR_SYSTEM | CVAR_INIT, "engine base data path");
idCVar sys_savepath("sys_savepath", "", CVAR_SYSTEM | CVAR_INIT, "user base save path");
idCVar sys_currentWorkDir("sys_currentWorkDir", "", CVAR_SYSTEM | CVAR_INIT, "current engine run location");

// SDL_GetBasePath
// SDL_GetPrefPath
// SDL_CreateDirectory
// SDL_RemovePath
// SDL_RenamePath
// SDL_CopyFile
// SDL_GetPathInfo
// SDL_GlobDirectory

/*
==============
Sys_Cwd
==============
*/
const char *Sys_Cwd( void )
{
	if( strcmp( sys_currentWorkDir.GetString(), "" ) == 0 )
	{
		// get current work dir and store on a cvar
		const char* cwd = SDL_GetCurrentDirectory();
		if (cwd)
		{
			sys_currentWorkDir.SetString( cwd );
			SDL_free( (void*)cwd );
		}
		else
		{
			common->Error( "Sys_Cwd: %s\n", SDL_GetError() );
			return "./";
		}
	}

	// return current work dir
	return sys_currentWorkDir.GetString();
}

/*
==============
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char *path )
{
	// check if path already exist
	if ( Sys_PathExist( path ) )
		return;
	
	// create the directory
	if ( !SDL_CreateDirectory( path ) )
		common->Error( "Sys_Mkdir: %s\n", SDL_GetError() );
}

/*
==============
Sys_DefaultCDPath
==============
*/
const char *Sys_DefaultCDPath( void )
{
	return ""; // ingonore
}

/*
==============
Sys_DefaultBasePath
==============
*/
const char *Sys_DefaultBasePath( void )
{
	if ( strcmp( sys_basepath.GetString(), "" ) == 0 )
	{
		const char* base_path = SDL_GetBasePath();
		if (base_path)
		{
			sys_basepath.SetString( base_path );
			SDL_free( (void*)base_path );
		}
		else
		{
			common->Error( "Sys_DefaultBasePath: %s\n", SDL_GetError() );
			return "./";
		}

	}

	return sys_basepath.GetString();
}

/*
==============
Sys_DefaultSavePath
==============
*/
const char *Sys_DefaultSavePath( void )
{
	if ( strcmp( sys_savepath.GetString(), "" ) == 0 )
	{
		const char* save_path = SDL_GetPrefPath( "Beato", "idTech4" );
		if (save_path)
		{
			sys_savepath.SetString( save_path );
			SDL_free( (void*)save_path );
		}
		else
		{
			common->Error( "Sys_DefaultSavePath: %s\n", SDL_GetError() );
			return "./";
		}
	}
	
	return sys_savepath.GetString();
}

/*
==============
Sys_EXEPath
==============
*/
const char *Sys_EXEPath( void )
{
	static char	buf[ MAX_OSPATH ];
#ifdef __linux__
	idStr		linkpath;
	int			len;

	buf[ 0 ] = '\0';
	sprintf( linkpath, "/proc/%d/exe", getpid() );
	len = readlink( linkpath.c_str(), buf, sizeof( buf ) );
	if ( len == -1 ) 
	{
		Sys_Printf("couldn't stat exe path link %s\n", linkpath.c_str());
		buf[ len ] = '\0';
	}
#elif(WIN32)
	GetModuleFileName( NULL, buf, sizeof( buf ) - 1 );
#endif
	return buf;
}

bool Sys_PathExist( const char* path )
{
	SDL_PathInfo info;
	if ( SDL_GetPathInfo( path, &info ) )
		return info.type == SDL_PATHTYPE_DIRECTORY || info.type == SDL_PATHTYPE_FILE;

	return false;
}

/*
==============
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idList<class idStr> &list )
{
	int count = 0;
	char ** dirs = SDL_GlobDirectory( directory, extension, SDL_GLOB_CASEINSENSITIVE, &count );
	for ( int i = 0; i < count; i++)
	{
		list.Append( dirs[i] );
	}
	
	SDL_free( dirs );

	return list.Size();
}


/*
===========
Sys_GetDriveFreeSpace
return in MegaBytes
===========
*/
uint64_t Sys_GetDriveFreeSpace( const char *directory )
{
	static const uint32_t k_MEGABYTE = 1048576; 

	std::error_code err;
	const std::filesystem::space_info si = std::filesystem::space( fs::path( directory ), err );
	if (err.value() != 0)
	{
		common->Error( " Sys_GetDriveFreeSpace Fail: %s\n", err );
		return 0;
	}

	return si.free / k_MEGABYTE;
}