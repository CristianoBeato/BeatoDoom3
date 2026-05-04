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

#include <SDL3/SDL_filesystem.h>

#include "sys_platform.h"
#include "sys_main.h"

#if SDL_PLATFORM_LINUX
#	include <sys/stat.h>
#endif

/*
==============
Sys_Cwd
==============
*/
const char *Sys_Cwd( void )
{
	static idStr k_cwdPath = idStr();
	if( k_cwdPath.IsEmpty() )
	{
		auto currentWorkingDirectory = SDL_GetCurrentDirectory();
		k_cwdPath = currentWorkingDirectory;
		SDL_free( currentWorkingDirectory );
	}
	
	return k_cwdPath.c_str();
}

/*
==============
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char *path )
{
	if( !SDL_CreateDirectory( path ) )
		idLib::Error( SDL_GetError() );
}

/*
=================
Sys_FileTimeStamp
=================
*/
ID_TIME_T Sys_FileTimeStamp( FILE *fp )
{
#if SDL_PLATFORM_WINDOWS
	struct _stat st;
	_fstat( _fileno( fp ), &st );
	return (long)st.st_mtime;
#else
	struct stat64 st;
	fstat64( fileno( fp ), &st );
	return st.st_mtime;
#endif
}

/*
==============
Sys_DefaultCDPath
==============
*/
const char *Sys_DefaultCDPath( void )
{
	return "";
}

/*
==============
Sys_DefaultBasePath
==============
*/
const char *Sys_DefaultBasePath( void )
{
	static idStr k_basePath = idStr();
	if ( k_basePath.IsEmpty() )
	{
		auto save_path = SDL_GetBasePath();
		if (save_path)
		{
			k_basePath = SDL_strdup( save_path );
			SDL_free( const_cast<char*>( save_path ) );
		}
		else
			k_basePath = Sys_Cwd();

		k_basePath.BackSlashesToSlashes();
		
		// Remove the trailing slash if present to maintain the idTech 4 standard
		// (Ex: "C:/Doom3/" becomes "C:/Doom3")
        if ( k_basePath.Length() > 0 && k_basePath[k_basePath.Length() - 1] == '/' )
            k_basePath.StripTrailing( '/' );
	}

	return k_basePath.c_str();
}

/*
==============
Sys_DefaultSavePath
==============
*/
const char *Sys_DefaultSavePath( void )
{
	static idStr k_savePath = idStr();
	if ( k_savePath.IsEmpty() )
	{
		char* save_path = SDL_GetPrefPath( "BeatoSoftware", "BeatoD3" );
		if (save_path)
		{
			k_savePath = SDL_strdup( save_path );
			SDL_free( save_path );
		}
		else // if can't get a valid save path, save in game folder 
			k_savePath = Sys_DefaultBasePath();

		k_savePath.BackSlashesToSlashes();

		// Remove the trailing slash if present to maintain the idTech 4 standard
		// (Ex: "C:/Doom3/" becomes "C:/Doom3")
        if ( k_savePath.Length() > 0 && k_savePath[k_savePath.Length() - 1] == '/' )
            k_savePath.StripTrailing( '/' );
	}

	return k_savePath.c_str();
}

/*
==============
Sys_EXEPath
==============
*/
const char *Sys_EXEPath( void )
{
#if 0
	static char exe[MAX_OSPATH];
	GetModuleFileName( NULL, exe, sizeof( exe ) - 1 );
	return exe;
#else
	return Sys_Cwd();
#endif 
}

/*
==============
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idStrList &list )
{
	int count = 0;
	char pattern[MAX_OSPATH];

	if (!extension)
		extension = "";

	list.Clear();

	// idTech commonly uses extensions like ".txt" or "txt"
	// SDL_GlobDirectory uses shell standards (e.g., *.txt)
	if (extension && extension[0] != '\0') 
	{
		if (extension[0] == '.') 
			SDL_snprintf( pattern, sizeof( pattern ), "*%s", extension );
		else 
			SDL_snprintf( pattern, sizeof( pattern ), "*.%s", extension );
    } 
	else 
	{
		SDL_strlcpy( pattern, "*", sizeof( pattern ) );
    }
	
	// SDL_GlobDirectory returns an array of strings (char**)
	// The SDL_GLOB_CASEINSENSITIVE parameter is useful for Linux
	char **files = SDL_GlobDirectory( directory, pattern, SDL_GLOB_CASEINSENSITIVE, &count );
	if ( files )
	{
		for ( int i = 0; i < count; i++ )
		{
			list.Append( files[i] );
		}

		// In SDL3, we use SDL_free to release the returned array.
        SDL_free(files);
	}

	return list.Num();
}