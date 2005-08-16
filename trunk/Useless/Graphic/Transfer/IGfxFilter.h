#ifndef __INCLUDED_USELESS_IGFXFILTER_H__
#define __INCLUDED_USELESS_IGFXFILTER_H__

/*
**  $Id: IGfxFilter.h,v 1.4 2002/09/13 16:22:17 koolas Exp $
**
**  NAME
**      IGfxFilter
**
**  PURPOSE
**      Input graphics filter abstract class.
**
**  AUTHOR(S)
**      KooLAS (koolas@iavision.com)
*/

#include "Useless/System/Types.h"

namespace Useless {
/*! \ingroup ImgIO
 *  Abstract source of image transfer.
 */
class IGfxFilter
{
public:
    IGfxFilter() {}
    virtual ~IGfxFilter() {}

    virtual void GetSize( int *w, int *h, int *d ) = 0;
    virtual void ReadPalette(const Types::UBYTE **rgb, int *pal_size) = 0;
    virtual void Read( Types::UBYTE *s, int x, int y, int w, int h, int pitch ) = 0;
};


} // namespace Useless

#endif __INCLUDED_USELESS_IGFXFILTER_H__
