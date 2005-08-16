#include "UselessPch.h"

#include "Useless/Graphic/Planes/OGraphics.h"
#include "Useless/Graphic/Device/Surface.h"

namespace Useless {


    void OGraphics::SetConstantAlpha( int alpha )
    {
        GetSurface()->SetConstantAlpha( alpha );
    }

    int  OGraphics::GetConstantAlpha() const
    {
        return GetSurface()->GetConstantAlpha();
    }


}; //Useless
