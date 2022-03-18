##################################
##	Beato idTech4 Game Engine	##
##	Sound: Sound Engine 		##
##################################

##
## Sound
## Sound Engine structures
SET( SOUND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/snd_cache.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_decoder.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_efxfile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_emitter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_system.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_wavefile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/snd_world.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/efxlib.h
	${CMAKE_CURRENT_SOURCE_DIR}/snd_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/sound.h
	)
	
source_group("" FILES ${SOUND_SOURCES} )

include_directories( 
					${OPENAL_INCLUDE_DIR}
					${OGG_INCLUDE_DIR}
					${VORBIS_INCLUDE_DIR}
					)

add_library( sound STATIC ${SOUND_SOURCES} )
add_dependencies( sound idlib )
target_precompile_headers( sound PUBLIC ${CMAKE_SOURCE_DIR}/idlib/precompiled.h  )
