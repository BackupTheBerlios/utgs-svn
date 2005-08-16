#ifndef __INCLUDED_USELESS_CREATE_FILESYSTEMS_H__
#define __INCLUDED_USELESS_CREATE_FILESYSTEMS_H__

#include "Useless/XML/Resources/CreateFromXML.h"
#include "Useless/File/VIFS.h"

namespace Useless {

    /* Create methods
---------------------------------------------------------------------------------------------*/
Resource* CreateMountDir( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_mount_dir( "filesystems::dir", CreateMountDir );

Resource* CreateMountHtc( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_mount_htc( "filesystems::htc", CreateMountHtc );

Resource* CreateMountScan( XMLIterator i, XMLCreatorEnv *env );
static RegisterXMLCreator reg_create_mount_scan( "filesystems::scan", CreateMountScan );

};

#endif//__INCLUDED_USELESS_CREATE_FILESYSTEMS_H__
