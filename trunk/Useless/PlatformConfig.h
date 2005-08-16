#ifndef __INCLUDED_USELESS_PLATFORM_CONFIG_H__
#define __INCLUDED_USELESS_PLATFORM_CONFIG_H__

///////////////////////////////////
// THIS IS PLATFORM CONFIGURATION
//
// * Win32
//   + RenderWare Graphics
//   + DirectDraw 7.0
//   + DirectSound 8.1
//   + DirectShow (DX 8.1)
//
//--------------------------------

///////////////////////////////////
// User configuration
//--------------------------------

// Do we use Renderware-Graphics library ?

#ifndef RENDERWARE
#define RENDERWARE
#endif // RENDERWARE



///////////////////////////////////
// Automatic configuration
//--------------------------------
#ifdef RENDERWARE
#   define __USE_RENDERWARE__
#endif

#ifdef WIN32

#   ifndef __USE_RENDERWARE__
#       define __USE_DIRECT_DRAW__
#   endif

#   define __USE_DIRECT_SOUND__
#   define __USE_DIRECT_SHOW__

#endif

#endif//__INCLUDED_USELESS_PLATFORM_CONFIG_H__
