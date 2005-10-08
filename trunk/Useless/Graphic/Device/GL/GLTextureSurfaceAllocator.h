#ifndef __INCLUDED_GL_TEXTURE_SURFACE_ALLOCATOR_H__
#define __INCLUDED_GL_TEXTURE_SURFACE_ALLOCATOR_H__

#include "Useless/Graphic/Device/GL/GLTextureSurface.h"
#include <list>
#include <set>
#include <map>

namespace Useless {

    struct GLGarbage
    {
        int                         _age;
        const GLTextureSurface     *_surf;
        SPointer< ImageTesselator > _tess;
    };

    typedef std::map< const GLTextureSurface *, GLGarbage * > GLSurfTessMap;
    typedef std::list< GLGarbage > GLGarbageCollection;

    struct GLTextureSurfaceAllocator
    {
        GLSurfTessMap       _glSurfTess;
        GLGarbageCollection _glGarbage;

        GLTextureSurfaceAllocator();
        ~GLTextureSurfaceAllocator();

        ImageTesselator & GetTesselator     ( const GLTextureSurface *s );
        void              ReleaseTesselator ( const GLTextureSurface *s, ImageTesselator & tess );

        void GLFreeGarbage();
        void GLFreeGarbage( GLTextureSurface *s );
        void GLFinishGarbage();

        virtual bool MakeCurrent() = 0; 

        static std::set< GLTextureSurfaceAllocator * > s_allocators;
    };

    void GLFreeSurfaceGarbage( GLTextureSurface *s );
    void GLUpdateMovieSamples( GLTextureSurfaceAllocator *pTexAlloc );


};//Useless
#endif//__INCLUDED_GL_TEXTURE_SURFACE_ALLOCATOR_H__
