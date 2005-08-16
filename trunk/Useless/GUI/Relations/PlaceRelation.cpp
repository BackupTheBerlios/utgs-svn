#include "UselessPch.h"

#include "Useless/GUI/Relations/PlaceRelation.h"

namespace Useless {

PlaceRelation::PlaceRelation( Widget &widget, const Pos &pos, const Pos &base)
{
    widget.Place( pos, base);
    widget.GetParent()->OnResize.TieBinary( &widget,
            (void (Widget::*)(const Pos&, const Pos&))&Widget::Place,
            pos, base);
}

PlaceRelation::PlaceRelation( Widget &widget, const Rect &rect, const Pos &base)
{
    widget.Place( rect, base);
    widget.GetParent()->OnResize.TieBinary( &widget,
            (void (Widget::*)(const Rect&, const Pos&))&Widget::Place,
            rect, base);
}

PlaceRelation::~PlaceRelation()
{
    _on_resize.Untie();
}


};//namespace Useless
