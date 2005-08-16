#ifndef __INLCUDED_CREATE_TEXT_H__
#define __INLCUDED_CREATE_TEXT_H__

#include "Useless/XML/Resources/CreateFromXML.h"
#include "Useless/GUI/Text/Text.h"
#include "Useless/GUI/Misc/DataTypes.h"

namespace Useless {

Resource* CreateText( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_text( "texts::text", CreateText );
static RegisterXMLCreator reg_create_dt_text( "texts::dt_text", CreateText );

Resource* ResolveParentGroup( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_resolve_parent_group( "texts::group", ResolveParentGroup );

/* Query methods
----------------------------------------------------------------------*/

inline Text& QueryText( const std::string &id )
{
    SPointer<Text> *pt;
    Resources::Instance().Query("texts", id, pt);
    return **pt;
}

inline DT_Text& QueryDTText( const std::string &id )
{
    SPointer<DT_Text> *pt;
    Resources::Instance().Query("texts", id, pt);
    return **pt;
}

/* Helper methods
----------------------------------------------------------------------*/
void BeginParentGroup( XMLCreatorEnv *env, const char* &name, XMLCreatorEnv &ch_env );

Text MakeText( const Text &input );


}; // namespace Useless

#endif // __INLCUDED_CREATE_TEXT_H__
