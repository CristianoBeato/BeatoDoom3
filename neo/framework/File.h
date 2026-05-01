/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 GPL Source Code (?Doom 3 Source Code?).  

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#ifndef __FILE_H__
#define __FILE_H__

/*
==============================================================

  File Streams.

==============================================================
*/

// mode parm for Seek
typedef enum {
	FS_SEEK_CUR,
	FS_SEEK_END,
	FS_SEEK_SET
} fsOrigin_t;

class idFileSystemLocal;


class idFile {
public:
	virtual					~idFile( void ) {};
							// Get the name of the file.
	virtual const char *	GetName( void );
							// Get the full file path.
	virtual const char *	GetFullPath( void );
							// Read data from the file to the buffer.
	virtual int				Read( void *buffer, int len );
							// Write data from the buffer to the file.
	virtual int				Write( const void *buffer, int len );
							// Returns the length of the file.
	virtual int				Length( void );
							// Return a time value for reload operations.
	virtual ID_TIME_T			Timestamp( void );
							// Returns offset in file.
	virtual int				Tell( void );
							// Forces flush on files being writting to.
	virtual void			ForceFlush( void );
							// Causes any buffered data to be written to the file.
	virtual void			Flush( void );
							// Seek on a file.
	virtual int				Seek( long offset, fsOrigin_t origin );
							// Go back to the beginning of the file.
	virtual void			Rewind( void );
							// Like fprintf.
	virtual int				Printf( const char *fmt, ... ) id_attribute((format(printf,2,3)));
							// Like fprintf but with argument pointer
	virtual int				VPrintf( const char *fmt, va_list arg );
							// Write a string with high precision floating point numbers to the file.
	virtual int				WriteFloatString( const char *fmt, ... ) id_attribute((format(printf,2,3)));
	
	// Endian portable alternatives to Read(...)
	ID_INLINE size_t		ReadLong( int64_t &value )
	{
		size_t l = Read( &value, sizeof( int64_t ) );
		value = SDL_Swap64LE( value );
		return l;
	}

	ID_INLINE size_t		ReadUnsignedLong( uint64_t &value )
	{
		size_t l = Read( &value, sizeof( uint64_t ) );
		value = SDL_Swap64LE( value );
		return l;
	}

	ID_INLINE size_t	ReadInt( int32_t &value )
	{
		size_t l = Read( &value, sizeof( int32_t ) );
		value = SDL_Swap32LE( value );
		return l;
	}

	ID_INLINE size_t	ReadUnsignedInt( uint32_t &value )
	{
		size_t result = Read( &value, sizeof( uint32_t ) );
		value = SDL_Swap32LE( value );
		return result;
	}

	ID_INLINE size_t	ReadShort( int16_t &value )
	{
		size_t result = Read( &value, sizeof( int16_t ) );
		value = SDL_Swap16LE( value );
		return result;
	}

	ID_INLINE size_t	ReadUnsignedShort( uint16_t &value )
	{
		size_t result = Read( &value, sizeof( uint16_t ) );
		value = LittleShort(value);
		return result;
	}

	ID_INLINE size_t	ReadFloat( float &value )
	{
		size_t result = Read( &value, sizeof( float ) );
		value = SDL_SwapFloatLE( value );
		return result;
	}

	ID_INLINE size_t	ReadChar( char &value )
	{
		return Read( &value, sizeof( char ) );
	}


	ID_INLINE size_t	ReadUnsignedChar( unsigned char &value )
	{
		return Read( &value, sizeof( unsigned char ) );
	}

	ID_INLINE size_t		ReadBool( bool &value )
	{
		unsigned char c;
		size_t result = Read( &c, sizeof( unsigned char ) );
		value = c != 0;
		return result;
	}

	virtual size_t		ReadString( idStr &string );
	virtual size_t		ReadVec2( idVec2 &vec );
	virtual size_t		ReadVec3( idVec3 &vec );
	virtual size_t		ReadVec4( idVec4 &vec );
	virtual size_t		ReadVec6( idVec6 &vec );
	virtual size_t		ReadMat3( idMat3 &mat );
	
	// Endian portable alternatives to Write(...)
	ID_INLINE size_t	WriteLong( const int64_t value )
	{
		int64_t v = SDL_Swap64LE( value );
		return Write( &v, sizeof( int64_t ) );
	}

	ID_INLINE size_t	WriteUnsignedLong( const uint64_t value )
	{
		uint64_t v = SDL_Swap64LE( value );
		return Write( &v, sizeof( uint64_t ) );
	}
	
	ID_INLINE size_t	WriteInt( const int32_t value )
	{
		int32_t v = SDL_Swap32LE( value );
		return Write( &v, sizeof( int32_t ) );
	}

	ID_INLINE size_t	WriteUnsignedInt( const uint32_t value )
	{
		uint32_t v = SDL_Swap32LE( value );
		return Write( &v, sizeof( uint32_t ) );
	}

	ID_INLINE size_t	WriteShort( const int16_t value )
	{
		int16_t v = SDL_Swap16LE( value );
		return Write( &v, sizeof( int16_t ) );
	}
	
	ID_INLINE size_t	WriteUnsignedShort( unsigned short value )
	{
		uint16_t v = SDL_Swap16LE( value );
		return Write( &v, sizeof( uint16_t ) );
	}
	
	ID_INLINE size_t	WriteChar( const char value )
	{
		return Write( &value, sizeof( char ) );
	}

	ID_INLINE size_t	WriteUnsignedChar( const unsigned char value )
	{
		return Write( &value, sizeof( unsigned char ) );
	}

	ID_INLINE size_t	WriteFloat( const float value )
	{
		float f = SDL_SwapFloatLE( value );
		return Write( &f, sizeof( float ) );
	}

	ID_INLINE size_t	WriteBool( const bool value )
	{
		unsigned char c = value;
		return WriteUnsignedChar( c );
	}

	virtual int				WriteString( const char *string );
	virtual int				WriteVec2( const idVec2 &vec );
	virtual int				WriteVec3( const idVec3 &vec );
	virtual int				WriteVec4( const idVec4 &vec );
	virtual int				WriteVec6( const idVec6 &vec );
	virtual int				WriteMat3( const idMat3 &mat );
};


class idFile_Memory : public idFile {
	friend class			idFileSystemLocal;

public:
							idFile_Memory( void );	// file for writing without name
							idFile_Memory( const char *name );	// file for writing
							idFile_Memory( const char *name, char *data, int length );	// file for writing
							idFile_Memory( const char *name, const char *data, int length );	// file for reading
	virtual					~idFile_Memory( void );

	virtual const char *	GetName( void ) { return name.c_str(); }
	virtual const char *	GetFullPath( void ) { return name.c_str(); }
	virtual int				Read( void *buffer, int len );
	virtual int				Write( const void *buffer, int len );
	virtual int				Length( void );
	virtual ID_TIME_T			Timestamp( void );
	virtual int				Tell( void );
	virtual void			ForceFlush( void );
	virtual void			Flush( void );
	virtual int				Seek( long offset, fsOrigin_t origin );

							// changes memory file to read only
	virtual void			MakeReadOnly( void );
							// clear the file
	virtual void			Clear( bool freeMemory = true );
							// set data for reading
	void					SetData( const char *data, int length );
							// returns const pointer to the memory buffer
	const char *			GetDataPtr( void ) const { return filePtr; }
							// set the file granularity
	void					SetGranularity( int g ) { assert( g > 0 ); granularity = g; }

private:
	idStr					name;			// name of the file
	int						mode;			// open mode
	int						maxSize;		// maximum size of file
	int						fileSize;		// size of the file
	int						allocated;		// allocated size
	int						granularity;	// file granularity
	char *					filePtr;		// buffer holding the file data
	char *					curPtr;			// current read/write pointer
};


class idFile_BitMsg : public idFile {
	friend class			idFileSystemLocal;

public:
							idFile_BitMsg( idBitMsg &msg );
							idFile_BitMsg( const idBitMsg &msg );
	virtual					~idFile_BitMsg( void );

	virtual const char *	GetName( void ) { return name.c_str(); }
	virtual const char *	GetFullPath( void ) { return name.c_str(); }
	virtual int				Read( void *buffer, int len );
	virtual int				Write( const void *buffer, int len );
	virtual int				Length( void );
	virtual ID_TIME_T			Timestamp( void );
	virtual int				Tell( void );
	virtual void			ForceFlush( void );
	virtual void			Flush( void );
	virtual int				Seek( long offset, fsOrigin_t origin );

private:
	idStr					name;			// name of the file
	int						mode;			// open mode
	idBitMsg *				msg;
};


class idFile_Permanent : public idFile {
	friend class			idFileSystemLocal;

public:
							idFile_Permanent( void );
	virtual					~idFile_Permanent( void );

	virtual const char *	GetName( void ) { return name.c_str(); }
	virtual const char *	GetFullPath( void ) { return fullPath.c_str(); }
	virtual int				Read( void *buffer, int len );
	virtual int				Write( const void *buffer, int len );
	virtual int				Length( void );
	virtual ID_TIME_T			Timestamp( void );
	virtual int				Tell( void );
	virtual void			ForceFlush( void );
	virtual void			Flush( void );
	virtual int				Seek( long offset, fsOrigin_t origin );

	// returns file pointer
	FILE *					GetFilePtr( void ) { return o; }

private:
	idStr					name;			// relative path of the file - relative path
	idStr					fullPath;		// full file path - OS path
	int						mode;			// open mode
	int						fileSize;		// size of the file
	FILE *					o;				// file handle
	bool					handleSync;		// true if written data is immediately flushed
};


class idFile_InZip : public idFile {
	friend class			idFileSystemLocal;

public:
							idFile_InZip( void );
	virtual					~idFile_InZip( void );

	virtual const char *	GetName( void ) { return name.c_str(); }
	virtual const char *	GetFullPath( void ) { return fullPath.c_str(); }
	virtual int				Read( void *buffer, int len );
	virtual int				Write( const void *buffer, int len );
	virtual int				Length( void );
	virtual ID_TIME_T			Timestamp( void );
	virtual int				Tell( void );
	virtual void			ForceFlush( void );
	virtual void			Flush( void );
	virtual int				Seek( long offset, fsOrigin_t origin );

private:
	idStr					name;			// name of the file in the pak
	idStr					fullPath;		// full file path including pak file name
	int						zipFilePos;		// zip file info position in pak
	int						fileSize;		// size of the file
	void *					z;				// unzip info
};

#endif /* !__FILE_H__ */
