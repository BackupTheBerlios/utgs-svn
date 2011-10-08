//
// Use these 3rd party libraries
//

#define USELESS_HAS_DSHOW
#define USELESS_HAS_VORBIS
//#define USELESS_HAS_CRYPTOPP
#define USELESS_HAS_BOOST_SERIALIZATION
#define USELESS_HAS_LIBPNG



//
// Configurations
//

#ifdef UTGS_STATIC_MSVC10
#   define KOOLIB_STATICLIB
#   define USELESS_STATICLIB
#   define TEMPLATES_MSVC100
#endif

#ifdef UTGS_DLL_MSVC10
#   define TEMPLATES_MSVC100
#endif

#ifdef USELESS_DLL_MSVC10
#   define USELESS_EXPORTS
#   define TEMPLATES_MSVC100
#endif

#ifdef USELESS_RESOURCES_DLL_MSVC10
#   define USELESS_RESOURCES_EXPORTS
#   define TEMPLATES_MSVC100
#endif

#ifdef KOOLIB_DLL_MSVC10
#   define KOOLIB_EXPORTS
#   define TEMPLATES_MSVC100
#endif

