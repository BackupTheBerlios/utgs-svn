#ifndef __INCLUDED_USELESS_CREATE_SCREEN_H__
#define __INCLUDED_USELESS_CREATE_SCREEN_H__

#include "Useless/Graphic/Device/Screen.h"
#include "Useless/Graphic/Device/Surface.h"

namespace Useless {

    enum ScreenDriverId
    {
        //DIRECT_DRAW_SCREEN=0,
        //RENDERWARE_SCREEN=1,
        //WINDOWS_GDI_SCREEN=2,
        USER_SCREEN_DRIVER=3,
        NUM_SCREEN_DRIVERS=64
    };

    extern int g_screen_driver;

    typedef Screen* (ScreenCtor)( const char*);
    typedef Surface* (SurfaceCtor)();
    typedef Surface* (SurfaceCtorProp)( const Surf::Properties &);
    
    extern ScreenCtor *g_CreateScreen[NUM_SCREEN_DRIVERS];
    extern SurfaceCtor *g_CreateSurface[NUM_SCREEN_DRIVERS];
    extern SurfaceCtorProp *g_CreateSurfaceProp[NUM_SCREEN_DRIVERS];
    
    extern Screen* CreateScreenByLib( const char *title);
    extern Surface* CreateSurfaceByLib();
    extern Surface* CreateSurfaceByLib( const Surf::Properties &);

    Screen* CreateScreen( const char *title);
    Surface* CreateSurface();
    Surface* CreateSurface( const Surf::Properties &);

    inline void RegisterScreenDriver( int driver_id,
            ScreenCtor *ptrCtor,
            SurfaceCtor *ptrStor,
            SurfaceCtorProp *ptrStorB)
    {
        g_CreateScreen[driver_id] = ptrCtor;
        g_CreateSurface[driver_id] = ptrStor;
        g_CreateSurfaceProp[driver_id] = ptrStorB;
    }

    inline void SetScreenDriver( int driver_id)
    {
        g_screen_driver = driver_id;
    }

    struct ScreenDriverRegistrator
    {
        ScreenDriverRegistrator( int driver_id,
                ScreenCtor *ptrCtor,
                SurfaceCtor *ptrStor,
                SurfaceCtorProp *ptrStorB)
        {
            RegisterScreenDriver( driver_id, ptrCtor, ptrStor, ptrStorB);
            SetScreenDriver( driver_id);
        }
    };

};//namespace Useless

#endif//__INCLUDED_USELESS_CREATE_SCREEN_H__
