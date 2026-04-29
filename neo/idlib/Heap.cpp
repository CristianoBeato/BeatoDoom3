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

#include "../idlib/precompiled.h"
#pragma hdrstop

#ifndef USE_LIBC_MALLOC
	#define USE_LIBC_MALLOC		0
#endif

#ifndef CRASH_ON_STATIC_ALLOCATION
//	#define CRASH_ON_STATIC_ALLOCATION
#endif

//===============================================================
//
//	memory allocation all in one place
//
//===============================================================

#undef new

static memoryStats_t	mem_total_allocs = { 0, 0x0fffffff, 0, 0 };
static memoryStats_t	mem_frame_allocs;
static memoryStats_t	mem_frame_frees;

/*
==================
Mem_ClearFrameStats
==================
*/
void Mem_ClearFrameStats( void ) 
{
	mem_frame_allocs.num = mem_frame_frees.num = 0;
	mem_frame_allocs.minSize = mem_frame_frees.minSize = 0x0fffffff;
	mem_frame_allocs.maxSize = mem_frame_frees.maxSize = -1;
	mem_frame_allocs.totalSize = mem_frame_frees.totalSize = 0;
}

/*
==================
Mem_GetFrameStats
==================
*/
void Mem_GetFrameStats( memoryStats_t &allocs, memoryStats_t &frees ) 
{
	allocs = mem_frame_allocs;
	frees = mem_frame_frees;
}

/*
==================
Mem_GetStats
==================
*/
void Mem_GetStats( memoryStats_t &stats ) 
{
	stats = mem_total_allocs;
}

/*
==================
Mem_UpdateStats
==================
*/
void Mem_UpdateStats( memoryStats_t &stats, const size_t size ) 
{
	stats.num++;
	if ( size < stats.minSize ) 
		stats.minSize = size;

	if ( size > stats.maxSize ) 
		stats.maxSize = size;
	
	stats.totalSize += size;
}

/*
==================
Mem_UpdateAllocStats
==================
*/
void Mem_UpdateAllocStats( int size ) 
{
	Mem_UpdateStats( mem_frame_allocs, size );
	Mem_UpdateStats( mem_total_allocs, size );
}

/*
==================
Mem_UpdateFreeStats
==================
*/
void Mem_UpdateFreeStats( int size ) 
{
	Mem_UpdateStats( mem_frame_frees, size );
	mem_total_allocs.num--;
	mem_total_allocs.totalSize -= size;
}


#ifndef ID_DEBUG_MEMORY

/*
==================
Mem_ClearedAlloc
==================
*/
void Mem_AllocDefragBlock( void ) 
{
}

/*
==================
Mem_CopyString
==================
*/
char *Mem_CopyString( const char *in ) 
{
	char	*out;
	out = (char *)SDL_malloc( strlen( in ) + 1 );
	strcpy( out, in );
	return out;
}

/*
==================
Mem_Dump_f
==================
*/
void Mem_Dump_f( const idCmdArgs &args ) 
{
}

/*
==================
Mem_DumpCompressed_f
==================
*/
void Mem_DumpCompressed_f( const idCmdArgs &args ) 
{
}

/*
==================
Mem_Init
==================
*/
void Mem_Init( void ) 
{
	Mem_ClearFrameStats();
}

/*
==================
Mem_Shutdown
==================
*/
void Mem_Shutdown( void ) 
{
}

/*
==================
Mem_EnableLeakTest
==================
*/
void Mem_EnableLeakTest( const char *name )
{
}


#else /* !ID_DEBUG_MEMORY */

#undef		Mem_Alloc
#undef		Mem_ClearedAlloc
#undef		Com_ClearedReAlloc
#undef		Mem_Free
#undef		Mem_CopyString
#undef		Mem_Alloc16
#undef		Mem_Free16

#define MAX_CALLSTACK_DEPTH		6

// size of this struct must be a multiple of 16 bytes
typedef struct debugMemory_s {
	const char *			fileName;
	int						lineNumber;
	int						frameNumber;
	int						size;
	address_t				callStack[MAX_CALLSTACK_DEPTH];
	struct debugMemory_s *	prev;
	struct debugMemory_s *	next;
} debugMemory_t;

static debugMemory_t *	mem_debugMemory = NULL;
static char				mem_leakName[256] = "";

/*
==================
Mem_CleanupFileName
==================
*/
const char *Mem_CleanupFileName( const char *fileName ) {
	int i1, i2;
	idStr newFileName;
	static char newFileNames[4][MAX_STRING_CHARS];
	static int index;

	newFileName = fileName;
	newFileName.BackSlashesToSlashes();
	i1 = newFileName.Find( "neo", false );
	if ( i1 >= 0 ) {
		i1 = newFileName.Find( "/", false, i1 );
		newFileName = newFileName.Right( newFileName.Length() - ( i1 + 1 ) );
	}
	while( 1 ) {
		i1 = newFileName.Find( "/../" );
		if ( i1 <= 0 ) {
			break;
		}
		i2 = i1 - 1;
		while( i2 > 1 && newFileName[i2-1] != '/' ) {
			i2--;
		}
		newFileName = newFileName.Left( i2 - 1 ) + newFileName.Right( newFileName.Length() - ( i1 + 4 ) );
	}
	index = ( index + 1 ) & 3;
	strncpy( newFileNames[index], newFileName.c_str(), sizeof( newFileNames[index] ) );
	return newFileNames[index];
}

/*
==================
Mem_Dump
==================
*/
void Mem_Dump( const char *fileName ) {
	int i, numBlocks, totalSize;
	char dump[32], *ptr;
	debugMemory_t *b;
	idStr module, funcName;
	FILE *f;

	f = fopen( fileName, "wb" );
	if ( !f ) {
		return;
	}

	totalSize = 0;
	for ( numBlocks = 0, b = mem_debugMemory; b; b = b->next, numBlocks++ ) {
		ptr = ((char *) b) + sizeof(debugMemory_t);
		totalSize += b->size;
		for ( i = 0; i < (sizeof(dump)-1) && i < b->size; i++) {
			if ( ptr[i] >= 32 && ptr[i] < 127 ) {
				dump[i] = ptr[i];
			} else {
				dump[i] = '_';
			}
		}
		dump[i] = '\0';
		if ( ( b->size >> 10 ) != 0 ) {
			fprintf( f, "size: %6d KB: %s, line: %d [%s], call stack: %s\r\n", ( b->size >> 10 ), Mem_CleanupFileName(b->fileName), b->lineNumber, dump, idLib::sys->GetCallStackStr( b->callStack, MAX_CALLSTACK_DEPTH ) );
		}
		else {
			fprintf( f, "size: %7d B: %s, line: %d [%s], call stack: %s\r\n", b->size, Mem_CleanupFileName(b->fileName), b->lineNumber, dump, idLib::sys->GetCallStackStr( b->callStack, MAX_CALLSTACK_DEPTH ) );
		}
	}

	idLib::sys->ShutdownSymbols();

	fprintf( f, "%8d total memory blocks allocated\r\n", numBlocks );
	fprintf( f, "%8d KB memory allocated\r\n", ( totalSize >> 10 ) );

	fclose( f );
}

/*
==================
Mem_Dump_f
==================
*/
void Mem_Dump_f( const idCmdArgs &args ) {
	const char *fileName;

	if ( args.Argc() >= 2 ) {
		fileName = args.Argv( 1 );
	}
	else {
		fileName = "memorydump.txt";
	}
	Mem_Dump( fileName );
}

/*
==================
Mem_DumpCompressed
==================
*/
typedef struct allocInfo_s {
	const char *			fileName;
	int						lineNumber;
	int						size;
	int						numAllocs;
	address_t				callStack[MAX_CALLSTACK_DEPTH];
	struct allocInfo_s *	next;
} allocInfo_t;

typedef enum {
	MEMSORT_SIZE,
	MEMSORT_LOCATION,
	MEMSORT_NUMALLOCS,
	MEMSORT_CALLSTACK
} memorySortType_t;

void Mem_DumpCompressed( const char *fileName, memorySortType_t memSort, int sortCallStack, int numFrames ) {
	int numBlocks, totalSize, r, j;
	debugMemory_t *b;
	allocInfo_t *a, *nexta, *allocInfo = NULL, *sortedAllocInfo = NULL, *prevSorted, *nextSorted;
	idStr module, funcName;
	FILE *f;

	// build list with memory allocations
	totalSize = 0;
	numBlocks = 0;
	for ( b = mem_debugMemory; b; b = b->next ) {

		if ( numFrames && b->frameNumber < idLib::frameNumber - numFrames ) {
			continue;
		}

		numBlocks++;
		totalSize += b->size;

		// search for an allocation from the same source location
		for ( a = allocInfo; a; a = a->next ) {
			if ( a->lineNumber != b->lineNumber ) {
				continue;
			}
			for ( j = 0; j < MAX_CALLSTACK_DEPTH; j++ ) {
				if ( a->callStack[j] != b->callStack[j] ) {
					break;
				}
			}
			if ( j < MAX_CALLSTACK_DEPTH ) {
				continue;
			}
			if ( idStr::Cmp( a->fileName, b->fileName ) != 0 ) {
				continue;
			}
			a->numAllocs++;
			a->size += b->size;
			break;
		}

		// if this is an allocation from a new source location
		if ( !a ) {
			a = (allocInfo_t *) ::malloc( sizeof( allocInfo_t ) );
			a->fileName = b->fileName;
			a->lineNumber = b->lineNumber;
			a->size = b->size;
			a->numAllocs = 1;
			for ( j = 0; j < MAX_CALLSTACK_DEPTH; j++ ) {
				a->callStack[j] = b->callStack[j];
			}
			a->next = allocInfo;
			allocInfo = a;
		}
	}

	// sort list
	for ( a = allocInfo; a; a = nexta ) {
		nexta = a->next;

		prevSorted = NULL;
		switch( memSort ) {
			// sort on size
			case MEMSORT_SIZE: {
				for ( nextSorted = sortedAllocInfo; nextSorted; nextSorted = nextSorted->next ) {
					if ( a->size > nextSorted->size ) {
						break;
					}
					prevSorted = nextSorted;
				}
				break;
			}
			// sort on file name and line number
			case MEMSORT_LOCATION: {
				for ( nextSorted = sortedAllocInfo; nextSorted; nextSorted = nextSorted->next ) {
					r = idStr::Cmp( Mem_CleanupFileName( a->fileName ), Mem_CleanupFileName( nextSorted->fileName ) );
					if ( r < 0 || ( r == 0 && a->lineNumber < nextSorted->lineNumber ) ) {
						break;
					}
					prevSorted = nextSorted;
				}
				break;
			}
			// sort on the number of allocations
			case MEMSORT_NUMALLOCS: {
				for ( nextSorted = sortedAllocInfo; nextSorted; nextSorted = nextSorted->next ) {
					if ( a->numAllocs > nextSorted->numAllocs ) {
						break;
					}
					prevSorted = nextSorted;
				}
				break;
			}
			// sort on call stack
			case MEMSORT_CALLSTACK: {
				for ( nextSorted = sortedAllocInfo; nextSorted; nextSorted = nextSorted->next ) {
					if ( a->callStack[sortCallStack] < nextSorted->callStack[sortCallStack] ) {
						break;
					}
					prevSorted = nextSorted;
				}
				break;
			}
		}
		if ( !prevSorted ) {
			a->next = sortedAllocInfo;
			sortedAllocInfo = a;
		}
		else {
			prevSorted->next = a;
			a->next = nextSorted;
		}
	}

	f = fopen( fileName, "wb" );
	if ( !f ) {
		return;
	}

	// write list to file
	for ( a = sortedAllocInfo; a; a = nexta ) {
		nexta = a->next;
		fprintf( f, "size: %6d KB, allocs: %5d: %s, line: %d, call stack: %s\r\n",
					(a->size >> 10), a->numAllocs, Mem_CleanupFileName(a->fileName),
							a->lineNumber, idLib::sys->GetCallStackStr( a->callStack, MAX_CALLSTACK_DEPTH ) );
		::free( a );
	}

	idLib::sys->ShutdownSymbols();

	fprintf( f, "%8d total memory blocks allocated\r\n", numBlocks );
	fprintf( f, "%8d KB memory allocated\r\n", ( totalSize >> 10 ) );

	fclose( f );
}

/*
==================
Mem_DumpCompressed_f
==================
*/
void Mem_DumpCompressed_f( const idCmdArgs &args ) {
	int argNum;
	const char *arg, *fileName;
	memorySortType_t memSort = MEMSORT_LOCATION;
	int sortCallStack = 0, numFrames = 0;

	// get cmd-line options
	argNum = 1;
	arg = args.Argv( argNum );
	while( arg[0] == '-' ) {
		arg = args.Argv( ++argNum );
		if ( idStr::Icmp( arg, "s" ) == 0 ) {
			memSort = MEMSORT_SIZE;
		} else if ( idStr::Icmp( arg, "l" ) == 0 ) {
			memSort = MEMSORT_LOCATION;
		} else if ( idStr::Icmp( arg, "a" ) == 0 ) {
			memSort = MEMSORT_NUMALLOCS;
		} else if ( idStr::Icmp( arg, "cs1" ) == 0 ) {
			memSort = MEMSORT_CALLSTACK;
			sortCallStack = 2;
		} else if ( idStr::Icmp( arg, "cs2" ) == 0 ) {
			memSort = MEMSORT_CALLSTACK;
			sortCallStack = 1;
		} else if ( idStr::Icmp( arg, "cs3" ) == 0 ) {
			memSort = MEMSORT_CALLSTACK;
			sortCallStack = 0;
		} else if ( arg[0] == 'f' ) {
			numFrames = atoi( arg + 1 );
		} else {
			idLib::common->Printf( "memoryDumpCompressed [options] [filename]\n"
						"options:\n"
						"  -s     sort on size\n"
						"  -l     sort on location\n"
						"  -a     sort on the number of allocations\n"
						"  -cs1   sort on first function on call stack\n"
						"  -cs2   sort on second function on call stack\n"
						"  -cs3   sort on third function on call stack\n"
						"  -f<X>  only report allocations the last X frames\n"
						"By default the memory allocations are sorted on location.\n"
						"By default a 'memorydump.txt' is written if no file name is specified.\n" );
			return;
		}
		arg = args.Argv( ++argNum );
	}
	if ( argNum >= args.Argc() ) {
		fileName = "memorydump.txt";
	} else {
		fileName = arg;
	}
	Mem_DumpCompressed( fileName, memSort, sortCallStack, numFrames );
}

/*
==================
Mem_AllocDebugMemory
==================
*/
void *Mem_AllocDebugMemory( const int size, const char *fileName, const int lineNumber, const bool align16 ) {
	void *p;
	debugMemory_t *m;

	if ( !size ) {
		return NULL;
	}

	if ( !mem_heap ) {
#ifdef CRASH_ON_STATIC_ALLOCATION
		*((int*)0x0) = 1;
#endif
		// NOTE: set a breakpoint here to find memory allocations before mem_heap is initialized
		return malloc( size );
	}

	if ( align16 ) {
		p = mem_heap->Allocate16( size + sizeof( debugMemory_t ) );
	}
	else {
		p = mem_heap->Allocate( size + sizeof( debugMemory_t ) );
	}

	Mem_UpdateAllocStats( size );

	m = (debugMemory_t *) p;
	m->fileName = fileName;
	m->lineNumber = lineNumber;
	m->frameNumber = idLib::frameNumber;
	m->size = size;
	m->next = mem_debugMemory;
	m->prev = NULL;
	if ( mem_debugMemory ) {
		mem_debugMemory->prev = m;
	}
	mem_debugMemory = m;
	idLib::sys->GetCallStack( m->callStack, MAX_CALLSTACK_DEPTH );

	return ( ( (byte *) p ) + sizeof( debugMemory_t ) );
}

/*
==================
Mem_FreeDebugMemory
==================
*/
void Mem_FreeDebugMemory( void *p, const char *fileName, const int lineNumber, const bool align16 ) {
	debugMemory_t *m;

	if ( !p ) {
		return;
	}

	if ( !mem_heap ) {
#ifdef CRASH_ON_STATIC_ALLOCATION
		*((int*)0x0) = 1;
#endif
		// NOTE: set a breakpoint here to find memory being freed before mem_heap is initialized
		free( p );
		return;
	}

	m = (debugMemory_t *) ( ( (byte *) p ) - sizeof( debugMemory_t ) );

	if ( m->size < 0 ) {
		idLib::common->FatalError( "memory freed twice, first from %s, now from %s", idLib::sys->GetCallStackStr( m->callStack, MAX_CALLSTACK_DEPTH ), idLib::sys->GetCallStackCurStr( MAX_CALLSTACK_DEPTH ) );
	}

	Mem_UpdateFreeStats( m->size );

	if ( m->next ) {
		m->next->prev = m->prev;
	}
	if ( m->prev ) {
		m->prev->next = m->next;
	}
	else {
		mem_debugMemory = m->next;
	}

	m->fileName = fileName;
	m->lineNumber = lineNumber;
	m->frameNumber = idLib::frameNumber;
	m->size = -m->size;
	idLib::sys->GetCallStack( m->callStack, MAX_CALLSTACK_DEPTH );

	if ( align16 ) {
 		mem_heap->Free16( m );
	}
	else {
 		mem_heap->Free( m );
	}
}

/*
==================
Mem_Alloc
==================
*/
void *Mem_Alloc( const int size, const char *fileName, const int lineNumber ) {
	if ( !size ) {
		return NULL;
	}
	return Mem_AllocDebugMemory( size, fileName, lineNumber, false );
}

/*
==================
Mem_Free
==================
*/
void Mem_Free( void *ptr, const char *fileName, const int lineNumber ) {
	if ( !ptr ) {
		return;
	}
	Mem_FreeDebugMemory( ptr, fileName, lineNumber, false );
}

/*
==================
Mem_Alloc16
==================
*/
void *Mem_Alloc16( const int size, const char *fileName, const int lineNumber ) {
	if ( !size ) {
		return NULL;
	}
	void *mem = Mem_AllocDebugMemory( size, fileName, lineNumber, true );
	// make sure the memory is 16 byte aligned
	assert( ( ((int)mem) & 15) == 0 );
	return mem;
}

/*
==================
Mem_Free16
==================
*/
void Mem_Free16( void *ptr, const char *fileName, const int lineNumber ) {
	if ( !ptr ) {
		return;
	}
	// make sure the memory is 16 byte aligned
	assert( ( ((int)ptr) & 15) == 0 );
	Mem_FreeDebugMemory( ptr, fileName, lineNumber, true );
}

/*
==================
Mem_ClearedAlloc
==================
*/
void *Mem_ClearedAlloc( const int size, const char *fileName, const int lineNumber ) {
	void *mem = Mem_Alloc( size, fileName, lineNumber );
	SIMDProcessor->Memset( mem, 0, size );
	return mem;
}

/*
==================
Mem_CopyString
==================
*/
char *Mem_CopyString( const char *in, const char *fileName, const int lineNumber ) {
	char	*out;
	
	out = (char *)Mem_Alloc( strlen(in) + 1, fileName, lineNumber );
	strcpy( out, in );
	return out;
}

/*
==================
Mem_Init
==================
*/
void Mem_Init( void ) {
	mem_heap = new idHeap;
}

/*
==================
Mem_Shutdown
==================
*/
void Mem_Shutdown( void ) {

	if ( mem_leakName[0] != '\0' ) {
		Mem_DumpCompressed( va( "%s_leak_size.txt", mem_leakName ), MEMSORT_SIZE, 0, 0 );
		Mem_DumpCompressed( va( "%s_leak_location.txt", mem_leakName ), MEMSORT_LOCATION, 0, 0 );
		Mem_DumpCompressed( va( "%s_leak_cs1.txt", mem_leakName ), MEMSORT_CALLSTACK, 2, 0 );
	}

	idHeap *m = mem_heap;
	mem_heap = NULL;
	delete m;
}

/*
==================
Mem_EnableLeakTest
==================
*/
void Mem_EnableLeakTest( const char *name ) {
	idStr::Copynz( mem_leakName, name, sizeof( mem_leakName ) );
}

#endif /* !ID_DEBUG_MEMORY */
