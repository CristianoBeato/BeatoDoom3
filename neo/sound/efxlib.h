/*
*/

#ifndef __EFXLIBH
#define __EFXLIBH

#define EFXprintf(...) do { common->Printf(__VA_ARGS__); } while (false)

///////////////////////////////////////////////////////////
// Class definitions.
class idSoundEffect
{
public:
	idSoundEffect() {
	};
	~idSoundEffect() 
	{ 
		if ( data && datasize )
		{
			Mem_Free( data );
			data = NULL;
		}

		if (effectID)
			effectID = 0;
	}
	
	idStr name;
	int datasize;
	void *data;
	ALuint effectID;
};

class idEFXFile
{
private:

protected:
    // Protected data members.

public:
    // Public data members.

private:
    
public:
	idEFXFile();
	~idEFXFile();

	bool FindEffect( idStr &name, idSoundEffect **effect, int *index );
	bool ReadEffect( idLexer &lexer, idSoundEffect *effect );
	bool LoadFile( const char *filename, bool OSPath = false );
	void UnloadFile( void );
	void Clear( void );

	idList<idSoundEffect *>effects;
};
///////////////////////////////////////////////////////////




#endif // __EFXLIBH

