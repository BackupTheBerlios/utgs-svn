#ifndef __INCLUDED__USELESS__IMAGE_DOT_TRACER_H__
#define __INCLUDED__USELESS__IMAGE_DOT_TRACER_H__

#include "Useless/GUI/Skins/ImageDicer.h"

namespace Useless {

class ImageDotTracer : public ImageDicer
{
public:
    ImageDotTracer(): _max_width(0), _max_height(0) {}
    ImageDotTracer( const RefImage &ref_image );
    virtual ~ImageDotTracer() {}

    int GetMaxWidth () const { return _max_width; }
    int GetMaxHeight() const { return _max_height; }

private:
    int  _max_width;
    int  _max_height;

    void InitTracer();

public:
    // some global parameters to setup optional positional correction
    // changes occure next time ImageDotTracer is used
    static int _Dx, _Dy, _Dw, _Dh;
};

};//namespace Useless
#endif//__INCLUDED__USELESS__IMAGE_DOT_TRACER_H__
