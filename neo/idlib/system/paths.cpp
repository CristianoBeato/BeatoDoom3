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

#include "sys_paths.h"
#include <SDL3/SDL_filesystem.h>

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
	if( sys_currentWorkDir.Empty() )
	{
		sys_currentWorkDir.SetString(Win_GetCurrentUser());
	}

	return sys_currentWorkDir.GetString();
}

/*
==============
Sys_Mkdir
==============
*/
void Sys_Mkdir( const char *path )
{
#if 1
	fs::path pathToCreate = path;
	if (!fs::create_directory( pathToCreate ))
		common->Error( " can't create dir %s\n", path );
#else
	_mkdir( path );
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
	static idStr basePath = idStr();
	if (basePath.IsEmpty())
	{
		char* save_path = SDL_GetBasePath();
		if (save_path)
		{
			basePath = SDL_strdup( save_path );
			SDL_free( save_path );
		}
		else
			basePath = Sys_Cwd();

		basePath.BackSlashesToSlashes();
	}

	return basePath.c_str();
}

/*
==============
Sys_DefaultSavePath
==============
*/
const char *Sys_DefaultSavePath( void )
{
	static idStr savePath = idStr();
	//Beato: uses getenv() on linux, because sdl set in "/home/user name/.local/share/SAVE_PATH/"
#if defined(__linux__)
	sprintf( savePath, "%s/.%s", getenv( "HOME" ), "btech4" );
#else
	if (savePath.IsEmpty())
	{
		char* save_path = SDL_GetPrefPath( "BeatoSoftware", "BeatoD3" );
		if (save_path)
		{
			savePath = SDL_strdup( save_path );
			SDL_free( save_path );
		}
		else // if can't get a valid save path, save in game folder 
			savePath = Sys_DefaultBasePath();

		savePath.BackSlashesToSlashes();
	}
#endif
	return savePath.c_str();
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
}

/*
==============
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idList<class idStr> &list )
{
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