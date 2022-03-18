##################################
##	Beato idTech4 Game Engine	##
##	Framework: Engine Core		##
##################################

include_directories(
	${OPENGL_INCLUDE_DIR}
	${JPEG_INCLUDE_DIR}
					)

##
## Renderer
## Core renderer engine 
SET( RENDERER_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Cinematic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MegaTexture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem_init.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VertexCache.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/Cinematic.h
	${CMAKE_CURRENT_SOURCE_DIR}/Material.h
	${CMAKE_CURRENT_SOURCE_DIR}/MegaTexture.h
	${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem.h
	${CMAKE_CURRENT_SOURCE_DIR}/simplex.h
	${CMAKE_CURRENT_SOURCE_DIR}/VertexCache.h
	)
	
source_group("" FILES ${RENDERER_SOURCES} )

	
##
## Backend
## 
SET( RENDERER_BAKCEND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/cg_explicit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/draw_arb.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/draw_arb2.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/draw_common.cpp
#	${CMAKE_CURRENT_SOURCE_DIR}/draw_exp.cpp		#Mark as remove
#	${CMAKE_CURRENT_SOURCE_DIR}/draw_exp_stub.cpp	#Mark as remove
	${CMAKE_CURRENT_SOURCE_DIR}/draw_nv10.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/draw_nv20.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/draw_r200.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_backend.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/cg_explicit.h
	)

source_group("Backend" FILES ${RENDERER_BAKCEND_SOURCES} )

##
## Frontend
## 
SET( RENDERER_FRONTEND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Interaction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_deform.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_font.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_font.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_guisurf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_light.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_lightrun.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_orderIndexes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_polytope.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_render.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_rendertools.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_shadowbounds.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_stencilshadow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_subview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_trace.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_trisurf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_turboshadow.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/Interaction.h
	${CMAKE_CURRENT_SOURCE_DIR}/tr_local.h
	)
	
source_group("Frontend" FILES ${RENDERER_FRONTEND_SOURCES} )

##
## Imagens
## 
SET( RENDERER_IMAGENS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Image_files.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Image_init.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Image_load.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Image_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Image_program.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/Image.h
	)
	
source_group("Imagens" FILES ${RENDERER_IMAGENS_SOURCES} )


##
## Models
## 
SET( RENDERER_MODELS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/GuiModel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_ase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_beam.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_liquid.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_lwo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_ma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_md3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_md5.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_prt.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model_sprite.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ModelDecal.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ModelManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ModelOverlay.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/GuiModel.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model_ase.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model_lwo.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model_ma.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model_md3.h
	${CMAKE_CURRENT_SOURCE_DIR}/ModelDecal.h
	${CMAKE_CURRENT_SOURCE_DIR}/ModelManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/ModelOverlay.h
	)
	
source_group("Models" FILES ${RENDERER_MODELS_SOURCES} )

##
## OpenGL
## 
SET( RENDERER_OPENGL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/qgl.h
	${CMAKE_CURRENT_SOURCE_DIR}/qgl_linked.h
	)
	
source_group("OpenGL" FILES ${RENDERER_OPENGL_SOURCES} )

##
## Render World
## 
SET( RENDERER_RENDERWORL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/RenderEntity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderWorld.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderWorld_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderWorld_load.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderWorld_portals.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/ModelOverlay.h
	${CMAKE_CURRENT_SOURCE_DIR}/RenderWorld.h
	${CMAKE_CURRENT_SOURCE_DIR}/RenderWorld_local.h
	)
	
source_group("RenderWorld" FILES ${RENDERER_RENDERWORL_SOURCES} )

SET( RENDERER_SOURCES_ALL 
	${RENDERER_SOURCES}
	${RENDERER_BAKCEND_SOURCES}
	${RENDERER_FRONTEND_SOURCES}
	${RENDERER_IMAGENS_SOURCES}
	${RENDERER_MODELS_SOURCES}
	${RENDERER_OPENGL_SOURCES}
	${RENDERER_RENDERWORL_SOURCES}
	)
	
add_library( renderer STATIC ${RENDERER_SOURCES_ALL} )
add_dependencies( renderer idlib )
target_precompile_headers( renderer PUBLIC ${CMAKE_SOURCE_DIR}/idlib/precompiled.h PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/tr_local.h>" )
