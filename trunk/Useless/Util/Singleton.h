#ifndef __INCLUDED_USELESS_SINGLETON_H__
#define __INCLUDED_USELESS_SINGLETON_H__

/*
**  $Id: Singleton.h,v 1.8 2002/09/13 16:22:11 koolas Exp $
**
**  NAME
**      Singleton
**
**  PURPOSE
**      Singleton has only one instance
**
**  AUTHOR(S)
**      KooLAS (koolas@iavision.com)
**
*/

//#include "unused Threads/Lock.h"

namespace Useless {
        
template< class T>
class CreateUsingNew
{
    public:    
        static T* Create(){ return new T; }
        static void Destroy(T * pt) { delete pt; }
};


template< class T>
class CreateUsingMalloc
{
    public:    
        static T* Create(){ return malloc( sizeof(T) ); }
        static void Destroy(T * pt) { free(pt); }
};

/*! \ingroup Util
 *  Use when only one instance of your class is to be allowed in whole application.
 */
template< class T, class CreationPolicy = CreateUsingNew<T> >
class Singleton
{
    public:
        static T& Instance()
        {
            //static Mutex _mutex;
            //Lock guard(_mutex);
            if (!pInstance)
            {
                pInstance = CreationPolicy::Create();    
            }
            return (*pInstance);
        }        

        static void Kill()
        {
            CreationPolicy::Destroy(pInstance); pInstance=0;
        }

        static void SafeKill()
        {
            if (!!pInstance)
            CreationPolicy::Destroy(pInstance); pInstance=0;
        }

        static bool Exists() { return !!pInstance; }

        typedef T type;

    private:
        Singleton();
        Singleton(const Singleton&);
        Singleton& operator=(const Singleton&);
        ~Singleton();
        static  T * pInstance;
};

}; // namespace Useless

#include "Useless/Util/SingletonKiller.h"

#endif //__INCLUDED_USELESS_SINGLETON_H__
