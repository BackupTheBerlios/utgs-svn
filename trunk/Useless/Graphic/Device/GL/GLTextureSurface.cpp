#include "UselessPch.h"

#include "GLTextureSurface.h"
#include "Useless/Graphic/Device/SurfaceExtFun.h"
#include "Useless/Graphic/Device/GL/GLTextureSurfaceAllocator.h"
#include <string>

namespace Useless {

GLTextureSurface::GLTextureSurface()
{
    _colorKey = 0L;
    _useColorKey = false;
    _useAlpha = false;
    _dirty = false;
}
    
GLTextureSurface::GLTextureSurface( Surf::Properties &properties )
{
    _useColorKey = properties.has_color_key;
    _colorKey = properties.color_key;
    _useAlpha = properties.alpha;
    _dirty = false;
    
    _pixelBuffer.AllocatePixels(
            properties.width,
            properties.height,
            ( _useAlpha || _useColorKey ) ? ImageFormat::B8G8R8A8 : ImageFormat::B8G8R8, 
            1, 1, 2, 2 );

    properties = Surf::Properties();
    properties.width = _pixelBuffer.GetWidth();
    properties.height = _pixelBuffer.GetHeight();
    properties.pixelformat = ImageFormat( _pixelBuffer.GetFormat() );
    properties.has_color_key = _useColorKey;
    properties.color_key = _colorKey;
    properties.alpha = _useAlpha;

}

GLTextureSurface::~GLTextureSurface()
{
    GLFreeSurfaceGarbage( this );
}


Surface *
GLTextureSurface::CreateSurface( Surf::Properties &properties ) const
{
    GLTextureSurface *ptr = new GLTextureSurface( properties );
    return ptr;
}


struct GenericTransferAndNotify : GenericTransfer
{
    GLTextureSurface *_glSurface;

    GenericTransferAndNotify( GLTextureSurface *glSurface )
        : GenericTransfer( 0, glSurface->_pixelBuffer.GetData() )
        , _glSurface( glSurface )
        {}

    ~GenericTransferAndNotify()
    {
        if ( _glSurface->_useColorKey )
        {
            PixelBuffer & pixBuf = _glSurface->_pixelBuffer;
            char *pByte = (char*)pixBuf.GetData();

            NormalPixel colorKey( _glSurface->_colorKey );

            for ( int y=0, Y=pixBuf.GetHeight(); y < Y; ++y )
            {
                NormalRGB *pPix = (NormalRGB *)pByte;

                for ( int x=0, X=pixBuf.GetWidth(); x < X; ++x )
                {
                    if ( pPix->r == colorKey.c.r &&
                            pPix->g == colorKey.c.g &&
                            pPix->b == colorKey.c.b )
                    {
                        pPix->r = 0;
                        pPix->g = 0;
                        pPix->b = 0;
                        pPix->a = 0;
                    }
                    else
                    {
                        pPix->a = 255;
                    }
                    ++pPix;
                }
                pByte += pixBuf.GetPitch();
            }
        }

        _glSurface->_dirty = true;
    }
};


SPointer< PixelTransfer >
GLTextureSurface::CreateWriter( int channelFormat )
{
    PixelTransfer *ptr = new GenericTransferAndNotify( this );
    ptr->SetDestination(
            _pixelBuffer.GetWidth(),
            _pixelBuffer.GetHeight(),
            _pixelBuffer.GetPitch(),
            (channelFormat == ALPHA ? ImageFormat::ALPHA8 : _pixelBuffer.GetFormat())
            );
    return ptr;
}
 
bool GLTextureSurface::IsColorKeyed () const
{
    return _useColorKey;
}

bool GLTextureSurface::SetColorKey  ( unsigned long color )
{
    _useColorKey = true;
    _colorKey = color;
    return true;
}

void GLTextureSurface::SetNoColorKey()
{
    _useColorKey = false;
}

void GLTextureSurface::GetProperties( Surf::Properties *prop ) const
{
    prop->width = _pixelBuffer.GetWidth();
    prop->height = _pixelBuffer.GetHeight();
    prop->pixelformat = ImageFormat( _pixelBuffer.GetFormat() );
}


bool GLTextureSurface::operator == ( const Surface& s )
{
    assert(0);
    return (true);
}

bool GLTextureSurface::operator != ( const Surface& s )
{
    return !(this->operator==(s));
}

void * GLTextureSurface::GetExtFun( const std::string &name )
{
    if ( name == std::string("IsDirty") )
    {
        return (void *)(& SurfExt::Thunk1< int *, GLTextureSurface, &GLTextureSurface::IsDirty >::apply );
    }
}

}; //Useless
