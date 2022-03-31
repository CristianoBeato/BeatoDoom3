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

#include <SDL_filesystem.h>

// Requires c++ 17
#include <filesystem>
namespace fs = std::filesystem;


#include "sys_platform.h"
#include "sys_main.h"

static idStr savePath = idStr();
static idStr basePath = idStr();

/*
==============
Sys_Cwd
==============
*/
const char *Sys_Cwd( void )
{
	static char cwd[MAX_OSPATH];
#if 1
	fs::path cwdPath = fs::current_path();
	strcpy( cwd, cwdPath.u8string().c_str() );
#else
	_getcwd( cwd, sizeof( cwd ) - 1 );
	cwd[MAX_OSPATH - 1] = 0;
#endif
	return cwd;
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
=================
Sys_FileTimeStamp
=================
*/
ID_TIME_T Sys_FileTimeStamp( FILE *fp )
{
#if _WIN32 || _WIN64
	struct _stat st;
	_fstat( _fileno( fp ), &st );
	return (long)st.st_mtime;
#else
	struct stat st;
	fstat( fileno( fp ), &st );
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
	//Beato: uses getenv() on linux, because sdl set in "/home/user name/.local/share/SAVE_PATH/"
#if defined(__linux__)
	sprintf( savePath, "%s/.%s", getenv( "HOME" ), SAVE_PATH );
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
	static char exe[MAX_OSPATH];
	GetModuleFileName( NULL, exe, sizeof( exe ) - 1 );
	return exe;
}

/*
==============
Sys_ListFiles
==============
*/
int Sys_ListFiles( const char *directory, const char *extension, idStrList &list )
{
	idStr		search;

	if (!extension)
		extension = "";

#if 1
	fs::path path = directory;
	for (const fs::directory_entry & entry : fs::directory_iterator( path ))
	{
		// passing a slash as extension will find directories
		if (extension[0] == '/' && extension[1] == 0)
		{
			if (entry.is_directory())
			{
				fs::path subDirPath = entry.path();
				idStr subDir = idStr( subDirPath.string().c_str() );
				list.Append( subDir );
			}
		}
		else
		{
			if ( entry.is_regular_file() )
			{
				fs::path filePath = entry.path();
				fs::path fileExt = filePath.extension();
				if (!fileExt.empty() && fileExt.string().compare( extension ))
				{
					idStr file = idStr( filePath.string().c_str() );
					list.Append( file );
				}
				
			}
		}
	}
#else
	struct _finddata_t findinfo;
	int			findhandle;
	int			flag;

	

	// passing a slash as extension will find directories
	if (extension[0] == '/' && extension[1] == 0)
	{
		extension = "";
		flag = 0;
	}
	else
	{
		flag = _A_SUBDIR;
	}

	sprintf( search, "%s\\*%s", directory, extension );

	// search
	list.Clear();

	findhandle = _findfirst( search, &findinfo );
	if (findhandle == -1)
	{
		return -1;
	}

	do
	{
		if (flag ^ (findinfo.attrib & _A_SUBDIR))
		{
			list.Append( findinfo.name );
		}
	}
	while (_findnext( findhandle, &findinfo ) != -1);

	_findclose( findhandle );
#endif
	return list.Num();

}