#ifndef __INCLUDED_KOOLIB_TEMPLATE_CONFIG_H__
#define __INCLUDED_KOOLIB_TEMPLATE_CONFIG_H__

#ifdef TEMPLATES_MSVC100
#   define KOOLIB_EMPTY_SPECIALIZATION
#   define KOOLIB_SPECIALIZATION template<>
#	define KOOLIB_CONSTRUCTOR_SPECIALIZATION
#   define KOOLIB_TYPENAME typename
#   define KOOLIB_STRUCT struct
#elif defined(TEMPLATES_MSVC70)
#   define KOOLIB_EMPTY_SPECIALIZATION
#   define KOOLIB_SPECIALIZATION template<>
#	define KOOLIB_CONSTRUCTOR_SPECIALIZATION
#   define KOOLIB_TYPENAME typename
#   define KOOLIB_STRUCT
#elif defined(TEMPLATES_GCC29)
#   define KOOLIB_EMPTY_SPECIALIZATION
#   define KOOLIB_SPECIALIZATION
#   define KOOLIB_CONSTRUCTOR_SPECIALIZATION
#   define KOOLIB_TYPENAME typename
#   define KOOLIB_STRUCT struct
#else
#   error You must define in the project one of: TEMPLATES_GCC29, TEMPLATES_MSVC70, TEMPLATES_MSVC100!!!
#
#endif


#endif//__INCLUDED_KOOLIB_TEMPLATE_CONFIG_H__
