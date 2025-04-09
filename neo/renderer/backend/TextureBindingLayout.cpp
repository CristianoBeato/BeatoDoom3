
#include "precompiled.h"
#pragma hdrstop

#include "renderer/renderer_common.h"
#include "TextureBindingLayout.h"

crTextureBindingLayout::crTextureBindingLayout( void ) :
    m_head( 0 ),
    m_tail( 0 )
{
}

crTextureBindingLayout::~crTextureBindingLayout( void )
{
}

int32_t crTextureBindingLayout::BindTexture( crAutoPointer<crTexture> texture )
{
    uint32_t textureIndex = 0;

    if ( texture->GetBindingIndex() != 0 )
        return texture->GetBindingIndex();
    
    

    return textureIndex;
}

void crTextureBindingLayout::Purge(void)
{
    for ( int i = 0; i < m_textures.Num(); i++)
    {
        m_textures[i]->SetBinding( -1 ); // release textures references 
        m_textures[i]->Unmakeresident();
    }
    
    m_textures.Resize( 0 );
}
