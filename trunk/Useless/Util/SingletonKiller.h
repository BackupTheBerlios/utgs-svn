#ifndef __INCLUDED_SINGLETON_KILLER_H__
#define __INCLUDED_SINGLETON_KILLER_H__

#include "Useless/Util/PreInitializer.h"

namespace Useless {

template< class SingletonType >
class SingletonKiller
{
public:
    typedef PreInitializer< SingletonKiller > PostDestr;

private:
    static void Init() {}
    static void Destroy() { SingletonType::SafeKill(); }

    friend class PreInitializer< SingletonKiller >;
};


/* Type this lines into place where you will use killer
--------------------------------------------------------
in .h:
    static SingletonKiller<T>::PostDestr _your_singleton_post_destructor;
*/

#define DECLARE_SINGLETON(base_name, singleton_name) \
typedef Useless::Singleton< base_name > singleton_name; \
static Useless::SingletonKiller<singleton_name>::PostDestr _##singleton_name##_singleton_post_destructor;

#define DECLARE_SINGLETON_CTOR(base_name, singleton_name, ctor_type) \
typedef Useless::Singleton< base_name, ctor_type > singleton_name; \
static Useless::SingletonKiller<singleton_name>::PostDestr _##singleton_name##_singleton_post_destructor;

/*
in .cpp:
    int SingletonKiller<T>::PostDestr::_counter = 0; 
*/
#define INIT_SINGLETON(singleton_name) \
int Useless::SingletonKiller< singleton_name >::PostDestr::_counter = 0; \
singleton_name::type* singleton_name::pInstance = 0;




};//namespace Useless

#endif//__INCLUDED_SINGLETON_KILLER_H__
