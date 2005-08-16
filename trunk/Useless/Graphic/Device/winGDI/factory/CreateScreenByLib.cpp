#include "Useless/Graphic/Device/winGDI/GDIScreen.h"

namespace Useless {

    Screen * CreateScreenByLib( const char *title )
    {
        return new GDIScreen( title);
    }

};//namespace Useless
