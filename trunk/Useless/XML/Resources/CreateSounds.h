#ifndef __INCLUDED_USELESS_RESOURCES_SOUNDS_H__
#define __INCLUDED_USELESS_RESOURCES_SOUNDS_H__

#include "Useless/XML/Resources/CreateFromXML.h"
#include "Useless/Sound/Device/SoundCard.h"

namespace Useless {

Resource* CreateSoundCard( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_soundcard( "system::soundcard", CreateSoundCard );


Resource* CreateSample( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_sound( "sounds::sample", CreateSample );


typedef ResourceQuery< SPointer< SoundCard > > SoundCardResource;
typedef ResourceQuery< SPointer< Sample > > SampleResource;


};//Useless

#endif//__INCLUDED_USELESS_RESOURCES_SOUNDS_H__
