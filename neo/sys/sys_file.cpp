
#include "idlib/precompiled.h"
#pragma hdrstop

#ifdef __linux__
#include <sys/stat.h>
#endif // __linux__

/*
=================
Sys_FileTimeStamp
=================
*/
ID_TIME_T Sys_FileTimeStamp( FILE *fp )
{
#if _WIN32 || _WIN64
	struct _stat st;
	_fstat( _fileno( fp ), &st );
	return (long)st.st_mtime;
#else
	struct stat st;
	fstat( fileno( fp ), &st );
	return st.st_mtime;
#endif
}