#ifndef __INCLUDED_USELESS_TEMPLATE_CONFIG_H__
#define __INCLUDED_USELESS_TEMPLATE_CONFIG_H__

#ifdef WIN32
#   define USELESS_EMPTY_SPECIALIZATION
#   define USELESS_SPECIALIZATION template<>
#   define USELESS_TYPENAME typename
#   define USELESS_STRUCT
#else
#   define USELESS_EMPTY_SPECIALIZATION
#   define USELESS_SPECIALIZATION
#   define USELESS_TYPENAME typename
#   define USELESS_STRUCT struct
#endif

#endif//__INCLUDED_USELESS_TEMPLATE_CONFIG_H__
