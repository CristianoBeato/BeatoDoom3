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

#ifndef __SCREEN_RECT_H__
#define __SCREEN_RECT_H__

// idScreenRect gets carried around with each drawSurf, so it makes sense
// to keep it compact, instead of just using the idBounds class
class idScreenRect 
{
public:
	int16_t		x1, y1, x2, y2;							// inclusive pixel bounds inside viewport
    float       zmin, zmax;								// for depth bounds test

	void		Clear();								// clear to backwards values
	void		AddPoint( float x, float y );			// adds a point
	void		Expand();								// expand by one pixel each way to fix roundoffs
	void		Intersect( const idScreenRect &rect );
	void		Union( const idScreenRect &rect );
	bool		Equals( const idScreenRect &rect ) const;
	bool		IsEmpty() const;
};

#endif //!__SCREEN_RECT_H__