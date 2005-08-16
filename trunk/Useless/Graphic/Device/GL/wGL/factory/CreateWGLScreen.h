#include "Useless/Graphic/Device/CreateScreen.h"

namespace Useless {
    
    enum GDIScreenDriverId
    {
        WINDOWS_OPEN_GL_SCREEN=2
    };
    
    extern Screen*  CreateWGLScreen( const char *title);
    extern Surface* CreateWGLSurface();
    extern Surface* CreateWGLSurfaceProp( const Surf::Properties &);

    static ScreenDriverRegistrator __WGL_scr_drv_reg( WINDOWS_OPEN_GL_SCREEN,
            &CreateWGLScreen, &CreateWGLSurface, &CreateWGLSurfaceProp);
};
