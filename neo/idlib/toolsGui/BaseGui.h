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

#ifndef _BASE_GUI_H_
#define _BASE_GUI_H_

#include <SDL_rect.h>

static const Uint32 k_ALIGN_NONE	= 0;
static const Uint32 k_ALIGN_LEFT	= (1 << 1);
static const Uint32 k_ALIGN_RIGHT	= (1 << 2);
static const Uint32 k_ALIGN_TOP		= (1 << 3);
static const Uint32 k_ALIGN_BOTTOM	= (1 << 4);
static const Uint32	k_ALIGN_CENTER_VERTICAL = (1 << 5);
static const Uint32 k_ALIGN_CENTER_HORIZONTAL = (1 << 6);
static const Uint32	k_ALIGN_FILL = (1 << 7);
static const Uint32 k_ALIGN_CENTERED = k_ALIGN_CENTER_VERTICAL | k_ALIGN_CENTER_HORIZONTAL;

class btRenderGui;
class btBaseGui : public btObjectCounter
{
public:
	typedef	btRefCntAutoPtr<btBaseGui>		btBaseGui_t;
	typedef	btRefCntAutoPtr<btRenderGui>	btRenderGui_t;

	btBaseGui( const char* name, const btBaseGui_t parent );
	btBaseGui( const char* name, const btBaseGui_t parent, const btRenderGui_t renderer );
	~btBaseGui( void );

	virtual	void		DoUpdate( void );
	virtual void		DoRender( void );

	virtual void		Dock( const Uint32 docking );
	virtual void		SetHidden( const bool hidden );
	virtual void		SetPos( const int x, const int y );
	virtual void		SetSize( const int w, const int h );
	virtual void		SetBounds( const SDL_Rect boundsrect );

	virtual Uint32		GetDock( void ) const;
	virtual bool		IsHidden( void ) const;
	virtual void		GetPos( int &x, int &y );
	virtual void		GetSize( int &w, int &h );
	virtual SDL_Rect	GetBounds( void ) const;

	virtual const btRenderGui_t GetRenderer( void ) const;

	virtual void	RemoveAllChildren( void );

protected:

	virtual bool ShouldClip( void ) { return false; }

	virtual void	Update( void ) {};
	virtual void	Render( void ) {};

	virtual void AddChild( btBaseGui_t child );
	virtual void RemoveChild( btBaseGui_t parent );

private:
	bool					m_hiden;
	Uint32					m_dock;
	SDL_Rect				m_bounds;
	btBaseGui_t				m_parent;
	btRenderGui_t			m_renderer;
	idList<btBaseGui_t>		m_childs;
	const char*				m_name;
};

#endif // !_BASE_GUI_H_
