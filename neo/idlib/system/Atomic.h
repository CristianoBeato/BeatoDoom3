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

#ifndef _ATOMIC_H_
#define _ATOMIC_H_

/*
================================================
idSysInterlockedInteger is a C++ wrapper for the low level system interlocked integer
routines to atomically increment or decrement an integer.
================================================
*/
class idSysInterlockedInteger 
{
public:
	idSysInterlockedInteger( void );
	ID_INLINE int	Increment( void );		// atomically increments the integer and returns the old value
	ID_INLINE int	Decrement( void );		// atomically decrements the integer and returns the old value
	ID_INLINE int	Add( int v ); 			// atomically adds a value to the integer and returns the new value
	ID_INLINE int	Sub( int v ); 			// atomically subtracts a value from the integer and returns the new value
	ID_INLINE int	GetValue( void ) const;	// returns the current value of the integer	
	ID_INLINE void	SetValue( int v ); 		// sets a new value, Note: this operation is not atomic

	ID_INLINE bool	operator==( const int v ) const;
private:
	int	value;
};

ID_INLINE int idSysInterlockedInteger::Increment( void ) 
{ 
	return __sync_fetch_and_add( &value, 1); 
}

ID_INLINE int idSysInterlockedInteger::Decrement( void ) 
{
	return __sync_fetch_and_sub( &value, 1 );
}

ID_INLINE int	idSysInterlockedInteger::Add( int v ) 
{ 
	return __sync_add_and_fetch( &value, v ); 
}

ID_INLINE int idSysInterlockedInteger::Sub( int v ) 
{ 
	return __sync_sub_and_fetch( &value, v );
}

ID_INLINE int idSysInterlockedInteger::GetValue( void ) const 
{ 
	return value; 
}

ID_INLINE void	idSysInterlockedInteger::SetValue( int v ) 
{  
	__sync_val_compare_and_swap( &value, value, v );
}

ID_INLINE bool idSysInterlockedInteger::operator==(const int v) const
{
	return value == v;
}

/*
================================================
idSysInterlockedPointer is a C++ wrapper around the low level system interlocked pointer
routine to atomically set a pointer while retrieving the previous value of the pointer.
================================================
*/
template< typename T >
class idSysInterlockedPointer 
{
public:
	idSysInterlockedPointer( void );

	// atomically sets the pointer and returns the previous pointer value
	ID_INLINE T* Set( T * newPtr );

	// atomically sets the pointer to 'newPtr' only if the previous pointer is equal to 'comparePtr'
	// ptr = ( ptr == comparePtr ) ? newPtr : ptr
	ID_INLINE T* CompareExchange( T * comparePtr, T * newPtr );

	// returns the current value of the pointer
	ID_INLINE T* Get( void ) const;

private:
	void * ptr;
};

template< typename T >
idSysInterlockedPointer<T>::idSysInterlockedPointer( void) : ptr( nullptr ) 
{
}

template< typename T >
ID_INLINE T* idSysInterlockedPointer<T>::Set( T * newPtr ) 
{
	return static_cast<T*>( __sync_val_compare_and_swap( &ptr, ptr, newPtr ) ); 
}

template< typename T >
ID_INLINE T* idSysInterlockedPointer<T>::CompareExchange( T * comparePtr, T * newPtr ) 
{
	return static_cast<T*>( __sync_val_compare_and_swap( &ptr, comparePtr, newPtr ) );
}

template< typename T >
ID_INLINE T* idSysInterlockedPointer<T>::Get( void ) const 
{ 
	return ptr;
}

#endif // !_ATOMIC_H_