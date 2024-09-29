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
#ifndef __POINTERS_H__
#define __POINTERS_H__

//
// Inteligent pointer holder class 
// Base pointer class 
//
template<typename _t>
class crPointer
{
public:
    typedef _t&         ref;
    typedef const _t&   constRef;
    typedef _t*         ptr;
    typedef const _t*   constPtr;

    crPointer( void ) : m_pointer( nullptr )
    {
    }

    crPointer( const uint32_t count ) : m_pointer( nullptr )
    {
        Alloc( count );
    }

    // REMEMBER losing reference don't clear this pointer
    ~crPointer( void )
    {
    }

    ID_INLINE void    Alloc( const uint32_t count )
    {
        m_pointer = static_cast<ptr>( Mem_Alloc( sizeof(_t) * count ) );
        if ( m_pointer != nullptr )
            throw idException( "fail to Mem_Alloc" );
    }

    ID_INLINE void  Free( void )
    {
        if ( m_pointer != nullptr )
        {
            Mem_Free( m_pointer );
            m_pointer = nullptr;
        }
    }

    ref         operator[] ( int i ) { return m_pointer[i]; }
    constRef    operator[] ( int i ) const { return m_pointer[i]; }
    ptr         operator& ( void ) { return m_pointer; }
    constPtr    operator& ( void ) const { return m_pointer; }
    ptr         operator->( void ) { return m_pointer; }
    constPtr    operator->( void ) const { return m_pointer; }
    operator bool( void ) const { return ( m_pointer != nullptr ); }
    const size_t      Size( void ) const { return sizeof( m_pointer ); }
    const uint32_t    Count( void ) const { return ( sizeof( m_pointer ) / sizeof(_t) ); }

private:
    ptr     m_pointer;
};

///
/// Scoped pointer
/// Allocate pointer at creation, and free at scope end 
///
template<typename _t>
class crScopedPtr : public crPointer<_t>
{
public:
    crScopedPtr( void ) : crPointer<_t>()
    {   
    }

    crScopedPtr( const uint32_t count ) : crPointer<_t>( count )
    {
    }

    ~crScopedPtr( void )
    {
        crPointer<_t>::Free();
    }

private:
    crScopedPtr( crScopedPtr<_t> &ref ){};
    void operator = ( crScopedPtr<_t> &ref ){};
};

#endif //!__POINTERS_H__
