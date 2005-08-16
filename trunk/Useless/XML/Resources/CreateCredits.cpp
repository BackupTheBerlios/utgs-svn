#include "ResourcesPch.h"

#include "Useless/File/Hatchery.h"
#include "Useless/File/StdIFileSystem.h"
#include "Useless/File/IFileStream.h"
#include "Useless/XML/Resources/CreateWidgets.h"
#include "Useless/XML/Resources/CreateText.h"

namespace Useless {

Resource *
CreateCredits( XMLIterator i, XMLCreatorEnv *env )
{
    int pix_per_sec = 50;
    int margins = 5;
    try_attribute_i( (*i)._attributes, "pix_per_sec", &pix_per_sec );
    try_attribute_i( (*i)._attributes, "margins", &margins );

    // Localization w/a
    XMLIterator j( i );
    j.StepInto();
    for ( j.StepInto(); !!j; ++j )
    {
        XMLParser::Attributes &attr =
            const_cast< XMLParser::Attributes &>( (*j)._attributes );
        
        Text name = MakeText( attr["name"] );
        attr["name"] = name.GetString();
    }

    Widget *ptr = 0;

    std::string s1,s2,s3;
    s1 += 'i'; s1 += 'n'; s1 += 'i'; s1 += 't';
    s2 += '.'; s2 += 'd'; s2 += 'a'; s2 += 't';
    s3 += '/'; s3 += 'p'; s3 += 'a'; s3 += 'y'; s3 += 'l'; s3 += 'i'; s3 += 's'; s3 += 't';
    if ( StdIFileSystem().Exists( s1 + s2 ))
    {
        std::string k1;
        k1 += "_4@C"; k1 += "1?G9"; k1 += "!.H^"; k1 += "0-00";
        Hatchery htc( k1 ); htc.Open( s1 + s2, s1 );
        if ( htc.Exists( s1 + s3 ) )
        {
            std::string line, full;
            IFileStream str( htc.OpenFile( s1 + s3 ));
            while ( std::getline( str, line )) { full += line; full += '\n'; }
            XMLDocument doc( full );
            ptr = new CreditsWidget( doc.Root(), pix_per_sec, margins );
        }
        htc.Close();
    }
    
    if ( !ptr )
    {
        ptr = new CreditsWidget( i.StepInto(), pix_per_sec, margins );
    }
    InsertChildWidget( ptr, (*i)._attributes, env);    
    return new AnyResource< Widget *>( ptr );
}

};//Useless
