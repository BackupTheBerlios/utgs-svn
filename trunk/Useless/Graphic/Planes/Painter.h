#ifndef __INCLUDED_USELESS_PAINTER_H__
#define __INCLUDED_USELESS_PAINTER_H__

#include "Useless/Graphic/Planes/OGraphics.h"
#include "Useless/Graphic/Planes/SurfacePainter.h"
#include "Useless/Graphic/Pixel/Pixel.h"

namespace Useless {

/*! ... */
class Painter
{
public:
    Painter();
    Painter(     OGraphics &plane,   const Pos &offset = Pos() ); //!< Create painter
    Painter( const Painter &painter, const Pos &offset = Pos() ); //!< Copy painter
    virtual ~Painter();

    void  Clear          ( NormalPixel color )              const;
    void  PaintRectangle ( NormalPixel color, const Rect&)  const;
    void  ProjectImage   ( int x, int y, IGraphics& )       const;
    void  ProjectImages  ( IGraphics&, PointList, RectList) const;
    void  PaintBoundary  ( const Rect&, NormalPixel left, NormalPixel top,
                           NormalPixel right, NormalPixel bottom ) const;

    bool  Intersect      ( Rect crop_rect ); //!< Crop clip-region
    void  RestoreClipper ();                 //!< Restore

    void  MultiplyAlpha( int alpha );

    Pos   GetOffset      ()                                 const { return _offset; }

    void  Flush() const;

    SPointer< IGraphics >
        GetImage         ( const Rect &cut ) const;
    
    SurfacePainter &
        GetSurfacePainter() const;

    // Common drawing funcitons
    void  PutPixel       ( const Pos& p, NormalPixel color ) const;    

    void  PaintLine      ( const Pos &p1, const Pos &p2, NormalPixel color,
                           int thickness, int algorithm = 0 ) const;
    void  PaintLine      ( const std::vector<Pos> &vertices, NormalPixel color,
                           int thickness, int algorithm = 0 ) const;
    void  PaintPolygon   ( const std::vector<Pos> &vertices, NormalPixel color ) const;

    void * GetExtFun( const std::string &fun );

private:
    OGraphics         *_plane;          //!< Destination for paint operations
    Pos                _offset;         //!< Destination position offset
    RectList           _minimal_area;   //!< Initial destination clip-region
    int                _constantAlpha;
    
    mutable SPointer< SurfacePainter > _surfacePainter;
};

};//namespace Useless
#endif//__INCLUDED_USELESS_PAINTER_H__
