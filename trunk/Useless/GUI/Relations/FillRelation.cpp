#include "UselessPch.h"

#include "Useless/GUI/Relations/FillRelation.h"

namespace Useless {

FillRelation::FillRelation( Widget &widget )
{   
    assert( widget.GetParent() && "FillRelation::FillRelation(): Not in group");
    widget.Fill();
    _on_client_area_resize = widget.GetParent()->OnResize.TieUnary( &widget, &Widget::Fill, Rect() );
}

FillRelation::~FillRelation()
{
    _on_client_area_resize.Untie();
}

};//namespace Useless 
