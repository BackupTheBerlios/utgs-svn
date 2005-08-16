#ifndef __INCLUDED_USELESS_CREATE_SKINS_H__
#define __INCLUDED_USELESS_CREATE_SKINS_H__

#include "Useless/XML/Resources/CreateFromXML.h"
#include "Useless/Graphic/Planes/ImageBase.h"
#include "Useless/Graphic/Planes/Animation.h"
//#include "Useless/Gfx/GraphicPlanes/AnimatedImage.h" // - no support under renderware (to be replaced by DSMedia)
#include "Useless/GUI/Skins/ImageDicer.h"
#include "Useless/GUI/Skins/SkinSet.h"
#include "Useless/GUI/Skins/BarSkinner.h"
#include "Useless/GUI/Skins/FrameSkin.h"
#include "Useless/GUI/Skins/ParametricSkin.h"
#include "Useless/GUI/Skins/ImageSkin.h"
#include "Useless/GUI/Skins/NullSkin.h"
#include "Useless/GUI/Skins/SizerSkin.h"
#include "Useless/GUI/Skins/TableSkin.h"
#include "Useless/GUI/Skins/SkinMixer.h"

namespace Useless {

/* Create methods
---------------------------------------------------------------------------------------------*/
Resource* CreateImage( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_image( "images::image", CreateImage );

Resource* CreateImageCopy( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_image_copy( "images::copy", CreateImageCopy );

Resource* CreateAnimation( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_animation( "images::animation", CreateAnimation );

Resource* CreateMapping( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_map( "mappings::map", CreateMapping );

Resource* CreateImageDicer( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_image_dicer( "skins::dicer", CreateImageDicer );

Resource* CreateBarSkins( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_bar_skins( "skins::bars", CreateBarSkins );
static RegisterXMLCreator reg_create_bar_skin( "skins::bar", CreateBarSkins );

Resource* CreateFrameSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_frame_skin( "skins::frame", CreateFrameSkin );

Resource* CreateParametricSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_param_skin( "skins::parametric", CreateParametricSkin );

Resource* CreateImageSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_image_skin( "skins::image", CreateImageSkin );

Resource* CreateImageSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_image_skins( "skins::images", CreateImageSkin );

Resource* CreateSkinSet( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_skin_set( "skins::skinset", CreateSkinSet );

Resource* ResolveSkinLink( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_resolve_skin_link( "skins::link", ResolveSkinLink );
static RegisterXMLCreator reg_resolve_skin_copy( "skins::copy", ResolveSkinLink );

Resource* CreateNullSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_null_skin( "skins::null", CreateNullSkin );

Resource* CreateSizerSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_sizer_skin( "skins::sizer", CreateSizerSkin );

Resource* CreateTableSkin( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_table_skin( "skins::table", CreateTableSkin );

Resource* CreateSkinMixer( XMLIterator i, XMLCreatorEnv *env);
static RegisterXMLCreator reg_create_skin_mixer( "skins::skinmixer", CreateSkinMixer );


/* Query methods
---------------------------------------------------------------------------------------------*/
typedef ResourceQuery< SPointer< ImageBase > > ImageResource;
typedef ResourceQuery< SPointer< Skin > >      SkinResource;
typedef ResourceQuery< SPointer< SkinMan > >   SkinManResource;


inline ImageBase& QueryImage( const std::string &id )
{
    SPointer<ImageBase> *p_image;
    Resources::Instance().Query("images", id, p_image);
    return **p_image;
}

inline Animation& QueryAnimation( const std::string &id )
{
    SPointer<ImageBase> *p_image;
    Resources::Instance().Query("images", id, p_image);
    Animation *p_ani = dynamic_cast<Animation*>(&**p_image);
    return *p_ani;
}

inline SkinMan& QuerySkinMan( const std::string &id )
{
    SPointer<SkinMan> *p_skin_man;
    Resources::Instance().Query("skins", id, p_skin_man);
    return **p_skin_man;
}   

inline ImageDicer& QueryImageDicer( const std::string &id )
{
    ImageDicer *p = dynamic_cast<ImageDicer*>( &QuerySkinMan(id) );
    if (!p) { throw Error("SkinMan type is not ImageDicer"); }
    return *p;
}

inline SkinSet& QuerySkinSet( const std::string &id )
{
    SkinSet *p = dynamic_cast<SkinSet*>( &QuerySkinMan(id) );
    if (!p) { throw Error("SkinMan type is not SkinSet"); }
    return *p;
}

inline Skin& QuerySkin( const std::string &id )
{
    SPointer<Skin> *p_skin;
    Resources::Instance().Query("skins", id, p_skin);
    return **p_skin;
}

inline ArraySkin& QueryArraySkin( const std::string &id )
{
    ArraySkin *p = dynamic_cast<ArraySkin*>( &QuerySkin(id) );
    if (!p) { throw Error("Skin type is not array"); }
    return *p;
}


/* Helper methods
---------------------------------------------------------------------------------------------*/
SPointer<ImageBase> MakeImage( const std::string &id );//< create image "image_name(x,y,w,h)" 

bool InsertChildSkin( const Skin &skin, XMLCreatorEnv *env );
void SetupSkinAttributes( Skin &skin, const XMLAttributes &attributes);

};//namespace Useless
#endif//__INCLUDED_USELESS_CREATE_SKINS_H__
