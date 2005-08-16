#include "UselessPch.h"

#include "MaskedColor.h"

#include "Useless/Graphic/Transfer/SurfaceWriter.h"
#include "Useless/Graphic/FileIO/ImageFactory.h"
#include "Useless/Graphic/Transfer/MemOGfx.h"

#include "Useless/File/VIFS.h"
#include "Useless/Graphic/Planes/OGraphics.h"

#include "Useless/ErrorConfig.h"
#define   IMAGE_ERROR(e) APP_ERROR(e, "Image")

namespace Useless {

MaskedColor::MaskedColor() 
    :_alpha_loader(NULL), _alpha_loaded(false), _inverted(0), _alpha_0(0), _alpha_1(255), _angle(0.0)
{}

MaskedColor::MaskedColor( const char *alpha_file, NormalPixel color )
    :_alpha_file(alpha_file), _alpha_loaded(false), _color(color),
     _inverted(0), _alpha_0(0), _alpha_1(255), _angle(0.0)
{
    SPointer<IFile> file = IFS::Instance().OpenFile(_alpha_file);
    _alpha_loader = ImageFactory::Create(*file,&_properties.width,
        &_properties.height, (int*)&_properties.pixelformat );

    _rectangle = Rect(0, 0, _properties.width, _properties.height);

    _width = _rectangle.GetW(); _height = _rectangle.GetH();
    _properties.alpha = true;
    _properties.color = false;
}

MaskedColor::MaskedColor( const MaskedColor &image, const Rect &rect )
    :ImageBase(image, rect), _alpha_file(image._alpha_file),
    _alpha_loader(image._alpha_loader), _alpha_loaded(image._alpha_loaded),
    _color(image._color), _inverted(image._inverted),
    _alpha_0(image._alpha_0), _alpha_1(image._alpha_1), _angle(image._angle)
{}


MaskedColor::~MaskedColor()
{}

void MaskedColor::BlitTo( int x, int y, OGraphics& output)
{
    Surface &dest = *output.GetSurface();
    Cooperate( dest );

    Surf::BlitFX fx;
    fx.flood_fill = true;
    fx.flood_color = _color.u;
    fx.invert_src_alpha = _inverted;
    fx.alpha_factors[0] = _alpha_0;
    fx.alpha_factors[1] = _alpha_1;
    fx.rotation = !!_angle;
    fx.angle = _angle;

    dest.Blit( x, y, *GetSurface(), GetRect(), fx );
}

void MaskedColor::MultiBlitTo( OGraphics &output, const PointList &pnts, const RectList &rcs )
{
    Surface &dest = *output.GetSurface();
    Cooperate( dest );
    const Surface &source = *GetSurface();

    Surf::BlitFX fx;
    fx.flood_fill = true;
    fx.flood_color = _color.u;
    fx.invert_src_alpha = _inverted;
    fx.alpha_factors[0] = _alpha_0;
    fx.alpha_factors[1] = _alpha_1;
    fx.rotation = !!_angle;
    fx.angle = _angle;

    dest.MultiBlit( source, pnts, rcs, fx );
}

void MaskedColor::Load()
{
    {   // Workaround: SINGLE8 doesn't mean alpha...
        int w = GetWidth();
        int h = GetHeight();
        SPointer<PixelTransfer> sptColor = _sh_surface->CreateWriter( Surface::COLOR );
        sptColor->SetSource( w, h, w * 32, 32 );
        char *ptr = new char[w * h * 32];
        std::memset( ptr, 0xFF, w * h * 32 );
        sptColor->Fetch( ptr, Rect(0,0,w,h));
        sptColor->Transform();
    }
    
    SPointer<IFile> file = IFS::Instance().OpenFile(_alpha_file);

    SPointer<PixelTransfer> spt = _sh_surface->CreateWriter( Surface::ALPHA );
    _alpha_loader->Load( *file, *spt );
    spt->Transform();

    _alpha_loaded = true;
}



};//namespace Useless
