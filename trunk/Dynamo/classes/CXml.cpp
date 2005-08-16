#include "Dynamo/classes/CInterfaceProvider.h"
#include "koolib/HyperObject.h"
#include "Useless/HatcheryConfig.h"
#include "Useless/File/StdOFile.h"
#include "Useless/File/StdIFileSystem.h"
#include "Useless/File/StdIFile.h"

namespace Dynamo {

struct CXml : CInterface, virtual IXml
{
    std::string _id;
    std::string _fileName;
    bool        _useEncryption;
    HyperObjectResource::Type _data;

    CXml( const std::string &xmlId, const std::string fileName, bool useEncryption )
    {
        _id = xmlId;
        _fileName = fileName;
        _useEncryption = useEncryption;
        
        if ( Useless::StdIFileSystem().Exists( _fileName ))
        {
            Useless::SPointer< Useless::IFile > input;        
            input = new Useless::StdIFile( _fileName );
            if ( useEncryption )
            {
                input = new Useless::HatcheryCipher::In( input, Useless::__key );
            }
            Useless::__Resources::ResourceMap::iterator it = Useless::Resources::Instance().Find("hyper", xmlId);
            if ( it != Useless::Resources::Instance().End() )
            {
                Useless::Resources::Instance().Erase( it );
            }
            Useless::CreateFromXMLFile( input );
        }

        _data = *HyperObjectResource("hyper", xmlId);
    }

    ~CXml()
    {
    }

    HyperObject & GetData()
    {
        return (*_data);
    }
        
    void WriteData()
    {
        Useless::XMLTag tag1("xml");
        Useless::XMLTag tag2("hyper");
        Useless::XMLTag tag3("data");
        tag3 << GetData();
        tag1.setvalue( tag2.setvalue( tag3 ));
        tag1.setattr("encoding","utf-8");

        Useless::StdOFile oFile( _fileName );
        Useless::HatcheryCipher::Out encFile( Useless::SPointer< Useless::OFile >( oFile ), Useless::__key );

        std::string s = tag1.to_string();
        
        if ( !_useEncryption )
        {
            oFile.WriteUpto( s.c_str(), s.length() );
        }
        else
        {
            int remainAlgn = s.length() % Useless::HatcheryCipher::BLOCKSIZE;        
            if ( 0 != remainAlgn )
            {
                int toAlign = Useless::HatcheryCipher::BLOCKSIZE - remainAlgn;
                while ( 0 < toAlign-- )
                {
                    s += (char)0x0;
                }
            }
            encFile.Write( s.c_str(), s.length() );
        }
    }
};


IXml * CInterfaceProvider::ProvideIXml( std::string xmlId, std::string fileName, bool useEncryption )
{
    return new CXml( xmlId, fileName, useEncryption );
}

};//Dynamo
