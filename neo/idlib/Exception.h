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

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

class idException
{
public:
	char error[MAX_STRING_CHARS];

	idException( void )
	{
		memset( error, 0x00, MAX_STRING_CHARS );
	}

	explicit idException( const char *fmt, ... )
	{
		va_list		argptr;
		va_start( argptr, fmt );
		vsnprintf( error, MAX_STRING_CHARS, fmt, argptr );
		va_end( argptr );
	}

	~idException( void )
	{
		memset( error, 0x00, MAX_STRING_CHARS );
	}

	inline const char* What( void ) const
	{
		return error;
	}

};

#endif // !__EXCEPTION_H__
