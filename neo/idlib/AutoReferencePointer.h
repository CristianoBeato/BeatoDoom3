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

#ifndef __AUTO_REFERENCE_POINTER_H__
#define __AUTO_REFERENCE_POINTER_H__

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
class crAutoPointer
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

#if 0
	// 
	template<u_>
	crAutoPointer( const crAutoPointer<u_, a_>& ptr )
	{
		// if already have a reference
		if( crAutoPointer<t_, a_>::m_object != nullptr )
		{
			if ( DecRefCount( crAutoPointer<t_, a_>::m_object ) < 1 )
				Delete( *this );
		}
		
		// reference the pointer 
		crAutoPointer<t_, a_>::m_object =  dynamic_cast<pointer>( ptr.m_object );
		
		// incrase pointer reference
		if( crAutoPointer<t_, a_>::m_object != nullptr )
			IncRefCount( crAutoPointer<t_, a_>::m_object );
	}
#endif

	/// @brief Alloc a array of elements 
	/// @param count Number of elements in the array  
	ID_INLINE static crAutoPointer<t_, a_>  Alloc( const uint32_t count );

	/// @brief Realocate the content of the array to a new count 
	/// @param count New number of elements in the array 
	ID_INLINE static crAutoPointer<t_, a_>  Realloc( crAutoPointer<t_, a_> &ref, const uint32_t count );
	
	/// @brief Release the content ( W/O destroi object )
	/// @param  
	ID_INLINE static void				    Free( crAutoPointer<t_, a_> &ref );

	/// @brief Create a new objet from reference 
	/// @param ref 
    template<typename... Args>
	ID_INLINE static crAutoPointer<t_, a_> New( Args&&... args );
	
	// Destroy object from reference 
	ID_INLINE static void                   Delete(  crAutoPointer<t_, a_> &ref );

	ID_INLINE crAutoPointer<t_, a_> operator = ( const crAutoPointer<t_, a_> &ref ) 
	{
		// if already have a reference
		if( m_object != nullptr )
		{
			if ( DecRefCount( m_object ) < 1 )
				Delete( *this );
		}
		
		// reference the pointer 
		m_object = const_cast<pointer>( ref.m_object );
		
		// incrase pointer reference
		if( m_object != nullptr )
			IncRefCount( m_object );

		return *this;
	}

	/// @brief Re cast from types to diferents types
	/// @tparam u_ new cast type
	/// @param  
	/// @return 
	template< typename u_ >
	ID_INLINE crAutoPointer<u_, a_>	ReinterpretCast( void ) const
	{
        crAutoPointer<u_, a_> ref = crAutoPointer<u_, a_>(); // empty ref
        ref.m_object = dynamic_cast<u_*>( m_object ); // cast to type    
		return ref;
	}

	/// @brief 
	/// @tparam u_ 
	/// @param  
	/// @return 
	template< typename u_ >
	ID_INLINE crAutoPointer<u_, a_>	DynamicCast( void ) const
	{
        crAutoPointer<u_, a_> ref = crAutoPointer<u_, a_>(); // empty ref
        &ref = dynamic_cast<u_*>( m_object ); // cast to type 
		return ref; // return reference
	}
	
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
	
	ID_INLINE bool			operator == ( const crAutoPointer<t_, a_> & ref ) const;
	ID_INLINE bool			operator != ( const crAutoPointer<t_, a_> & ref ) const;
	ID_INLINE t_*			operator -> ( void );
	ID_INLINE const t_*		operator -> ( void ) const;
	ID_INLINE t_*			operator & ( void );
	ID_INLINE const t_*	 	operator & ( void ) const;
	ID_INLINE t_&			operator *(void);
	ID_INLINE const t_&		operator *( void ) const;

	ID_INLINE t_&			operator[]( int index );
	ID_INLINE const t_&		operator[]( int index ) const;

	ID_INLINE explicit operator bool( void ) const noexcept;

private:
    pointer	m_object;

	uint32_t IncRefCount( const_pointer ptr );
	uint32_t DecRefCount( const_pointer ptr );
};

/*
===========================================
crAutoPointer::crAutoPointer
===========================================
*/
template<typename t_, typename a_>
crAutoPointer<t_, a_>::crAutoPointer( void ) : m_object( nullptr )
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
	if( m_object != nullptr )
	{
        // decrease reference
		if ( DecRefCount( m_object ) < 1 )
			Delete( *this );
	}

	// copy pointer endress
	m_object = ptr.m_object;
	
	// incrase pointer reference
	if( m_object != nullptr )
		IncRefCount( m_object );
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
	if( m_object != nullptr )
	{
		if ( DecRefCount( m_object ) < 1 )
			Delete( *this );
	}
}

/*
===========================================
crAutoPointer::Alloc
===========================================
*/
template <typename t_, typename a_>
ID_INLINE crAutoPointer<t_, a_> crAutoPointer<t_, a_>::Alloc( const uint32_t count )
{
	a_ alloc; // allocator structure 

    // new reference pointer holder 
    crAutoPointer<t_, a_> newptr = crAutoPointer<t_, a_>();

    // Allocate the atomic counter at pointer begining 
	void* ptr = alloc.Allocate( ( sizeof(t_) * count ) + sizeof( uintptr_t ) );
    assert(ptr != nullptr); //

    // set counter to zero
    *static_cast<uintptr_t*>( ptr ) = 0;
        
	// hide atomic counter
	newptr.m_object = reinterpret_cast<pointer>( reinterpret_cast<uintptr_t>( ptr ) + sizeof(uintptr_t) );

    return newptr;
}

/*
===========================================
crAutoPointer::Realloc
===========================================
*/
template <typename t_, typename a_>
ID_INLINE crAutoPointer<t_, a_> crAutoPointer<t_, a_>::Realloc( crAutoPointer<t_, a_> &ref, const uint32_t count )
{
	a_ alloc;
	if ( ref.m_object == nullptr )
	{
        // Allocate the atomic counter at pointer begining 
	    void* ptr = alloc.Allocate( ( sizeof(t_) * count ) + sizeof( uintptr_t ) );
        assert(ptr != nullptr); //

        // set counter to zero
        *static_cast<uintptr_t*>( ptr ) = 0;
        
	    // hide atomic counter
	    ref.m_object = reinterpret_cast<pointer>( reinterpret_cast<uintptr_t>( ptr ) + sizeof(uintptr_t) );
	}
	else
	{
		// get whole pointer
		void* ptr = reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( ref.m_object ) - sizeof(uint32_t) );
        assert(ptr != nullptr); //

		// realocate memory
		ptr =  alloc.Reallocate( ptr, ( sizeof(t_) * count ) + sizeof( uintptr_t ) );
        assert(ptr != nullptr); //

		// hide counter
		ref.m_object = reinterpret_cast<pointer>( reinterpret_cast<uintptr_t>( ptr ) + sizeof( uint32_t ) );
	}

    return ref;
}

/*
===========================================
crAutoPointer::Free
===========================================
*/
template <typename t_, typename a_>
ID_INLINE void crAutoPointer<t_, a_>::Free( crAutoPointer<t_, a_> &ref )
{
	a_ alloc;
	if ( ref.m_object != nullptr )
	{
		// get whole pointer
		void* ptr = reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( ref.m_object ) + sizeof(uintptr_t) );
        assert(ptr != nullptr); //

		// release
		alloc.Deallocate( ptr );
		ref.m_object = nullptr;
	}
}


/*
===========================================
crAutoPointer::New
===========================================
*/
template <typename t_, typename a_>
template <typename... Args>
ID_INLINE crAutoPointer<t_, a_> crAutoPointer<t_, a_>::New( Args &&...args )
{
    a_ alloc; // allocator structure 

    // new reference pointer holder 
    crAutoPointer<t_, a_> newptr = crAutoPointer<t_, a_>();

    // Allocate the atomic counter at pointer begining 
	void* ptr = alloc.Allocate( sizeof(t_) + sizeof( uintptr_t ) );
    assert(ptr != nullptr); //

    // set counter to zero
    *static_cast<uintptr_t*>( ptr ) = 0;
        
	// hide atomic counter
	newptr.m_object = reinterpret_cast<pointer>( reinterpret_cast<uintptr_t>( ptr ) + sizeof(uintptr_t) );
    
    // construct the class 
    new( newptr.m_object ) t_( std::forward<Args>(args)... ); 

    return newptr;
}

/*
===========================================
crAutoPointer::Delete
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crAutoPointer<t_, a_>::Delete( crAutoPointer<t_, a_> &ref )
{
	a_ alloc;
	if ( ref.m_object != nullptr )
	{
		reinterpret_cast<pointer>( ref.m_object )->~t_();

        // get whole pointer
		void* ptr = reinterpret_cast<void*>( reinterpret_cast<uintptr_t>( ref.m_object ) + sizeof(uintptr_t) );

		// release
		alloc.Deallocate( ptr );
		ref.m_object = nullptr;
	}
}


/*
===========================================
crAutoPointer::Memcpy
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crAutoPointer<t_, a_>::Memcpy(const void *src, const size_t first, const size_t count )
{
	::memcpy( static_cast<void*>(&m_object[first]), src, sizeof( t_ ) * count  );
}

/*
===========================================
crAutoPointer::Memcmp
===========================================
*/
template< typename t_, class a_ >
ID_INLINE bool crAutoPointer<t_, a_>::Memcmp(const void *src, const size_t first, const size_t count )
{
    return ::memcmp( static_cast<void*>(&m_object[first]) , src, sizeof( t_ ) * count  ) == 0;
}

/*

===========================================
crAutoPointer::Memset
===========================================
*/
template< typename t_, class a_ >
ID_INLINE void crAutoPointer<t_, a_>::Memset( const int c, const size_t first, size_t count )
{
	::memset( static_cast<void*>(&m_object[first]), c, sizeof( t_ ) * count );
}

/*
===========================================
crAutoPointer::Size
===========================================
*/
template< typename t_, class a_ >
ID_INLINE size_t crAutoPointer<t_, a_>::Size(void) const
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
crAutoPointer:: ==
===========================================
*/
template< typename t_, class a_ >
ID_INLINE bool crAutoPointer<t_, a_>::operator == ( const crAutoPointer<t_, a_> & ref ) const
{
	return m_object == ref.m_object;
}

/*
===========================================
crAutoPointer:: != 
===========================================
*/
template< typename t_, class a_ >
ID_INLINE bool crAutoPointer<t_, a_>::operator != ( const crAutoPointer<t_, a_> & ref ) const
{
	return m_object != ref.m_object;
}

/*
===========================================
crAutoPointer::->
===========================================
*/
template< typename t_, class a_ >
ID_INLINE t_* crAutoPointer<t_, a_>::operator->(void)
{
    return m_object;
}

/*
===========================================
crAutoPointer::->
===========================================
*/
template< typename t_, class a_ >
ID_INLINE const t_* crAutoPointer<t_, a_>::operator->(void) const
{
    return m_object;
}

/*
===========================================
crAutoPointer::&
===========================================
*/
template< typename t_, class a_ >
ID_INLINE t_* crAutoPointer<t_, a_>::operator&(void)
{
    return m_object;
}

/*
===========================================
crAutoPointer::&
===========================================
*/
template< typename t_, class a_ >
ID_INLINE const t_* crAutoPointer<t_, a_>::operator&(void) const
{
    return m_object;
}

/*
===========================================
crAutoPointer::*
===========================================
*/
template< typename t_, class a_ >
ID_INLINE t_& crAutoPointer<t_, a_>::operator*(void)
{
    return *m_object;
}

/*
===========================================
crAutoPointer::*
===========================================
*/
template< typename t_, class a_ >
ID_INLINE const t_& crAutoPointer<t_, a_>::operator*(void) const
{
    return *m_object;
}

/*
===========================================
crAutoPointer::[]
===========================================
*/
template< typename t_, class a_ >
ID_INLINE t_& crAutoPointer<t_, a_>::operator[]( int index )
{
	if ( index <= ( sizeof( m_object ) / sizeof( t_ ) ) )
		throw std::out_of_range( "array index out of range!" );
	
    return m_object[index];
}

/*
===========================================
crAutoPointer::[]
===========================================
*/
template< typename t_, class a_ >
ID_INLINE const t_& crAutoPointer<t_, a_>::operator[]( int index ) const
{
	if ( index <= ( sizeof( m_object ) / sizeof( t_ ) ) )
		throw std::out_of_range( "array index out of range!" );

    return m_object[index];
}

/*
===========================================
crAutoPointer::bool
===========================================

*/
template< typename t_, class a_ >
ID_INLINE crAutoPointer<t_, a_>::operator bool(void) const noexcept
{
	return m_object != nullptr;
}

/*
===========================================
crAutoPointer::IncRefCount
===========================================
*/
template <typename t_, typename a_>
ID_INLINE uint32_t crAutoPointer<t_, a_>::IncRefCount( const_pointer ptr )
{
	assert( ptr != nullptr );
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
	assert( ptr != nullptr );
	uint32_t* val = reinterpret_cast<uint32_t*>( reinterpret_cast<uintptr_t>( ptr ) - sizeof( int32_t ) );
#ifdef USE_GCC_ATTOMIC
		return atomic_fetch_sub( val, 1 ) - 1;
#else
		return __sync_sub_and_fetch( val, 1 ) - 1;
#endif
}

#endif //!__AUTO_REFERENCE_POINTER_H__