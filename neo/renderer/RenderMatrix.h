
#ifndef __RENDER_MATRIX_H__
#define __RENDER_MATRIX_H__

/// @brief a OpenGL/Vulkan Column-major matrix
class alignas( 16 ) crRenderMatrix
{
public:
    crRenderMatrix( void );
    ~crRenderMatrix( void );

    void        Identity( void );
    void        Zero( void );
    void        FromAxist( const idMat3 &axis, const idVec3 &origin );
    
//    idVec3      LocalPointToGlobal( const idVec3 &in );
//    idVec3      GlobalPointToLocal( const idVec3 &in );
    idVec3      LocalVectorToGlobal( const idVec3 &in ) const;
    idVec3      GlobalVectorToLocal( const idVec3 &in ) const;
    idPlane     GlobalPlaneToLocal( const idPlane &in ) const;
    idPlane     LocalPlaneToGlobal( const idPlane &in ) const;
    float       TransformEyeZToWin( const float srcz ) const;

    idVec4          operator* ( const idVec4 &in ) const;
    crRenderMatrix  operator* ( const crRenderMatrix &ref ) const;
    
    crRenderMatrix Transpose( void ) const;

    float &operator[]( int i ) { return static_cast<float*>( &mat[0][0] )[i]; }
    const float operator[]( int i ) const { return static_cast<const float*>( &mat[0][0] )[i]; }
    float* operator &( void ) { return &mat[0][0]; }
    const float* operator &( void ) const { return &mat[0][0]; }

private:
    float   mat[4][4];
};

#endif //__RENDER_MATRIX_H__