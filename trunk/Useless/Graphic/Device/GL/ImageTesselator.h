#ifndef __INCLUDED_USELESS_IMAGE_TESSELATOR_H__
#define __INCLUDED_USELESS_IMAGE_TESSELATOR_H__

#include "Useless/Graphic/Device/GL/PixelBuffer.h"
#include "Useless/Graphic/Device/GL/Texture.h"

namespace Useless {

class ImageTesselator
{
public:
    ImageTesselator( const PixelBuffer &pix_buf );
    ~ImageTesselator();

    void Render();
    void Render( float screen_x, float screen_y, bool absolute=true );
    void Render( float screen_x, float screen_y, float screen_w, float screen_h, bool absolute=true );

    struct RenderTransform
    {
        double w_scale;
        double h_scale;
        double l_bnd;
        double t_bnd;
        double h_full;
        double v_full;
        double h_size;
        double v_size;
        double x_scale;
        double y_scale;
        double x_scale_recip;
        double y_scale_recip;
        double h_full_recip;
        double v_full_recip;
    };

    void SetRenderTransform( RenderTransform *pTrans, double w_scale, double h_scale );
    void UseRenderTransform( ImageTesselator::RenderTransform *T, double screen_x, double screen_y );

    void Clip( float min_x, float min_y, float max_x, float max_y );

    double GetWidth() const { return _w; }
    double GetHeight()const { return _h; }
    
    void SetFlip( bool x, bool y ) { _flipX=x; _flipY=y; }

    void UpdateAllTextures( const PixelBuffer &pix_buf );
    void UpdateAllTextures( const void *pData, int width, int height, int fmt, int rowLength );

private:
    double _w;
    double _h;
    bool   _flipX;
    bool   _flipY;

    struct Tessel
    {
        Tessel( const Texture &texture ): _texture( texture ) {}
        Texture _texture;
        double  _u1;
        double  _u2;
        double  _v1;
        double  _v2;
        double  _x1;
        double  _x2;
        double  _y1;
        double  _y2;
    };

    std::vector< GLuint > _tesselRowHeights;
    std::vector< GLuint > _tesselColWidths;
    std::vector< Tessel > _tesselAll;
    std::vector< GLuint > _tesselRender;
    friend struct GLContextSurface;
};

}; //namespace Useless
#endif//__INCLUDED_USELESS_IMAGE_TESSELATOR_H__
