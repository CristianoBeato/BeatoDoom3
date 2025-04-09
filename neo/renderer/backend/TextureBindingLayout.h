
#ifndef __TEXTURE_BINDING_LAYOUT_H__
#define __TEXTURE_BINDING_LAYOUT_H__

class crTexture;
class crBuffer;
class crTextureBindingLayout
{
public:
    crTextureBindingLayout( void );
    ~crTextureBindingLayout( void );

    void        StartUp( void );
    void        ShutDown( void );

    int32_t     BindTexture( crAutoPointer<crTexture> texture );

    // free all textures from the buffer
    void        Purge( void );

private:
    int32_t                             m_head;
    int32_t                             m_tail;
    crAutoPointer<crBuffer>             m_buffer;
    idList<crAutoPointer<crTexture>>    m_textures;
};

#endif //!__TEXTURE_BINDING_BUFFER_H__
