/*
===========================================================================

Beato idTech 4 Source Code
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

#ifndef _RENDER_GUI_H_
#define _RENDER_GUI_H_

// Canvas
//0 ______________3
// |			 |
// |			 |
//1|_____________|2

// Button 
//0____________
//|\________/|
//| |______| |
//|/________\|

struct guiPosf32_s
{
	float x = 0.0f;
	float y = 0.0f;
};

struct guiPosi16_s
{
	Uint16 x = 0;
	Uint16 y = 0;
};

struct guiColorf32
{
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 0.0f;
};

struct guiColori8
{
	Uint8 r = 0;
	Uint8 g = 0;
	Uint8 b = 0;
	Uint8 a = 0;
};

typedef struct
{
	guiPosi16_s	pos;
	guiColori8	color;
}guiVertex_t;


class btRenderGui
{
public:
	btRenderGui( void );
	~btRenderGui( void );

	void	Init( void );
	void	ShutDown( void );

	const uint32_t	Cache( const uint32_t size, const void* data );

	void	Draw(  uint32_t mode, const uint32_t offset, const uint32_t Count );

private:
	uint32_t	m_vertexBuffer;
	uint32_t	m_vertexArray;
};

#endif // !_RENDER_GUI_H_
