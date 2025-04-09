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

// BEATO HELP:
// GCC atomic
// __sync_fetch_and_add: atomically increase and return the old value ( old = x, x += n, old )
// __sync_fetch_and_sub: atomically decrement and return the old value ( old = x, x -= n, old )
// __sync_add_and_fetch atomically increase and return the new value ( x += n, x )
// __sync_sub_and_fetch atomically decrements and return the new value ( x -= n, x )

// STD C11 atomic
// atomic_fetch_add: Atomically replaces the value pointed by obj with the result of addition of arg to the old value of obj
// atomic_fetch_sub: Atomically replaces the value pointed by obj with the result of subtraction of arg from the old value of obj

/*
=====================================================================================
crAutoPointer
=====================================================================================
*/
template< typename t_, typename a_ = crBasicAllocator >
class crAutoPointer : public crPointerBase<t_>
{
public:
	typedef t_ value_type;
	typedef t_ *pointer;
	typedef const t_ *const_pointer;
	typedef t_ &reference;
	typedef const t_ &const_reference;
	typedef const t_*& pointer_reference;
	typedef const t_*& const_pointer_reference;

	crAutoPointer( void );
	crAutoPointer( const crAutoPointer<t_, a_>& ptr );
	~crAutoPointer( void );

	// WARNING: don't use external pointers, this is only for "this" operator use only 
	crAutoPointer( pointer ptr ) : crPointerBase<t_>()
	{
		// if already have a reference
		if( crPointerBase<t_>::m_object != nullptr )
		{
			if ( DecRefCount( crPointerBase<t_>::m_object ) < 1 )
				Delete();
		}
		
		// reference the pointer 
		crPointerBase<t_>::m_object =  reinterpret_cast<pointer>( ptr );
		
		// incrase pointer reference
		if( crPointerBase<t_>::m_object != nullptr )
			IncRefCount( crPointerBase<t_>::m_object );
	}

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
	ID_INLINE virtual void				New( const_reference ref = t_() ) override;
	// ID_INLINE virtual void				New( const_pointer ref ) override;

	// Destroy object from reference 
	ID_INLINE virtual void				Delete( void ) override;

	/// @brief Re cast from types to diferents types
	/// @tparam u_ new cast type
	/// @param  
	/// @return 
	template< typename u_ >
	ID_INLINE crPointer<u_, a_>	ReinterpretCast( void ) const
	{
		return crAutoPointer<u_, a_>( reinterpret_cast<u_*>( crPointerBase<t_>::m_object ) );
	}


private:
	// DANGER: 
	crAutoPointer( const t_ &ref )
	{
		if( crPointerBase<t_>::m_object != nullptr )
		{
			if ( DecRefCount( crPointerBase<t_>::m_object ) < 1 )
				Delete();
		}

		crAutoPointer::m_object = ref;

		// incrase point reference 
		if( crPointerBase<t_>::m_object != nullptr )
			IncRefCount( crPointerBase<t_>::m_object );
	}

	uint32_t IncRefCount( const_pointer ptr );
	uint32_t DecRefCount( const_pointer ptr );
};

/*
===========================================
crAutoPointer::crAutoPointer
===========================================
*/
template<typename t_, typename a_>
crAutoPointer<t_, a_>::crAutoPointer( void ) : crPointerBase<t_>()
{
}

/*
===========================================
crAutoPointer::crAutoPointer
===========================================
*/
template<typename t_, typename a_>
crAutoPointer<t_, a_>::crAutoPointer( const crAutoPointer<t_, a_>& ptr )
{
	// if already have a reference
	if( crPointerBase<t_>::m_object != nullptr )
	{
		if ( DecRefCount( crPointerBase<t_>::m_object ) < 1 )
			Delete();
	}

	// copy  content 
	crPointerBase<t_>::m_object = ptr.m_object;
	
	// incrase pointer reference
	if( crPointerBase<t_>::m_object != nullptr )
		IncRefCount( crPointerBase<t_>::m_object );
}

/*
===========================================
crAutoPointer::crAutoPointer
===========================================
*/
template<typename t_, typename a_>
crAutoPointer<t_, a_>::~crAutoPointer( void )
{
	// if is the last pointer reference delete the object
	if( crPointerBase<t_>::m_object != nullptr )
	{
		if ( DecRefCount( crPointerBase<t_>::m_object ) < 1 )
			Delete();
	}
}

/*
===========================================
crAutoPointer::Alloc
===========================================
*/
template <typename t_, typename a_>
ID_INLINE void crAutoPointer<t_, a_>::Alloc( const uint32_t count )
{
	a_ alloc;
	assert( crPointerBase<t_>::m_object != nullptr );

	// Allocate the atomic counter at pointer begining 
	void* ptr = alloc.Allocate( ( sizeof(t_) * count ) + sizeof( uintptr_t ) );

	// hide atomic counter
	crPointerBase<t_>::m_object = reinterpret_cast<pointer>( reinterpret_cast<uintptr_t>( ptr ) + sizeof(uintptr_t) );
}

/*
===========================================
crAutoPointer::Realloc
===========================================
*/
template <typename t_, typename a_>
ID_INLINE void crAutoPointer<t_, a_>::Realloc(const uint32_t count)
{
	a_ alloc;
	if ( crPointerBase<t_>::m_object == nullptr )
	{
		crAutoPointer<t_, a_>::Alloc( count );
	}
	else
	{
		// get whole pointer
		void* ptr = reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( crPointerBase<t_>::m_object ) + sizeof(uint32_t) );

		// realocate memory
		ptr =  alloc.Reallocate( ptr, ( sizeof(t_) * count ) + sizeof( uintptr_t ) );

		// hide counter
		crPointerBase<t_>::m_object = reinterpret_cast<pointer>( reinterpret_cast<uintptr_t>( ptr ) + sizeof( uint32_t ) );
	}
}

/*
===========================================
crAutoPointer::Free
===========================================
*/
template <typename t_, typename a_>
ID_INLINE void crAutoPointer<t_, a_>::Free(void)
{
	a_ alloc;
	if ( crPointerBase<t_>::m_object != nullptr )
	{
		// get whole pointer
		void* ptr = reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( crPointerBase<t_>::m_object ) + sizeof(uintptr_t) );

		// release
		alloc.Deallocate( ptr );
		crPointerBase<t_>::m_object = nullptr;
	}
}


/*
===========================================
crAutoPointer::New
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crAutoPointer<t_, a_>::New( const_reference ref )
{
	assert( crPointerBase<t_>::m_object == nullptr );
	Alloc( 1 );
	new( crPointerBase<t_>::m_object ) t_( ref ); 
	IncRefCount( crPointerBase<t_>::m_object );
}

/*
===========================================
crAutoPointer::New
===========================================
*/
// template <typename t_, class a_>
// ID_INLINE void crAutoPointer<t_, a_>::New( pointer_reference ref )
// {
// 	a_ alloc;
// 	assert( crPointerBase<t_>::m_object == nullptr );
// 	crPointerBase<t_>::m_object = static_cast<pointer>( alloc.Allocate( sizeof( t_ ) ) );
// 	new ( crPointerBase<t_>::m_object ) t_( ref );
// }

/*
===========================================
crAutoPointer::Delete
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crAutoPointer<t_, a_>::Delete(void)
{
	a_ alloc;
	if ( crPointerBase<t_>::m_object != nullptr )
	{
		reinterpret_cast<pointer>(crPointerBase<t_>::m_object)->~t_();
		Free();
	}
}

/*
===========================================
crAutoPointer::IncRefCount
===========================================
*/
template <typename t_, typename a_>
ID_INLINE uint32_t crAutoPointer<t_, a_>::IncRefCount( const_pointer ptr )
{
	uint32_t* val = reinterpret_cast<uint32_t*>( reinterpret_cast<uintptr_t>( ptr ) - sizeof( int32_t ) );
    #ifdef USE_GCC_ATTOMIC
		return atomic_fetch_add( val, 1 ) + 1;
#else
		return __sync_fetch_and_add( val , 1 ) + 1;
#endif
}

/*
===========================================
crAutoPointer::DecRefCount
===========================================
*/
template <typename t_, typename a_>
ID_INLINE uint32_t crAutoPointer<t_, a_>::DecRefCount( const_pointer ptr )
{
	uint32_t* val = reinterpret_cast<uint32_t*>( reinterpret_cast<uintptr_t>( ptr ) - sizeof( int32_t ) );
#ifdef USE_GCC_ATTOMIC
		return atomic_fetch_sub( val, 1 ) - 1;
#else
		return __sync_sub_and_fetch( val, 1 ) - 1;
#endif
}

#endif // !_SMART_POINTER_H_
