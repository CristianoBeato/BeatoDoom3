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
#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"

// BEATO Begin:
#include "Backend_common.h"

crBackend::crBackend( void )
{
}

crBackend::~crBackend( void )
{
}
// BEATO End

/*
======================
RB_SetDefaultGLState

This should initialize all GL state that any part of the entire program
may touch, including the editor.
======================
*/
void RB_SetDefaultGLState( void ) 
{
	int		i;

	RB_LogComment( "--- R_SetDefaultGLState ---\n" );

#if 0
	glClearDepth( 1.0f );

	//
	// make sure our GL state vector is set correctly
	//
	memset( &backEnd.glState, 0, sizeof( backEnd.glState ) );
	backEnd.glState.forceGlState = true;

	glColorMask( 1, 1, 1, 1 );

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glEnable( GL_SCISSOR_TEST );
	glEnable( GL_CULL_FACE );
	glDisable( GL_LINE_STIPPLE );
	glDisable( GL_STENCIL_TEST );

	glDepthMask( GL_TRUE );
	glDepthFunc( GL_ALWAYS );
	
	glCullFace( GL_FRONT_AND_BACK );
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
//	glShadeModel( GL_SMOOTH ); // FIXED FUNCTION PIPELINE

	if ( r_useScissor.GetBool() ) 
	{
		glScissor( 0, 0, glConfig.vidWidth, glConfig.vidHeight );
	}

	for ( i = glConfig.maxTextureUnits - 1 ; i >= 0 ; i-- ) 
	{
		GL_SelectTexture( i );

		// object linear texgen is our default
		glTexGenf( GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );
		glTexGenf( GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR );

		GL_TexEnv( GL_MODULATE );
		glDisable( GL_TEXTURE_2D );
		if ( glConfig.texture3DAvailable ) 
		{
			glDisable( GL_TEXTURE_3D );
		}
		
		if ( glConfig.cubeMapAvailable ) 
		{
			glDisable( GL_TEXTURE_CUBE_MAP );
		}
	}
#endif
}


/*
====================
RB_LogComment
====================
*/
void RB_LogComment( const char *comment, ... ) 
{
#if 0 //todo: input for std::stream 
   va_list marker;

	if ( !tr.logFile ) 
	{
		return;
	}

	fprintf( tr.logFile, "// " );
	va_start( marker, comment );
	vfprintf( tr.logFile, comment, marker );
	va_end( marker );
#endif
}

//=============================================================================

/*
=============
crBackend::SetBuffer
=============
*/
void crBackend::SetBuffer( const void *data ) 
{ 
	VkResult result = VK_SUCCESS;
    
	// see which draw buffer we want to render the frame to
	const setBufferCommand_t	*cmd = static_cast<const setBufferCommand_t*>( data );

	frameCount = cmd->frameCount;
	frameID = frameCount % SMP_FRAMES; 

	/// BEATO Begin:

	// get the current output image
	m_swapChain->AcquireImage();

	// Wait for previous render frame to finish ( replaced the fence )
    VkSemaphoreWaitInfo waitRenderFinis{};
    waitRenderFinis.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
    waitRenderFinis.semaphoreCount = 1,
    waitRenderFinis.pSemaphores = &m_renderFinished;
    waitRenderFinis.pValues = &frameCount;
    result = vkWaitSemaphores( m_renderDevice->Device(), &waitRenderFinis, UINT64_MAX );
	if ( result != VK_SUCCESS && result == VK_TIMEOUT ) 
	{
		// TODO:
	}

	// reset last command buffer state
	result = vkResetCommandBuffer( m_commandBuffers[frameID], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT );
    if( result != VK_SUCCESS )
	{
		common->Warning( "crBackend::SetBuffer::vkResetCommandBuffer: failed" );
	}
    
	// begin register frame execution commands
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vkBeginCommandBuffer( m_commandBuffers[frameID], &beginInfo );
	if( result != VK_SUCCESS )
	{
		common->Warning( "crBackend::SetBuffer::vkBeginCommandBuffer: failed" );
	}

	// clear screen for debugging
	// automatically enable this with several other debug tools
	// that might leave unrendered portions of the screen
	VkClearColorValue clearColor{};
	if ( r_clear.GetFloat() || idStr::Length( r_clear.GetString() ) != 1 || r_lockSurfaces.GetBool() || r_singleArea.GetBool() || r_showOverDraw.GetBool() ) 
	{
		float c[3];
		if ( sscanf( r_clear.GetString(), "%f %f %f", &c[0], &c[1], &c[2] ) == 3 )
		{
			clearColor.float32[0] = c[0];
			clearColor.float32[1] = c[1];
			clearColor.float32[2] = c[2];
			clearColor.float32[3] = 1.0f;
		} 
		else if ( r_clear.GetInteger() == 2 ) 
		{
			clearColor.float32[0] = 0.0f;
			clearColor.float32[1] = 0.0f;
			clearColor.float32[2] = 0.0f;
			clearColor.float32[3] = 1.0f;
		}
		else if ( r_showOverDraw.GetBool() )
		{
			clearColor.float32[0] = 1.0f;
			clearColor.float32[1] = 1.0f;
			clearColor.float32[2] = 1.0f;
			clearColor.float32[3] = 1.0f;			
		} 
		else
		{
			clearColor.float32[0] = 0.4f;
			clearColor.float32[1] = 1.0f;
			clearColor.float32[2] = 0.25f;
			clearColor.float32[3] = 1.0f;			
		}
	}

	// clear color attachament
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_swapChain->CurrentImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = clearColor;

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = { {0, 0}, m_swapChain->Extent() };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    vkCmdBeginRendering( m_commandBuffers[frameID], &renderingInfo );
}

#if 0
/*
===============
RB_ShowImages

Draw all the images to the screen, on top of whatever
was there.  This is used to test for texture thrashing.
===============
*/
static void RB_ShowImages( void )
 {
	int		i;
	idImage	*image;
	float	x, y, w, h;
	int		start, end;

	RB_SetGL2D();

	//glClearColor( 0.2, 0.2, 0.2, 1 );
	//glClear( GL_COLOR_BUFFER_BIT );

	glFinish();

	start = Sys_Milliseconds();

	for ( i = 0 ; i < globalImages->images.Num() ; i++ ) {
		image = globalImages->images[i];

		if ( image->texnum == idImage::TEXTURE_NOT_LOADED && image->partialImage == NULL ) {
			continue;
		}

		w = glConfig.vidWidth / 20;
		h = glConfig.vidHeight / 15;
		x = i % 20 * w;
		y = i / 20 * h;

		// show in proportional size in mode 2
		if ( r_showImages.GetInteger() == 2 ) {
			w *= image->uploadWidth / 512.0f;
			h *= image->uploadHeight / 512.0f;
		}

		image->Bind();
		glBegin (GL_QUADS);
		glTexCoord2f( 0, 0 );
		glVertex2f( x, y );
		glTexCoord2f( 1, 0 );
		glVertex2f( x + w, y );
		glTexCoord2f( 1, 1 );
		glVertex2f( x + w, y + h );
		glTexCoord2f( 0, 1 );
		glVertex2f( x, y + h );
		glEnd();
	}

	glFinish();

	end = Sys_Milliseconds();
	common->Printf( "%i msec to draw all images\n", end - start );
}
#endif

/*
=============
crBackend::SwapBuffers
=============
*/
void crBackend::SwapBuffers( const void *data ) 
{
	VkResult result = VK_SUCCESS;
	crvkDeviceQueue* graphicQueue = m_renderDevice->GetQueue( CRVK_DEVICE_QUEUE_GRAPHICS );

#if 0 // TODO:
	// texture swapping test
	if ( r_showImages.GetInteger() != 0 ) 
		RB_ShowImages();
#endif 

	/// End rendenring to swapchain image
	vkCmdEndRendering( m_commandBuffers[frameID] );

	/// finish command buffer recording
	vkEndCommandBuffer( m_commandBuffers[frameID] );

	/// submit current frame command buffer 
	VkCommandBufferSubmitInfo commandBufferSubmit{};
	commandBufferSubmit.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	commandBufferSubmit.pNext = nullptr;
	commandBufferSubmit.commandBuffer = m_commandBuffers[frameID];
	commandBufferSubmit.deviceMask = 0;

	/// wait for current swap chain image be available 
	VkSemaphoreSubmitInfo wait{};
	wait.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	wait.pNext = nullptr;
	wait.semaphore = m_swapChain->CurrentSemaphore();
	wait.value = 0;
    wait.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	wait.deviceIndex = 0;

	/// signal that the current command buffer is done
	VkSemaphoreSubmitInfo signal{};
	signal.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	signal.pNext = nullptr;
	signal.semaphore = m_renderFinished;
	signal.value = frameCount + SMP_FRAMES; // frame N + 3 ( singal that our buffer are ready again at next 3 framess )
	signal.stageMask = /* VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT */ VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
	signal.deviceIndex = 0;

	// submit command buffer 
	graphicQueue->Submit( &wait, 1, &commandBufferSubmit, 1, &signal, 1, nullptr );

	// force a sync if requested
	if ( r_finish.GetBool() )
		graphicQueue->WaitIdle();
		
	// present to window the current image
	m_swapChain->PresentImage( nullptr, 0 );

    RB_LogComment( "***************** RB_SwapBuffers *****************\n\n\n" );

	// don't flip if drawing to front buffer
	// if ( !r_frontBuffer.GetBool() )
	// 	m_swapChain->SwapBuffers(); 
}

/*
=============
RB_CopyRender

Copy part of the current framebuffer to an image
=============
*/
void crBackend::CopyRender( const void *data ) 
{
	const copyRenderCommand_t	*cmd = static_cast<const copyRenderCommand_t *>( data );

	if ( r_skipCopyTexture.GetBool() )
		return;

    RB_LogComment( "***************** RB_CopyRender *****************\n" );

#if 0
#if 0
	m_currentFrameBuffer->CopyToImage( &cmd->image->GetTextureHandler(), cmd->x, cmd->y, cmd->imageWidth, cmd->imageHeight, 0 );
#else
	VkImage srcImage = nullptr; //TODO:

	// source image state
	VkImageSubresourceRange subresourceRangeSrc{};
	subresourceRangeSrc.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRangeSrc.baseMipLevel = 0;
	subresourceRangeSrc.levelCount = 1;
	subresourceRangeSrc.baseArrayLayer = 0;
	subresourceRangeSrc.layerCount = 1;
	
	VkImageMemoryBarrier2 barrierSrc{};
	barrierSrc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrierSrc.pNext = nullptr;
	barrierSrc.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	barrierSrc.srcAccessMask = VK_ACCESS_2_MEMORY_READ_BIT;
	barrierSrc.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	barrierSrc.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
	barrierSrc.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED; 
	barrierSrc.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	barrierSrc.srcQueueFamilyIndex = 0;
	barrierSrc.dstQueueFamilyIndex = 0;
	barrierSrc.image = srcImage;
	barrierSrc.subresourceRange = subresourceRangeSrc;

	// destine image 
	VkImageSubresourceRange subresourceRangeDst{};
	subresourceRangeSrc.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresourceRangeSrc.baseMipLevel = 0;
	subresourceRangeSrc.levelCount = 1;
	subresourceRangeSrc.baseArrayLayer = 0;
	subresourceRangeSrc.layerCount = 1;

	VkImageMemoryBarrier2 barrierDst{};
	barrierSrc.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrierDst.pNext = nullptr;
	barrierDst.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	barrierDst.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	barrierDst.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	barrierDst.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
	barrierDst.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrierDst.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrierDst.srcQueueFamilyIndex = 0;
	barrierDst.dstQueueFamilyIndex = 0;
	barrierDst.image = ;
	barrierDst.subresourceRange = ;


	// change the image state
	VkDependencyInfo barrierDependency{};
	barrierDependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	barrierDependency.pNext = nullptr;
	barrierDependency.dependencyFlags = ;
	barrierDependency.memoryBarrierCount = ;
	barrierDependency.pMemoryBarriers = ;
	barrierDependency.bufferMemoryBarrierCount = ;
	barrierDependency.pBufferMemoryBarriers = ;
	barrierDependency.imageMemoryBarrierCount = ;
	barrierDependency.pImageMemoryBarriers = ;

	vkCmdPipelineBarrier2( m_commandBuffers[frameID], &barrierDependency );

	VkCopyImageInfo2 copyImageInfo{};
	copyImageInfo.sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2;
	copyImageInfo.pNext = nullptr;
	copyImageInfo.srcImage ;
	copyImageInfo.srcImageLayout ;
	copyImageInfo.dstImage ;
	copyImageInfo.dstImageLayout ;
	copyImageInfo.regionCount ;
	copyImageInfo.pRegions ;
	
	auto dstImage = cmd->image->GetTextureHandler();
	vkCmdCopyImage2( m_commandBuffers[frameID], &copyImageInfo );
#endif
#endif 
	c_copyFrameBuffer++;
}

// BEATO Begin: Set on material
#if 0
void crBackend::SetCull( const cullType_t culling )
{
	VkCullModeFlags cullMode; 
	switch ( culling )
	{
		case CT_FRONT_SIDED:
			cullMode = VK_CULL_MODE_FRONT_BIT;
			break;
		case CT_BACK_SIDED:
			cullMode = VK_CULL_MODE_BACK_BIT;
			break;
		case CT_TWO_SIDED:
			cullMode = VK_CULL_MODE_FRONT_AND_BACK;
			break;	
	default: // TODO: defalt state 
		break;
	}

	// change the face culling 
	vkCmdSetCullMode( m_commandBuffers[frameID], cullMode );
}
#endif 
// BEATO End

/*
====================
ZeroPerformanceCounters
====================
*/
void crBackend::ZeroPerformanceCounters( void )
{
	pc = backEndCounters_t();
}

/*
====================
ExecuteBackEndCommands

This function will be called syncronously if running without
smp extensions, or asyncronously by another thread.
====================
*/
void crBackend::ExecuteBackEndCommands( const emptyCommand_t *cmds ) 
{
	static int backEndStartTime = 0; 
	static int backEndFinishTime = 0;

	// r_debugRenderToTexture
	int	c_draw3d = 0, c_draw2d = 0, c_setBuffers = 0, c_swapBuffers = 0, c_copyRenders = 0;

	if ( cmds->commandId == RC_NOP && !cmds->next ) 
		return;

	backEndStartTime = Sys_Milliseconds();

	// needed for editor rendering
	//Pipeline( PIPE_DEFAULT );

	// upload any image loads that have completed
	globalImages->CompleteBackgroundImageLoads();

	for ( ; cmds ; cmds = (const emptyCommand_t *)cmds->next ) 
	{
		switch ( cmds->commandId ) 
		{
		case RC_NOP:
			break;
		case RC_DRAW_VIEW:
			DrawView( cmds );
			if ( ((const drawSurfsCommand_t *)cmds)->viewDef->viewEntitys ) 
				c_draw3d++;
			else 
				c_draw2d++;
			break;
		case RC_SET_BUFFER:
			SetBuffer( cmds );
			c_setBuffers++;
			break;
		case RC_SWAP_BUFFERS:
			SwapBuffers( cmds );
			c_swapBuffers++;
			break;
		case RC_COPY_RENDER:
			CopyRender( cmds );
			c_copyRenders++;
			break;
		default:
			common->Error( "RB_ExecuteBackEndCommands: bad commandId" );
			break;
		}
	}

	// stop rendering on this thread
	backEndFinishTime = Sys_Milliseconds();
	pc.msec = backEndFinishTime - backEndStartTime;

	if ( r_debugRenderToTexture.GetInteger() == 1 ) 
	{
		common->Printf( "3d: %i, 2d: %i, SetBuf: %i, SwpBuf: %i, CpyRenders: %i, CpyFrameBuf: %i\n", c_draw3d, c_draw2d, c_setBuffers, c_swapBuffers, c_copyRenders, c_copyFrameBuffer );
		c_copyFrameBuffer = 0;
	}
}

void crBackend::Framebuffer( const uint32_t framebufferID )
{
	// DO nothin ( right now )
}

void crBackend::Viewport( const int x, const int y, const int width, const int height )
{
	VkViewport viewport{};
	viewport.x = x;
	viewport.y = y;
	viewport.width = width;
	viewport.height = height;
	viewport.minDepth = -1.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport( m_commandBuffers[frameID], 0, 1, &viewport );
}

void crBackend::Scissor( const idScreenRect &in_scissor )
{
	if( currentScissor.Equals( in_scissor ) )
		return;

	currentScissor = in_scissor;

#if CR_USE_CLIP_AS_SCISSOR
	float scissor[4] = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
	m_uniforms->SetUniform( scissor, VERTEX_UNIFORM_LOCATION_CLIP_BOUDS );
#else
	VkRect2D scissor{};
	scissor.offset.x = viewDef->viewport.x1 + currentScissor.x1;
	scissor.offset.y = viewDef->viewport.y1 + currentScissor.y1;
	scissor.extent.width = currentScissor.x2 + 1 - currentScissor.x1;
	scissor.extent.height = currentScissor.y2 + 1 - currentScissor.y1;
	vkCmdSetScissor( m_commandBuffers[frameID], 0, 1, &scissor );
#endif
}
