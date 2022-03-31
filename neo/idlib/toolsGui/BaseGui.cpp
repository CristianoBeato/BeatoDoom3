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

btBaseGui::btBaseGui( const char * name, const btBaseGui_t parent ) :
	m_name(name),
	m_parent( parent )
{
	if (m_parent)
		AddChild( this );
}

btBaseGui::btBaseGui( const char * name, const btBaseGui_t parent, const btRenderGui_t renderer ) :
	m_name( name ),
	m_parent( parent ),
	m_renderer( renderer )
{
	if (m_parent)
		AddChild( this );
}


btBaseGui::~btBaseGui( void )
{
	RemoveAllChildren();
	if (m_parent)
		m_parent->RemoveChild( this );
}

void btBaseGui::DoUpdate( void )
{
	if (m_hiden)
		return;

	Update();

	for ( int i = 0; i < m_childs.Num(); i++)
	{
		m_childs[i]->DoUpdate();
	}
}

void btBaseGui::DoRender( void )
{
	if (m_hiden)
		return;

	Render();

	for (int i = 0; i < m_childs.Num(); i++)
	{
		m_childs[i]->DoRender();
	}
}

void btBaseGui::Dock( const Uint32 docking )
{
	m_dock = docking;
}

void btBaseGui::SetHidden( const bool hidden )
{
	if (m_hiden != hidden)
		m_hiden = hidden;
}

void btBaseGui::SetPos( const int x, const int y )
{
	if (m_parent)
	{
		SDL_Rect parent = m_parent->GetBounds();
		m_bounds.x = Clamp( x, parent.x, parent.x + parent.w );
		m_bounds.y = Clamp( y, parent.y, parent.y + parent.h );
	}
	else
	{
		m_bounds.x = x;
		m_bounds.y = y;
	}
}

void btBaseGui::SetSize( const int w, const int h )
{
	if (m_parent)
	{
		SDL_Rect parent = m_parent->GetBounds();
		m_bounds.w = Min( w, m_bounds.x + parent.w ); 
		m_bounds.h = Min( h, m_bounds.y + parent.h );
	}
	else
	{
		m_bounds.w = w;
		m_bounds.h = h;
	}
}

void btBaseGui::SetBounds( const SDL_Rect boundsrect )
{
	m_bounds = boundsrect;
}

Uint32 btBaseGui::GetDock( void ) const
{
	return m_dock;
}

bool btBaseGui::IsHidden( void ) const
{
	return m_hiden;
}

void btBaseGui::GetPos( int & x, int & y )
{
	x = m_bounds.x;
	y = m_bounds.y;
}

void btBaseGui::GetSize( int & w, int & h )
{
	w = m_bounds.w;
	h = m_bounds.h;
}

SDL_Rect btBaseGui::GetBounds( void ) const
{
	return m_bounds;
}

const btBaseGui::btRenderGui_t btBaseGui::GetRenderer( void ) const
{
	assert( m_renderer != nullptr );
	return m_renderer;
}

void btBaseGui::RemoveAllChildren( void )
{
	while (m_childs.Num() !=  0 )
	{
		m_childs.RemoveIndex( 0 );
	}
}

void btBaseGui::AddChild( const btBaseGui_t child )
{
	m_childs.Append( child );
}

void btBaseGui::RemoveChild( btBaseGui_t parent )
{
	for ( int i = 0; i < m_childs.Num(); i++)
	{
		if(m_childs[i] == parent)
		{
			m_childs.RemoveIndex( i );
			break;
		}
	}
}