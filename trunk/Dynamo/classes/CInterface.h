#ifndef __INCLUDED_DYNAMO_CINTERFACE_H__
#define __INCLUDED_DYNAMO_CINTERFACE_H__

#include "Dynamo/interfaces.h"

namespace Dynamo {

    struct CInterface : virtual IInterface
    {
        CInterface(): _refCount(0)
        {
        }

        ~CInterface()
        {
        }

        int _refCount;

        int AddRef()
        {
            return ++_refCount;
        }

        int RemRef()
        {
            assert( _refCount > 0 );
            if ( 0 == --_refCount )                
            {
                Release();
                return 0;
            }
            else {
                return _refCount;
            }
        }

        virtual void Release()
        {
            delete this;
        }
    };

    struct CMemory : CInterface        
    {
        virtual void Release()
        {
            void *start = _pStart;
            Hand< IMemoryManager > manager = _pMemoryManager;
            this->~CMemory();
            manager->Deallocate( start );
        }

        void SetMemory( void *pStart, IMemoryManager *pMemoryManager )
        {
            _pStart = pStart;
            _pMemoryManager = pMemoryManager;
        }

        private:
        void *_pStart;
        Hand< IMemoryManager > _pMemoryManager;
    };

};//Dynamo

#endif//__INCLUDED_DYNAMO_CINTERFACE_H__
