#ifndef __INCLUDED_USELESS_WGL_SURFACE_H__
#define __INCLUDED_USELESS_WGL_SURFACE_H__

#include "Useless/Graphic/Device/GL/GLContextSurface.h"

namespace Useless {

class WGLSurface : public GLContextSurface
{
public:
    WGLSurface()
    {}
    
    WGLSurface( Surf::Properties &properties )
        : GLContextSurface( properties )
    {}

    virtual ~WGLSurface()
    {
        if ( _devContext )
        {
            GLFinishGarbage();
            ::ReleaseDC( _window, _devContext );
        }
    }
    
    virtual bool MakeCurrent()
    {
        if ( _glContext != wglGetCurrentContext() ||
                _devContext != wglGetCurrentDC() )
        {
            wglMakeCurrent( _devContext, _glContext );
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual GLContextSurface *CreateContext( Surf::Properties &properties ) const
    {
        return (new WGLSurface( properties ));
    }

    HDC     GetHDC()    const { return _devContext; }
    HGLRC   GetHGLRC()  const { return _glContext; }
    HWND    GetHWND()   const { return _window; }

private:
    HGLRC   _glContext;
    HDC     _devContext;
    HWND    _window;
    friend class WGLScreen;
};


};//namespace Useless
#endif//__INCLUDED_USELESS_WGL_SURFACE_H__
