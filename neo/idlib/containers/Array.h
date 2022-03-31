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

#ifndef _ARRAY_H_
#define _ARRAY_H_


template< typename t_ >
class btArray
{
public:
	typedef t_	_elmt_;
	typedef const t_ _celmt_;
	typedef t_*	_array_;
	typedef const t_* _carray_;

	btArray( void ) :
		m_arraySize( 0 ),
		m_elementCount( 0 ),
		m_elementArray( nullptr )
	{
	}

	btArray( const size_t  count ) :
		m_arraySize( 0 ),
		m_elementCount( count ),
		m_elementArray( nullptr )
	{
		m_arraySize = sizeof( _elmt_ ) * count;
		m_elementArray = Mem_AllocType<_elmt_>( count );
	}

	~btArray( void )
	{
		Clear();
	}

	// Limpa a lista e desaloca os elementos
	// Clear the array and dealloc
	ID_INLINE void	Clear( void )
	{
		m_elementCount = 0;
		m_arraySize = 0;
		SAFE_FREE( m_elementArray );
	}

	ID_INLINE void	Resize( const size_t newCount )
	{
		m_elementCount = newCount;
		m_arraySize = sizeof( _elmt_ ) * m_elementCount;
		Mem_Realloc( m_elementArray, m_arraySize );
	}

	ID_INLINE void	Reserve( const size_t count )
	{
		m_elementCount = count;
		m_arraySize = sizeof( _elmt_ ) * m_elementCount;
		m_elementArray = Mem_AllocType< _elmt_ >( m_elementCount );
	}

	// Retorna a contagem de elementos
	// Get the element count
	ID_INLINE const size_t	Count( void ) const { return m_elementCount; }

	// Retorna o tamanho alocado da lista
	// Return the allocated size
	ID_INLINE const size_t	Size( void ) const { return m_arraySize; }

	// Verdadeiro se a lista está vazia
	// True if the array is empty
	ID_INLINE const bool		IsEmpty( void ) const { return m_elementArray == nullptr }

	//
	// 
	ID_INLINE _elmt_&		operator[]( int ind ) { return m_elementArray; }
	ID_INLINE _celmt_&		operator[]( int ind ) const { return m_elementArray; }

	// Retorna o ponteiro da lista de elementos
	// Get the element array pointer
	ID_INLINE _array_		operator & ( void ) { return m_elementArray; }
	ID_INLINE _carray_		operator & ( void ) const { return m_elementArray; }

private:
	size_t	m_arraySize;
	size_t	m_elementCount;
	_array_	m_elementArray;
};

#endif // !_ARRAY_H_

