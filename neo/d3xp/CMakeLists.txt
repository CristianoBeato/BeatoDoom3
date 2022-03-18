##################################
##	Beato idTech4 Game Engine	##
##	Game: Game Logic Lib		##
##################################

add_definitions( -D__DOOM__ -DGAME_DLL )

##
## Basic Game Logic
##
SET( GAME_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Actor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AF.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AFEntity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BrittleFracture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Entity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Fx.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Game_local.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Game_network.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/GameEdit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IK.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Item.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Light.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Misc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Moveable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Mover.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MultiplayerGame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PlayerIcon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PlayerView.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Projectile.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Pvs.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SecurityCamera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SmokeParticles.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Sound.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Target.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Trigger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Weapon.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WorldSpawn.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/Actor.h
	${CMAKE_CURRENT_SOURCE_DIR}/AF.h
	${CMAKE_CURRENT_SOURCE_DIR}/AFEntity.h
	${CMAKE_CURRENT_SOURCE_DIR}/BrittleFracture.h
	${CMAKE_CURRENT_SOURCE_DIR}/Camera.h
	${CMAKE_CURRENT_SOURCE_DIR}/Entity.h
	${CMAKE_CURRENT_SOURCE_DIR}/Fx.h
	${CMAKE_CURRENT_SOURCE_DIR}/Game.h
	${CMAKE_CURRENT_SOURCE_DIR}/Game_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/GameEdit.h
	${CMAKE_CURRENT_SOURCE_DIR}/IK.h
	${CMAKE_CURRENT_SOURCE_DIR}/Item.h
	${CMAKE_CURRENT_SOURCE_DIR}/Light.h
	${CMAKE_CURRENT_SOURCE_DIR}/Misc.h
	${CMAKE_CURRENT_SOURCE_DIR}/Moveable.h
	${CMAKE_CURRENT_SOURCE_DIR}/Mover.h
	${CMAKE_CURRENT_SOURCE_DIR}/MultiplayerGame.h
	${CMAKE_CURRENT_SOURCE_DIR}/Player.h
	${CMAKE_CURRENT_SOURCE_DIR}/PlayerIcon.h
	${CMAKE_CURRENT_SOURCE_DIR}/PlayerView.h
	${CMAKE_CURRENT_SOURCE_DIR}/Projectile.h
	${CMAKE_CURRENT_SOURCE_DIR}/Pvs.h
	${CMAKE_CURRENT_SOURCE_DIR}/SecurityCamera.h
	${CMAKE_CURRENT_SOURCE_DIR}/SmokeParticles.h
	${CMAKE_CURRENT_SOURCE_DIR}/Sound.h
	${CMAKE_CURRENT_SOURCE_DIR}/Target.h
	${CMAKE_CURRENT_SOURCE_DIR}/Trigger.h
	${CMAKE_CURRENT_SOURCE_DIR}/Weapon.h
	${CMAKE_CURRENT_SOURCE_DIR}/WorldSpawn.h
	)

##
## AI 
##
SET( GAME_AI_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AAS.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AAS_debug.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AAS_pathing.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AAS_routing.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AI.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AI_events.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AI_pathing.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AI_Vagary.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/ai/AAS.h
	${CMAKE_CURRENT_SOURCE_DIR}/ai/AAS_local.h
	)

##
## Animation 
##
SET( GAME_ANIMATION_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/anim/Anim.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/anim/Anim_Blend.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/anim/Anim_Import.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/anim/Anim_Testmodel.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/anim/Anim.h
	${CMAKE_CURRENT_SOURCE_DIR}/anim/Anim_Testmodel.h
)

##
## GameSys 
##
SET( GAME_GAMESYS_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/Class.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/DebugGraph.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/Event.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/SaveGame.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/SysCmds.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/SysCvar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/TypeInfo.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/Class.h
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/DebugGraph.h
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/Event.h
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/NoGameTypeInfo.h
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/SaveGame.h
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/SysCmds.h
	${CMAKE_CURRENT_SOURCE_DIR}/gamesys/SysCvar.h
	)

##
## Physics 
##
SET( GAME_PHYSICS_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Clip.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Constant.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Drag.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Field.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Spring.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Actor.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_AF.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Monster.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Parametric.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Player.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_RigidBody.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Static.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_StaticMulti.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Push.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Clip.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Constant.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Drag.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Field.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Force_Spring.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Actor.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_AF.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Base.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Monster.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Parametric.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Player.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_RigidBody.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_Static.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Physics_StaticMulti.h
	${CMAKE_CURRENT_SOURCE_DIR}/physics/Push.h
	)

##
## Script 
##
SET( GAME_SCRIPT_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Compiler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Interpreter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Program.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Thread.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Compiler.h
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Interpreter.h
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Program.h
	${CMAKE_CURRENT_SOURCE_DIR}/script/Script_Thread.h
	)

source_group( "" FILES ${GAME_SOURCES} )
source_group( "AI" FILES ${GAME_AI_SOURCES} )
source_group( "Animation" FILES ${GAME_ANIMATION_SOURCES} )
source_group( "GameSys" FILES ${GAME_GAMESYS_SOURCES} )
source_group( "Physics" FILES ${GAME_PHYSICS_SOURCES} )
source_group( "Script" FILES ${GAME_SCRIPT_SOURCES} )

SET ( GAME_SOURCES_ALL
	${GAME_SOURCES} 
	${GAME_AI_SOURCES} 
	${GAME_ANIMATION_SOURCES} 
	${GAME_GAMESYS_SOURCES} 
	${GAME_PHYSICS_SOURCES} 
	${GAME_SCRIPT_SOURCES} 
	)

if(BUILD_MONOLITH)
	SET( GAME_LIBRARY_NAME "d3xp" )
	set(BUILD_MODE STATIC )
else(NOT MONOLITH)
	SET( GAME_LIBRARY_NAME "d3xp${BUILD_PLATFORM_POSTFIX}" )
	set(BUILD_MODE SHARED )
endif(BUILD_MONOLITH)

##
## game DLL resource 
##
if(WIN32 AND NOT BUILD_MONOLITH )
	list(APPEND GAME_SOURCES_ALL ${CMAKE_CURRENT_SOURCE_DIR}/Game.def)
endif(WIN32 AND NOT BUILD_MONOLITH )

add_library( ${GAME_LIBRARY_NAME} ${BUILD_MODE} ${GAME_SOURCES_ALL} )
add_dependencies(${GAME_LIBRARY_NAME} idlib )
target_precompile_headers( ${GAME_LIBRARY_NAME} 
	PUBLIC
		${CMAKE_SOURCE_DIR}/idlib/precompiled.h 
	PRIVATE
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Game.h>" 
		"$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/Game_local.h>" 
		)

if(NOT BUILD_MONOLITH)
	target_link_libraries( ${GAME_LIBRARY_NAME} idlib ${SDL_LIBRARIES} )
endif(NOT BUILD_MONOLITH)
