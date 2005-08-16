#include "Dynamo/classes/CInterfaceProvider.h"

namespace Dynamo {

    IAudio * CInterfaceProvider::ProvideIAudio()
    {
        assert(0);
        return 0;
    }

    //
    //  ProvideIGui is already implemented in CGui.cpp
    //
    //IGui * CInterfaceProvider::ProvideIGui();
    //{
    //    assert(0);
    //    return 0;
    //}

    IGameServer * CInterfaceProvider::ProvideIGameServer()
    {
        assert(0);
        return 0;
    }

    IProfileManager * CInterfaceProvider::ProvideIProfileManager()
    {
        assert(0);
        return 0;
    }

    IPlayerProfile * CInterfaceProvider::ProvideIPlayerProfile()
    {
        assert(0);
        return 0;
    }

    IGameInfo * CInterfaceProvider::ProvideIGameInfo()
    {
        assert(0);
        return 0;
    }

    //
    // ProvideIXml is already implemented in CXml.cpp
    //
    //IXml * CInterfaceProvider::ProvideIXml( std::string xmlId, std::string fileName, bool useEncryption )
    //{
    //    assert(0);
    //    return 0;
    //}
   
    //
    // ProvideIModule is already implemented in CModule.cpp
    // 
    //IModule *ProvideIModule( std::string moduleName, IXmlProgram *koolixp )
    //{
    //    assert(0);
    //    return 0;
    //}
    
    //
    // ProvideIXmlProgram is already implemented in CKooLiXP.cpp
    //
    //IXmlProgram * CInterfaceProvider::ProvideIXmlProgram( XMLProgram::IFiniteStateMachine *pFSM )
    //{
    //    assert(0);
    //    return 0;
    //}
    
    struct CMemoryManager : IMemoryManager, CInterface
    {
        void * Allocate( size_t size )
        {
            return new char[size];
        }

        void Deallocate( void *ptr )
        {
            delete ptr;
        }
    };

    IMemoryManager *CInterfaceProvider::ProvideIMemoryManager()
    {
        return new CMemoryManager();
    }

};//Dynamo
