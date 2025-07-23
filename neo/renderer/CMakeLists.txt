##################################
##	Beato idTech4 Game Engine	##
##	Framework: Engine Core		##
##################################

##
## Renderer
## Core renderer engine 
SET( RENDERER_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Cinematic.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Cinematic.h
	${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Material.h
	${CMAKE_CURRENT_SOURCE_DIR}/RenderMatrix.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderMatrix.h
	${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem_init.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/RenderSystem.h
	${CMAKE_CURRENT_SOURCE_DIR}/VertexCache.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VertexCache.h
	${CMAKE_CURRENT_SOURCE_DIR}/simplex.h
	)
	
source_group("" FILES ${RENDERER_SOURCES} )

#
# Backend OpenGL
#
SET ( RENDERER_BACKEND_OPENGL_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glBuffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glCommandQueue.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glCommandQueue.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glContext.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glContext.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glFence.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glFence.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glFramebuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glFramebuffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glPipeline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glPipeline.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glShaderStorage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glShaderStorage.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glTexture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/glTexture.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/qgl.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/opengl/qgl.h
	)

#
# Backend Vulkan
#	
SET ( RENDERER_BACKEND_VULKAN_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkBuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkBuffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkCommandQueue.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkCommandQueue.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkContext.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkContext.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkFence.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkFence.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkFramebuffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkFramebuffer.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkPipeline.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkPipeline.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkShaderStorage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkShaderStorage.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkSwapChain.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkSwapChain.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkTexture.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/vkTexture.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/qvk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/vulkan/qvk.h
	)
##
## Backend
## 
SET( RENDERER_BACKEND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend_apiwrapper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend_apiwrapper.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend_common.h
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend_draw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend_draw_interactions.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/backend/Backend_render.cpp
	)

source_group("Backend" FILES ${RENDERER_BAKCEND_SOURCES} )

##
## Frontend
## 
SET( RENDERER_FRONTEND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend.h
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_trisurf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_deform.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_guisurf.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_light.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_lightrun.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_stencilshadow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Frontend_subview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Transform.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/frontend/Transform.h

	${CMAKE_CURRENT_SOURCE_DIR}/Interaction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Interaction.h
	${CMAKE_CURRENT_SOURCE_DIR}/tr_font.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_font.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_orderIndexes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_polytope.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_rendertools.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_shadowbounds.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_trace.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tr_turboshadow.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderer_common.h
	)
	
source_group("Frontend" FILES ${RENDERER_FRONTEND_SOURCES} )

##
## Imagens
## 
SET( RENDERER_IMAGENS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/images/Image_files.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/images/Image_init.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/images/Image_load.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/images/Image_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/images/Image_program.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/images/Image.h
	)
	
source_group("Imagens" FILES ${RENDERER_IMAGENS_SOURCES} )

##
## Models
## 
SET( RENDERER_MODELS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/models/GuiModel.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/GuiModel.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_ase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_ase.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_beam.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_liquid.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_lwo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_lwo.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_ma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_ma.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_md3.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_md3.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_md5.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_prt.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_sprite.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/ModelDecal.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/ModelDecal.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/ModelManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/ModelManager.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/ModelOverlay.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/models/ModelOverlay.h
	${CMAKE_CURRENT_SOURCE_DIR}/models/Model_local.h
	)
	
source_group("Models" FILES ${RENDERER_MODELS_SOURCES} )

##
## Render World
## 
SET( RENDERER_RENDERWORL_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderEntity.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderWorld.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderWorld.h
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderWorld_demo.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderWorld_load.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderWorld_local.h
	${CMAKE_CURRENT_SOURCE_DIR}/renderworld/RenderWorld_portals.cpp

	)
	
source_group("RenderWorld" FILES ${RENDERER_RENDERWORL_SOURCES} )

set( RENDERER_SOURCES_ALL
	${RENDERER_SOURCES}
	${RENDERER_BAKCEND_SOURCES}
	${RENDERER_FRONTEND_SOURCES}
	${RENDERER_IMAGENS_SOURCES}
	${RENDERER_MODELS_SOURCES}
	${RENDERER_RENDERWORL_SOURCES}
	)

add_library( renderer STATIC ${RENDERER_SOURCES_ALL} )
add_dependencies( renderer idlib )
target_precompile_headers( renderer PUBLIC ${CMAKE_SOURCE_DIR}/idlib/precompiled.h PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/renderer_common.h>" )
