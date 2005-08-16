#include "UselessPch.h"

#pragma warning (disable:4786)
//------------------------------
#include "Texture.h"
#include "Useless/Graphic/Linear.h"
#include "GLResourcePool.h"
#include "PixelBuffer.h"

namespace Useless {

GLint Texture::_Min_filter=GL_LINEAR;
GLint Texture::_Max_filter=GL_LINEAR;
GLint Texture::_Wrap_s=GL_REPEAT;
GLint Texture::_Wrap_t=GL_REPEAT;

Texture::Texture( int w, int h )
    :_w(w), _h(h), _bind_id(-1)
{
    _bind_id=GLResourcePool::Instance().AcquireTexture();
}

Texture::Texture( int w, int h, int row_length, int fmt, void *data, int level, int priority)
    :_w(w), _h(h), _bind_id(-1)
{
    _bind_id=GLResourcePool::Instance().AcquireTexture();
    LoadPixels( w, h, row_length, fmt, data, level, priority );
}

Texture::~Texture()
{
    if ( Unique())
    {
        GLResourcePool::Instance().UnacquireTexture( _bind_id);
    }
}

void Texture::LoadPixels( int w, int h, int row_length, int fmt, void *data, int level, int priority)
{
    glBindTexture( GL_TEXTURE_2D, _bind_id);
  
    glPixelStorei( GL_UNPACK_ROW_LENGTH, row_length);

    GLenum format, tf;
    switch ( fmt)
    {
        case ImageFormat::B8G8R8:   format=GL_BGR_EXT;  tf=GL_RGB8; break;
        case ImageFormat::R8G8B8:   format=GL_RGB;      tf=GL_RGB8; break;
        case ImageFormat::B8G8R8A8: format=GL_BGRA_EXT; tf=GL_RGBA8; break;
        case ImageFormat::R8G8B8A8: format=GL_RGBA;     tf=GL_RGBA8; break;
        case ImageFormat::SINGLE8:  format=GL_LUMINANCE;tf=GL_LUMINANCE; break;
        default: assert(!"Texture: unknown GL format!");
    };

    glTexImage2D( GL_TEXTURE_2D, level, tf, _w, _h, 0, //< target texture format
                  format, GL_UNSIGNED_BYTE, data); //< format of source data

    GLenum err= glGetError();
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, priority);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _Min_filter);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _Max_filter);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _Wrap_s);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _Wrap_t);
}

void Texture::LoadSubPixels( int x, int y, int w, int h, int row_length, int fmt, void *data, int level, int priority )
{
    glBindTexture( GL_TEXTURE_2D, _bind_id);
  
    glPixelStorei( GL_UNPACK_ROW_LENGTH, row_length);

    GLenum format, tf;
    switch ( fmt)
    {
        case ImageFormat::B8G8R8:   format=GL_BGR_EXT;  tf=GL_RGB8; break;
        case ImageFormat::R8G8B8:   format=GL_RGB;      tf=GL_RGB8; break;
        case ImageFormat::B8G8R8A8: format=GL_BGRA_EXT; tf=GL_RGBA8; break;
        case ImageFormat::R8G8B8A8: format=GL_RGBA;     tf=GL_RGBA8; break;
        case ImageFormat::SINGLE8:  format=GL_LUMINANCE;tf=GL_LUMINANCE; break;
        default: assert(!"Texture: unknown GL format!");
    };

    glTexSubImage2D( GL_TEXTURE_2D, level, x, y, w, h, format, GL_UNSIGNED_BYTE, data );

    GLenum err= glGetError();
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, priority);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _Min_filter);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _Max_filter);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _Wrap_s);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _Wrap_t);
}

}; //namespace Useless
