#ifndef __INCLUDED_GL_TEXTURE_H__
#define __INCLUDED_GL_TEXTURE_H__

#include "Useless/Graphic/Device/GL/GLResourcePool.h"
#include "Useless/Graphic/detail/ImageDesc.h"
#include "Useless/Util/ReferenceCounter.h"
#include <vector>

namespace Useless {

/*! Texture with size: 64x64, 128x128 or 256x256
 */
class Texture : public ReferenceCounter
{
public:
    Texture( int w, int h );
    Texture( int w, int h, int row_length, int fmt, void *data, int level=0, int priority=0 );
    ~Texture();

    void Use()
    {
        glBindTexture( GL_TEXTURE_2D, _bind_id);
    }
    
    void LoadPixels( int w, int h, int row_length, int fmt, void *data, int level=0, int priority=0 );
    void LoadSubPixels( int x, int y, int w, int h, int row_length, int fmt, void *data, int level=0, int priority=0 );

private:
    int               _w;       // 64, 128 or 256
    int               _h;
    GLint             _bind_id;

public:
    static GLint _Min_filter;
    static GLint _Max_filter;
    static GLint _Wrap_s;
    static GLint _Wrap_t;
};

}; //namespace Useless
#endif//__INCLUDED_GL_TEXTURE_H__
