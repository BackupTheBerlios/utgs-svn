#include "UselessPch.h"

#include "Useless/Graphic/Device/winGDI/GDIScreen.h"

namespace Useless {

    Surface* CreateSurfaceByLib()
    {
        return new GDISurface();
    }

    Surface* CreateSurfaceByLib( const Surf::Properties &p )
    {
        return new GDISurface( p, GDIScreen::s_hwnd);
    }

};//namespace Useless
