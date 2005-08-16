#ifndef __INCLUDED__USELESS_PLACE_RELATION_H__
#define __INCLUDED__USELESS_PLACE_RELATION_H__

#include "Useless/GUI/Relations/Relation.h"
#include "Useless/GUI/Widgets/Widget.h"

namespace Useless {

class PlaceRelation : public Relation
{
public:
    PlaceRelation( Widget &widget, const Pos &pos, const Pos &base);
    PlaceRelation( Widget &widget, const Rect &rect, const Pos &base);
    virtual ~PlaceRelation();

private:
    Signal::FuncID _on_resize;
};

};//namespace Useless

#endif//__INCLUDED__USELESS_PLACE_RELATION_H__
