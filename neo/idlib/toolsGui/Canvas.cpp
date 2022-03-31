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

#include "idlib/precompiled.h"
#include "GuiCommon.h"

btCanvas::btCanvas( const char *name ) : btBaseGui( name, nullptr )
{
	m_rectColor = { 0x45, 0x45, 0x45, 0xFF };
	m_borderColor = { 0x2F, 0x2F, 0x2F, 0xFF };
	m_border = true;
}

btCanvas::~btCanvas( void )
{
}

void btCanvas::SetBorder( const bool hasBorder )
{
	m_border = hasBorder;
}

void btCanvas::SetColor( const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a )
{
	m_rectColor.r = r;
	m_rectColor.g = g;
	m_rectColor.b = b;
	m_rectColor.a = a;
}

void btCanvas::SetBorderColor( const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a )
{
	m_borderColor.r = r;
	m_borderColor.g = g;
	m_borderColor.b = b;
	m_borderColor.a = a;
}

void btCanvas::ReMesh( void )
{
	auto renderer = GetRenderer();

	Uint32 size = sizeof( guiVertex_t ) * 4;
	guiVertex_t rect[4];
	Uint16 indexes[4];

	m_rectOffset = renderer->Cache( size, (void*)rect );

	SDL_Rect bounds = GetBounds();

	// 0____2
	// |  / |
	// | /  |
	// 1/___3

	rect[0].pos = { (Uint16)bounds.x,  (Uint16)bounds.y };
	rect[0].color = m_rectColor;

	rect[1].pos = { (Uint16)bounds.x,  (Uint16)(bounds.y + bounds.h) };
	rect[1].color = m_rectColor;

	rect[2].pos = { (Uint16)(bounds.x + bounds.w),  (Uint16)bounds.y };
	rect[2].color = m_rectColor;

	rect[3].pos = { (Uint16)(bounds.x + bounds.w),  (Uint16)(bounds.y + bounds.h) };
	rect[3].color = m_rectColor;

	indexes[0] = 0;
	indexes[1] = 1;
	indexes[2] = 3;
	indexes[3] = 2;

	if (m_border)
	{
		indexes[0] = 0;
		indexes[1] = 1;
		indexes[2] = 3;
		indexes[3] = 2;

		m_rectOffset = renderer->Cache( size, (void*)rect );
	}
}

void btCanvas::Update( void )
{
	ReMesh();
}

void btCanvas::Render( void )
{
	auto renderer = GetRenderer();

	renderer->Draw( GL_TRIANGLE_STRIP, m_rectOffset, 6 );

	if (m_border)
		renderer->Draw( GL_LINE_LOOP, m_borderOffsets, 4 );
}
