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

#ifndef __RESOURCE_H__
#define __RESOURCE_H__

// resoucer load state
enum resourceState_s 
{
    RESOURCE_UNINITIALIZED = -1,    // unitialized resource
    RESOURCE_FAILED,                // failed to load resouce 
    RESOURCE_CREATED,               // just resource pointer
    RESOURCE_LOADING,               // resource is loading
    RESOURCE_LOADED                 // resource load is finished 
};

/// @brief abstract resource class,
/// resources are objects that need to be loaded from the engine's assets,
/// resources are planned to be loaded asynchronously, 
/// allowing fast loading without harming the real-time execution of the application.
class crResource
{
public:
    crResource( void );
    ~crResource( void );
    virtual void                Create(const idStr& in_name ) = 0;
    virtual void                Load( void ) = 0;
    virtual const idStr&        Name( void ) const { return m_name; };
    virtual resourceState_s     GetState( void ) const { return m_state; };

protected:
    resourceState_s m_state;
    idStr           m_name;
};

#endif //__RESOURCE_H__