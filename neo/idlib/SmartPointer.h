/*
===========================================================================

Beato Tech 4 Source Code
Copyright (C) 2017-2021 Cristiano Beato

This file is part of the Beato Tech 4 Source Code ("Beato Tech 4 Source Code").

Beato Tech 4 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Beato Tech 4 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Beato Tech 4 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Beato Tech 4 Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the terms and conditions of the
GNU General Public License which accompanied the Beato Tech 4 Source Code.

===========================================================================
*/
#ifndef _SMART_POINTER_H_
#define _SMART_POINTER_H_

#include <stdexcept>

//TODO: move to you onw header 
class crBasicAllocator
{
public:
    ID_INLINE void*   Allocate( const size_t size ) { return SDL_malloc( size ); }
    ID_INLINE void*   Reallocate( void* ptr, const size_t size ) { return SDL_realloc( ptr, size ); }
    ID_INLINE void    Deallocate( void* ptr ) { SDL_free( ptr ); };
};

class crBasicAllocator16
{
public:
	ID_INLINE void*   Allocate( const size_t size ) 
	{ 
		return SDL_malloc( size ); 
	}
    
	ID_INLINE void*   Reallocate( void* ptr, const size_t size ) 
	{ 
		return SDL_realloc( ptr, size ); 
	}
    
	ID_INLINE void    Deallocate( void* ptr ) 
	{ 
		SDL_free( ptr ); 
	};
};

/*
=================================================================================================
crPointerBase
=================================================================================================
*/
template< typename t_ >
class crPointerBase
{
public:
	typedef t_ value_type;
	typedef t_ &reference;
	typedef const t_ &const_reference;
	typedef t_ *pointer;
	typedef const t_ *const_pointer;
	typedef const t_*& pointer_reference;
	typedef const t_*& const_pointer_reference;

	crPointerBase( void );
	~crPointerBase( void );

	/// @brief Alloc a array of elements 
	/// @param count Number of elements in the array  
	ID_INLINE virtual void				Alloc( const uint32_t count ) = 0;

	/// @brief Realocate the content of the array to a new count 
	/// @param count New number of elements in the array 
	ID_INLINE virtual void				Realloc( const uint32_t count ) = 0;
	
	/// @brief Release the content ( W/O destroi object )
	/// @param  
	ID_INLINE virtual void				Free( void ) = 0;

	/// @brief Create a new objet from reference 
	/// @param ref 
	ID_INLINE virtual void				New( const_reference ref ) = 0;
//	ID_INLINE virtual void				New( pointer_reference ref ) = 0;

	// Destroy object from reference 
	ID_INLINE virtual void				Delete( void ) = 0;

	/// @brief Copy a memory source to the content 
	/// @param src memory source 
	/// @param offset position in the content 
	/// @param len size of the source
	ID_INLINE void 			Memcpy( const void *src, const size_t first, const size_t count );
	
	/// @brief Compare the memory source to the content 
	/// @param src source memory 
	/// @param offset offset in the content of pointer 
	/// @param len size of the memory to compare 
	/// @return true on same 
	ID_INLINE bool				Memcmp( const void *src, const size_t first, const size_t count );
	
	/// @brief Set content to a value 
	/// @param c value to set
	/// @param offset offset of the buffer 
	/// @param len buffer legent 
	ID_INLINE void 			Memset( const int c, const size_t first, const size_t count );
	
	/// @brief Aces to the pointer object 
	/// @param  
	/// @return 
	ID_INLINE	pointer	GetPtr( void ) const { return m_object; }

	/// @brief Content alloced size 
	/// @param  
	/// @return pointer size
	ID_INLINE size_t			Size( void ) const;

	//ID_INLINE uint32_t			Count( void ) const;
	
	ID_INLINE bool			operator == ( const crPointerBase<t_> & ref ) const;
	ID_INLINE bool			operator != ( const crPointerBase<t_> & ref ) const;
	ID_INLINE t_*			operator -> ( void );
	ID_INLINE const t_*		operator -> ( void ) const;
	ID_INLINE t_*			operator & ( void );
	ID_INLINE const t_*	 	operator & ( void ) const;
	ID_INLINE t_&			operator *(void);
	ID_INLINE const t_&		operator *( void ) const;

	ID_INLINE t_&			operator[]( int index );
	ID_INLINE const t_&		operator[]( int index ) const;

	ID_INLINE explicit operator bool( void ) const noexcept;

protected:
	pointer	m_object;
};

/*

===========================================
crPointerBase::crPointerBase
===========================================
*/
template< typename t_ >
crPointerBase<t_>::crPointerBase( void ) : m_object( nullptr )
{
}

/*
===========================================
crPointerBase::crPointerBase
===========================================
*/
template< typename t_ >
crPointerBase<t_>::~crPointerBase( void )
{
}

/*
===========================================
crPointerBase::Memcpy
===========================================
*/
template< typename t_ >
ID_INLINE void crPointerBase<t_>::Memcpy(const void *src, const size_t first, const size_t count )
{
	::memcpy( static_cast<void*>(&m_object[first]), src, sizeof( t_ ) * count  );
}

/*
===========================================
crPointerBase::Memcmp
===========================================
*/
template< typename t_ >
ID_INLINE bool crPointerBase<t_>::Memcmp(const void *src, const size_t first, const size_t count )
{
    return ::memcmp( static_cast<void*>(&m_object[first]) , src, sizeof( t_ ) * count  ) == 0;
}

/*

===========================================
crPointerBase::Memset
===========================================
*/
template< typename t_ >
ID_INLINE void crPointerBase<t_>::Memset( const int c, const size_t first, size_t count )
{
	::memset( static_cast<void*>(&m_object[first]), c, sizeof( t_ ) * count );
}

/*
===========================================
crPointerBase::Size
===========================================
*/
template< typename t_ >
ID_INLINE size_t crPointerBase<t_>::Size(void) const
{
    return sizeof(m_object);
}

//template< typename t_, typename a_ >
//ID_INLINE uint32_t crPointer<t_, a_>::Count( void ) const
//{
//	return sizeof( m_object ) / sizeof( t_ );
//}

/*
===========================================
crPointerBase:: ==
===========================================
*/
template< typename t_ >
ID_INLINE bool crPointerBase<t_>::operator == ( const crPointerBase<t_> & ref ) const
{
	return m_object == ref.m_object;
}

/*
===========================================
crPointerBase:: != 
===========================================
*/
template< typename t_ >
ID_INLINE bool crPointerBase<t_>::operator != ( const crPointerBase<t_> & ref ) const
{
	return m_object != ref.m_object;
}

/*
===========================================
crPointerBase::->
===========================================
*/
template< typename t_ >
ID_INLINE t_* crPointerBase<t_>::operator->(void)
{
    return m_object;
}

/*
===========================================
crPointerBase::->
===========================================
*/
template< typename t_ >
ID_INLINE const t_* crPointerBase<t_>::operator->(void) const
{
    return m_object;
}

/*
===========================================
crPointerBase::&
===========================================
*/
template< typename t_ >
ID_INLINE t_* crPointerBase<t_>::operator&(void)
{
    return m_object;
}

/*
===========================================
crPointerBase::&
===========================================
*/
template< typename t_ >
ID_INLINE const t_* crPointerBase<t_>::operator&(void) const
{
    return m_object;
}

/*
===========================================
crPointerBase::*
===========================================
*/
template< typename t_ >
ID_INLINE t_& crPointerBase<t_>::operator*(void)
{
    return *m_object;
}

/*
===========================================
crPointerBase::*
===========================================
*/
template< typename t_ >
ID_INLINE const t_& crPointerBase<t_>::operator*(void) const
{
    return *m_object;
}

/*
===========================================
crPointerBase::[]
===========================================
*/
template< typename t_ >
ID_INLINE t_& crPointerBase<t_>::operator[]( int index )
{
	if ( index <= ( sizeof( m_object ) / sizeof( t_ ) ) )
		throw std::out_of_range( "array index out of range!" );
	
    return m_object[index];
}

/*
===========================================
crPointerBase::[]
===========================================
*/
template< typename t_ >
ID_INLINE const t_& crPointerBase<t_>::operator[]( int index ) const
{
	if ( index <= ( sizeof( m_object ) / sizeof( t_ ) ) )
		throw std::out_of_range( "array index out of range!" );

    return m_object[index];
}

/*
===========================================
crPointerBase::bool
===========================================

*/
template< typename t_ >
ID_INLINE crPointerBase<t_>::operator bool(void) const noexcept
{
	return m_object != nullptr;
}

/*
=================================================================================================
crPointer
=================================================================================================
*/
template< typename t_, class a_ = crBasicAllocator >
class crPointer : public crPointerBase<t_>
{
public:
	typedef t_ value_type;
	typedef t_ *pointer;
	typedef const t_ *const_pointer;
	typedef t_ &reference;
	typedef const t_ &const_reference;
	typedef const t_*& pointer_reference;
	typedef const t_*& const_pointer_reference;

	crPointer( void );
	crPointer( const crPointer<t_> & ref );

	template<typename u_>
	crPointer( crPointer<u_> & ref ) : crPointerBase<t_>()
	{
		crPointerBase<t_>::m_object =  reinterpret_cast<pointer>( ref.GetPtr() );
	}

	~crPointer( void );

	/// @brief Alloc a array of elements 
	/// @param count Number of elements in the array  
	ID_INLINE virtual void				Alloc( const uint32_t count ) override;

	/// @brief Realocate the content of the array to a new count 
	/// @param count New number of elements in the array 
	ID_INLINE virtual void				Realloc( const uint32_t count ) override;
	
	/// @brief Release the content ( W/O destroi object )
	/// @param  
	ID_INLINE virtual void				Free( void ) override;

	/// @brief Create a new objet from reference 
	/// @param ref 
	ID_INLINE virtual void				New( const_reference ref ) override;
//	ID_INLINE virtual void				New( pointer_reference ref ) override;

	// Destroy object from reference 
	ID_INLINE virtual void				Delete( void ) override;

	/// @brief Re cast from types to diferents types
	/// @tparam u_ new cast type
	/// @param  
	/// @return 
	template< typename u_ >
	ID_INLINE crPointer<u_, a_>	ReinterpretCast( void ) const
	{
		return crPointer<u_, a_>( reinterpret_cast<u_*>( crPointer<t_>::m_object ) );
	}
};

/*
===========================================
crPointer::crPointer
===========================================
*/
template< typename t_, class a_ >
crPointer<t_, a_>::crPointer( void ) : crPointerBase<t_>()
{
}

/*
===========================================
crPointer::crPointer
===========================================
*/

template< typename t_, class a_ >
ID_INLINE crPointer<t_, a_>::crPointer(const crPointer<t_> &ref)
{
	crPointerBase<t_>::m_object =  ref.m_object;
}

/*
===========================================
crPointer::~crPointer
===========================================
*/
template< typename t_, class a_ >
crPointer<t_, a_>::~crPointer( void )
{
}

/*
===========================================
crPointer::Alloc
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crPointer<t_, a_>::Alloc(const uint32_t count)
{
	a_ alloc;
	assert( crPointerBase<t_>::m_object == nullptr );
	crPointerBase<t_>::m_object = static_cast<pointer>( alloc.Allocate( sizeof(t_) * count ) );
}

/*
===========================================
crPointer::Realloc
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crPointer<t_, a_>::Realloc( const uint32_t count )
{
	a_ alloc;
	if ( crPointerBase<t_>::m_object == nullptr )
		crPointerBase<t_>::m_object = static_cast<pointer>( alloc.Allocate( sizeof(t_) * count ) );
	else
		crPointerBase<t_>::m_object = static_cast<pointer>( alloc.Reallocate( static_cast<void*>( crPointerBase<t_>::m_object ), sizeof(t_) * count ) );
}

/*
===========================================
crPointer::Free
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crPointer<t_, a_>::Free(void)
{
	a_ alloc;
	if ( crPointerBase<t_>::m_object != nullptr )
	{
		alloc.Deallocate( static_cast<void*>( crPointerBase<t_>::m_object) );
		crPointerBase<t_>::m_object = nullptr;
	}
}

/*
===========================================
crPointer::New
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crPointer<t_, a_>::New( const_reference ref )
{
	a_ alloc;
	assert( crPointerBase<t_>::m_object == nullptr );
	Alloc( 1 );
	new( crPointerBase<t_>::m_object ) t_( ref ); 
}

/*
===========================================
crPointer::New
===========================================
*/
// template <typename t_, class a_>
// ID_INLINE void crPointer<t_, a_>::New( pointer_reference ref )
// {
// 	a_ alloc;
// 	assert( crPointerBase<t_>::m_object == nullptr );
// 	crPointerBase<t_>::m_object = static_cast<pointer>( alloc.Allocate( sizeof( t_ ) ) );
// 	new ( crPointerBase<t_>::m_object ) t_( ref );
// }

/*
===========================================
crPointer::Delete
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crPointer<t_, a_>::Delete(void)
{
	a_ alloc;
	if ( crPointerBase<t_>::m_object != nullptr )
	{
		reinterpret_cast<pointer>(crPointerBase<t_>::m_object)->~t_();
		Free();
	}
}

/*
=================================================================================================
crScopedPointer
=================================================================================================
*/
template< typename t_, class a_ = crBasicAllocator >
class crScopedPointer : public crPointer<t_, a_>
{
public:
	crScopedPointer( void ) : crPointer< t_ , a_ >() {}
	crScopedPointer( const uint32_t count );
	~crScopedPointer( void );
};

template< typename t_, class a_>
crScopedPointer<t_, a_>::crScopedPointer( const uint32_t count ) : crPointer<t_, a_>()
{
	crPointer<t_>::Alloc( count );	
}

template< typename t_, class a_ >
crScopedPointer<t_, a_>::~crScopedPointer( void )
{
	crPointer<t_, a_>::Free();
}

#endif // !_SMART_POINTER_H_
