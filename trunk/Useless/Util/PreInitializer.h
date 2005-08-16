#ifndef __INCLUDED_USELESS_PRE_INITIALIZER_H__
#define __INCLUDED_USELESS_PRE_INITIALIZER_H__

namespace Useless {

/*! \ingroup Util
 *  PreInitialisers are used to initialize static elements of classes
 *  before main application starts, and to terminate them after user programm end.
 */
template< class T >
class PreInitializer
{
public:
    PreInitializer()  { if ( 0==_counter++) T::Init();    }
    ~PreInitializer() { if ( --_counter==0) T::Destroy(); }

private:
    static int _counter;
};

};//namespace Useless
#endif//__INCLUDED_USELESS_PRE_INITIALIZER_H__
