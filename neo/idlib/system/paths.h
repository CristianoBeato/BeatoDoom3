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
#ifndef __SYS_PATH_H__
#define __SYS_PATH_H__

// current work dir: get current working directory 
extern const char*      Sys_Cwd( void );

// create a new directory
extern void             Sys_Mkdir( const char *path );

// defalt assets path 
extern const char*      Sys_DefaultBasePath( void );

// defalt user sava dir
extern const char*      Sys_DefaultSavePath( void );

//
extern const char*      Sys_EXEPath( void );

//
extern bool             Sys_PathExist( const char* path );

// 
extern int              Sys_ListFiles( const char *directory, const char *extension, idList<class idStr> &list );

//
extern uint64_t         Sys_GetDriveFreeSpace( const char *directory );

#endif //!__SYS_PATH_H__