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

// Based on Linderdaum Engine http ://www.linderdaum.com
// Copyright( C ) 2013 Sergey Kosarevsky( sk@linderdaum.com )
// Copyright( C ) 2013 Viktor Latypov( vl@linderdaum.com )
// All rights reserved.

class btObjectCounter : public btAtommicCounter
{
public:
	btObjectCounter( void ) {};
	~btObjectCounter( void ) {};

	ID_INLINE void	IncRefCount( void ) { Increment(); };
	ID_INLINE void	DecRefCount( void ) { if (Decrement() < 1) delete this; };

	static void IncRef( void* ptr );
	static void DecRef( void* ptr );
};

template<class t_ >
class btRefCntAutoPtr
{
public:
	typedef t_ value_type;
	typedef t_ *pointer;
	typedef const t_ *const_pointer;
	typedef t_ &reference;
	typedef const t_ &const_reference;

	btRefCntAutoPtr( void ) : m_ptr( nullptr ) {};

	//
	btRefCntAutoPtr( const btRefCntAutoPtr& Ptr ) : m_ptr( Ptr.m_ptr )
	{
		btObjectCounter::IncRef( m_ptr );
	}

	// Construtor de referencia
	// Reference constructor
	template <typename U>
	btRefCntAutoPtr( const btRefCntAutoPtr<U>& Ptr ) : m_ptr( (pointer)Ptr.GetInternalPtr() )
	{
		btObjectCounter::IncRef( m_ptr );
	}

	// Construtor para converção implicita de tipo
	// Constructor for an implicit type conversion
	btRefCntAutoPtr( pointer Object ) : m_ptr( Object )
	{
		btObjectCounter::IncRef( m_ptr );
	}

	// Destrutor de referencia 
	// Reference destructor
	~btRefCntAutoPtr( void )
	{
		btObjectCounter::DecRef( (void*)m_ptr );
	};

	// Adquire o ponteiro interno para
	// Retrieve an internal pointer to
	ID_INLINE const_pointer GetInternalPtr( void ) const
	{
		return m_ptr;
	}

	// Operador de referencia
	// The dereference operator
	ID_INLINE pointer operator -> ( void )
	{
		assert( m_ptr );
		return m_ptr;
	}

	//
	//
	ID_INLINE const_pointer operator -> ( void ) const
	{
		assert( m_ptr );
		return m_ptr;
	}

	// Imita o comportamento da operador dynamic_cast
	// Mimic a dynamic_cast behavior
	template <typename U>
	ID_INLINE btRefCntAutoPtr<U> DynamicCast( void ) const
	{
		return btRefCntAutoPtr<U>( dynamic_cast<U*>(m_ptr) );
	}

	// Imita o comportamento da operador dynamic_cast
	// Mimic a dynamic_cast behavior
	template <typename U>
	ID_INLINE btRefCntAutoPtr<U> StaticCast( void ) const
	{
		return btRefCntAutoPtr<U>( static_cast<U*>(m_ptr) );
	}

	ID_INLINE btRefCntAutoPtr<value_type> ConstCast( void ) const
	{
		return btRefCntAutoPtr<value_type>( const_cast<pointer>(m_ptr) );
	}

	// Operador de comparação de equalidade
	// Compare equality operator
	ID_INLINE bool operator == ( const t_* Ptr1 ) const
	{
		return m_ptr == Ptr1;
	}

	// Mesmo de cima
	// Same as above
	template <typename U>
	ID_INLINE bool operator == ( const btRefCntAutoPtr<U>& Ptr1 ) const
	{
		return m_ptr == Ptr1.GetInternalPtr();
	}

	// Operador de comparação de Diferença
	// Diference comparation operator
	ID_INLINE bool operator != ( const pointer Ptr1 ) const
	{
		return m_ptr != Ptr1;
	}


	template <typename U>
	ID_INLINE bool operator != ( const btRefCntAutoPtr<U>& Ptr1 ) const
	{
		return m_ptr != Ptr1.GetInternalPtr();
	}

	// Operador de assignamento
	// Assignment operator
	btRefCntAutoPtr& operator = ( const btRefCntAutoPtr& Ptr )
	{
		pointer Temp = m_ptr;
		m_ptr = Ptr.m_ptr;
		btObjectCounter::IncRef( m_ptr );
		btObjectCounter::DecRef( Temp );
		return *this;
	}

	void	ReleaseReference( void ) const
	{
		btObjectCounter::DecRef( m_ptr );
		const_cast<pointer>(m_ptr) = nullptr;
	}

private:
	pointer m_ptr;

private:
	class clProtector
	{
	private:
		void operator delete(void*);
	};
public:
	ID_INLINE operator clProtector* (void) const
	{
		if (m_ptr == nullptr)
			return nullptr;

		static clProtector Protector;
		return &Protector;
	}
};

#endif // !_SMART_POINTER_H_
