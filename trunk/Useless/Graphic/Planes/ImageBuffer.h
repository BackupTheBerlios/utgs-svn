#ifndef __INCLUDED_IMAGE_BUFFER_H__
#define __INCLUDED_IMAGE_BUFFER_H__

/*
**  $Id: ImageBuffer.h,v 1.20 2004/07/13 10:14:33 koolas Exp $
**
**  NAME
**      Image
**
**  PURPOSE
**      Image read-only graphics for blits
**
**  AUTHOR(S)
**      KooLAS (koolas@iavision.com)
**
*/

#include "Useless/Graphic/Planes/ImageBase.h"
#include "Useless/Graphic/Planes/OGraphics.h"
#include "Useless/Util/CommonReference.h"
#include "Useless/Functional/Signal.h"

namespace Useless {

//REQUIRE:
    class Screen;
    class Clipper;

//////////////////////////////////////////////////////////////////////
//  CLASS: ImageBuffer
//
/*! \ingroup GfxPlanes
 *  Acts as source graphics, which can be blited onto some dest graphics,
 *  and also gives interface to draw on it's Surface
 */
class ImageBuffer : public ImageBase, public OGraphics
{
public:
    ImageBuffer();
    ImageBuffer( int w, int h);
    ImageBuffer( const ImageBuffer& image, const Useless::Rect &rect );
    ImageBuffer( Surface *psurface, const Useless::Rect &rect=Useless::Rect() );
    ImageBuffer( Share<Surface> sh_surface, const Useless::Rect &rect=Useless::Rect() );
    ImageBuffer &operator=( const ImageBuffer & );
    virtual ~ImageBuffer();
    virtual SPointer<IGraphics> Copy()                 const { return new ImageBuffer (*this); }
    virtual SPointer<IGraphics> Copy(const Rect &rect) const { return new ImageBuffer (*this, rect); }
    virtual SPointer<IGraphics> QueryIGraphics( const Rect &cut ) { return Copy(cut); }

    virtual const Surface* GetSurface() const { return _sh_surface.GetTarget(); }
    virtual       Surface* GetSurface()       { return _sh_surface.GetTarget(); }

    virtual int   GetWidth()  const  { return _rectangle.GetW(); }
    virtual int   GetHeight() const  { return _rectangle.GetH(); }

    virtual void Clear    ( NormalPixel color=0L, const Rect &area=Useless::Rect() );
    
    virtual void SetClipper( const RectList &region );
    virtual RectList GetClipper( const Rect &crop = Rect() ) const;
    
    void Create( const Screen& screen );
    void Resize(int w, int h);

    bool IsValid() const { return (!!_sh_surface); }

    Useless::Signal OnPaint;


protected:
    virtual void Load() { OnPaint.Send(); }
};

};// namespace Useless

#include "Useless/Util/CopyingFactory.h"
REGISTER_COPIER(Useless,IGraphics,ImageBuffer);
REGISTER_COPIER(Useless,ImageBase,ImageBuffer);

#endif //__INCLUDED_IMAGE_BUFFER_H__
