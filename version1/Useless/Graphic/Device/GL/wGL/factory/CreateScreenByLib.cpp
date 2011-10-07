#include "UselessPch.h"

#include "Useless/Graphic/Device/GL/wGL/WGLScreen.h"

namespace Useless {

    Screen * CreateScreenByLib( const char *title )
    {
        return new WGLScreen( title);
    }

};//namespace Useless
