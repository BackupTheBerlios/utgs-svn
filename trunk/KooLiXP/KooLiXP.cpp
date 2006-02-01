#include "KoolixpPch.h"

#include "Useless/UselessLib.h"
#include "Useless/System/Application.h"
#include "Useless/GUI/Display/ScreenShooter.h"
#include "Useless/Graphic/Device/winGDI/GDIScreen.h" //!< used for KooLiXP splash
#include "Useless/Graphic/Device/winGDI/GDISurface.h" //!< used for KooLiXP splash

#include "koolib/xml/XMLChunksGUI.h"
#include "koolib/xml/XMLModuleBuiltIn.h"
#include "koolib/xml/XMLModuleUseless.h"
#include "koolib/xml/XMLModuleDynamo.h"

#include "resource.h"

#include <direct.h>
#include <iostream>

namespace Useless {
    const char * Application::_icon_name = "icon1.ico";
    const char * Application::_app_title = "KooLiXP";
    const int    Application::_icon_resource_id = IDI_ICON1;
    const int    Application::_big_icon_resource_id = IDI_ICON1;

    extern bool G_EnableDDrawScr;
    extern bool G_DisableAltTab;
    extern bool G_EnableDebugF11;
    extern bool G_EnablePrintScr;
    extern bool G_nVidiaViewport;
    extern double G_GLTesselatorStartingTexelOffset;
    extern double G_GLTesselatorEndingTexelOffset;
};

int get_argc()
{
    return Useless::Application::GetArgC();
}

std::string get_argv( int i )
{
    std::wstring wname( Useless::Application::GetArgV()[i] );
    std::string name( wname.size(), 0 );
    std::copy( wname.begin(), wname.end(), name.begin() );
    return name;
}


struct KooLiXP
{ 
    Useless::Error _error;
    Useless::ScreenResource::Type _screen;
    bool        _snapshotOnly;
    bool        _helpOnly;
    bool        _noSplash;
    std::string _file;
    std::string _guiScreen;

    KooLiXP()
        : _snapshotOnly( false )
        , _helpOnly( false )
        , _noSplash( false )
        {
            XMLProgram::GlobalsAddRef();
            Application::SetIdleDT(5);

            for ( int i=1, I=get_argc(); i < I; ++i )
            {
                if ( 0 == get_argv(i).find("--"))
                {
                    if ( get_argv(i) == "--chroot" )
                    {
                        _chdir( get_argv(++i).c_str() );
                    }
                    else if( get_argv(i) == "--snapshot" )
                    {
                        _snapshotOnly = true;
                    }
                    else if( get_argv(i) == "--no-splash" )
                    {
                        _noSplash = true;
                    }
                    else if ( get_argv(i) == "--gui-screen" )
                    {
                        _guiScreen = get_argv(++i);
                    }
                    else if ( std::string::npos != get_argv(i).rfind( "-direct-draw-screen" ))
                    {
                        Useless::G_EnableDDrawScr = (std::string::npos == get_argv(i).find("--no"));
                    }
                    else if ( std::string::npos != get_argv(i).rfind( "-alt-tab" ))
                    {
                        Useless::G_DisableAltTab =  ( 0 == get_argv(i).find("--no"));
                    }
                    else if ( std::string::npos != get_argv(i).rfind( "-print-screen" ))
                    {
                        Useless::G_EnablePrintScr = (std::string::npos == get_argv(i).find("--no"));
                    }
                    else if ( std::string::npos != get_argv(i).rfind( "-debug-keys" ))
                    {
                        Useless::G_EnableDebugF11 = (std::string::npos == get_argv(i).find("--no"));
                    }
                    else if ( std::string::npos != get_argv(i).rfind( "-viewport-fix" ))
                    {
                        Useless::G_nVidiaViewport = ( 0 == get_argv(i).find("--no"));
                    }
                    else if ( get_argv(i) == "--idle-time-delta" )
                    {
                        Useless::Application::SetIdleDT( Useless::to_integer( get_argv(++i)));
                    }
                    else if ( get_argv(i) == "--texel-offsets" )
                    {
                        G_GLTesselatorStartingTexelOffset = Useless::to_real( get_argv(++i));
                        G_GLTesselatorEndingTexelOffset = Useless::to_real( get_argv(++i));
                    }
                    else if ( get_argv(i) == "--help" )
                    {
                        std::stringstream ss;
                        PutInfo( ss );
                        PutUsage( ss );
                        _helpOnly = true;
                        _error = Useless::Error("%s", ss.str().c_str());
                    }
                    else if ( get_argv(i) == "--" )
                    {
                        XMLProgram::ExecutionState &gS = XMLProgram::GetGlobals();
                        XMLProgram::IBlock *pBlock = 0;
                        gS._currentBlock->AddChunk( L"argv", pBlock = XMLProgram::CreateBlock() );
                        for ( ++i; i < I; ++i )
                        {
                            if ( get_argv(i) == "--" )
                            {
                                break;
                            }
                            int p = get_argv(i).find("=");
                            if ( std::string::npos != p )
                            {
                                std::string name  = get_argv(i).substr(0,p);
                                std::string value = get_argv(i).substr(p+1,-1);
                                pBlock->AddChunk( TextUtf8( name ), XMLProgram::CreateValue( TextUtf8( value )));
                            }
                            else
                            {
                                pBlock->AddChunk( get_argv(i), XMLProgram::CreateValue( 1 ));
                            }
                        }
                    }
                    else
                    {
                        std::stringstream ss;
                        PutInfo( ss );
                        PutError( ss, "Bad option '"+ get_argv(i) +"' given.\n\nFor usage type KooLiXP --help.");
                        _helpOnly = true;
                        _error = Useless::Error("%s", ss.str().c_str());
                    }
                }
                else
                {
                    _file = get_argv(i);
                }
            }
            if ( !_helpOnly )
            {
                if ( _file.empty() )
                {
                    std::stringstream ss;
                    PutInfo( ss );
                    PutError( ss, "No filename given.\n\nFor usage type KooLiXP --help.");
                    _helpOnly = true;
                    _error = Useless::Error("%s", ss.str().c_str());
                }
                else if ( !Useless::IFS::Instance().Exists( _file ))
                {
                    std::stringstream ss;
                    PutInfo( ss );
                    PutError( ss, "File '"+ _file +"' was not found.\n\nFor usage type KooLiXP --help.");
                    _helpOnly = true;
                    _error = Useless::Error("%s", ss.str().c_str());
                }
            }
        }

    int MakeScreenshot()
    {
        _screen->HideDecorations();
        _screen->Reposition(0,0);
        Useless::Application::Step();
        Useless::ScreenShooter(*_screen).Shot();
        return 0;
    }

    int Run()
    {
        if ( _helpOnly )
        {
            throw _error;
            return 0;
        }
        else
        {
            try {
                if ( !_noSplash )
                {
                    Surf::Properties splashProps;
                    splashProps.width = 320;
                    splashProps.height = 240;
                    splashProps.pixelformat = ImageFormat::B8G8R8A8;
                    _screen = new GDIScreen( Error("KooLiXP: Starting %s, Please wait...", _file.c_str()).GetError() );
                    _screen->Reposition( 480, 360 );
                    _screen->HideDecorations();
                    _screen->OpenWindowed( splashProps.width, splashProps.height);
                    _screen->SetClipper( Rect(splashProps.width, splashProps.height) );
                    GDISurface splashSurf( splashProps, _screen->GetHandle(), IDB_BITMAP1 );
                    _screen->GetSurface()->Blit( 0, 0, splashSurf, Rect(splashProps.width, splashProps.height) );
                    _screen->Swap();
                }

                Useless::CreateFromXMLFile( _file );
                
                if ( !!_screen )
                {
                    _screen->Close();
                }
                _screen = *Useless::ScreenResource("system", "screen");
                if ( !_guiScreen.empty() )
                {
                    Useless::SetCurrentScreen( Useless::WidgetResource< Useless::Widget >::Query( _guiScreen ));
                }
                if ( _snapshotOnly )
                {
                    return MakeScreenshot();
                }
                else
                {
                    return Useless::Application::Run();
                }
            }
            catch ( const std::exception &e )
            {
                if ( !!_screen )
                {
                    _screen->Close();
                }
                std::stringstream ss;
                PutInfo( ss );
                PutError( ss, e.what() );
                _helpOnly = true;
                if ( IDYES == MessageBoxA( NULL
                            , (ss.str() + "\n\nLog this message ?").c_str()
                            , (_file + " - Fatal Exception").c_str()
                            , MB_YESNO | MB_ICONEXCLAMATION | MB_SYSTEMMODAL ))
                {
                    throw Useless::Error( ss.str().c_str() );
                }
                return -1;
            }
        }
    }

    ~KooLiXP()
    {
        XMLProgram::GlobalsRemRef();
        Useless::Resources::Kill();
    }

    void PutInfo( std::ostream &out )
    {
        out << "KooLAS XML Instruction Processor - copyright 2005 (c), Radoslaw Kolasinski (koolas@gmail.com)" << std::endl;
        out << std::endl;
    }

    void PutError( std::ostream &out, const std::string &message )
    {
        out << "Error : " << message << std::endl;
        out << std::endl;
    }

    void PutUsage( std::ostream &out )
    {
        out << "Usage : " << "KooLiXP.exe [switches] filename [-- global=value ... --]" << std::endl;
        out << std::endl;
        out << "\t" << "--chroot" << "\n\t\t\t" << "change root directory" << std::endl;
        out << std::endl;
        out << "\t" << "--snapshot" << "\n\t\t\t" << "write screen snapshot to screen0.tga" << std::endl;
        out << std::endl;
        out << "\t" << "--gui-screen" << "\n\t\t\t" << "set current gui container widget" << std::endl;
        out << std::endl;
        out << "\t" << "--direct-draw-screen" << "\n\t\t\t" << "open screen via ddraw" << std::endl;
        out << std::endl;
        out << "\t" << "--no-alt-tab" << "\n\t\t\t" << "disable alt tab" << std::endl;
        out << std::endl;
        out << "\t" << "--no-print-screen" << "\n\t\t\t" << "disable print screen" << std::endl;
        out << std::endl;
        out << "\t" << "--no-debug-keys" << "\n\t\t\t" << "disable debug F11 key" << std::endl;
        out << std::endl;
        out << "\t" << "--viewport-fix" << "\n\t\t\t" << "fixes problem with Matrox" << std::endl;
        out << std::endl;
        out << "\t" << "--idle-time-delta n" << "\n\t\t\t" << "minimum miliseconds between subsequent updates" << std::endl;
        out << std::endl;
        out << "\t" << "--texel-offsets n m" << "\n\t\t\t" << "n,m are real numbers. " << "\n\t\t\t"
            << "try 'n m' = '0.5 0.5' to fix image on Radeon cards." << std::endl;
        out << std::endl;
    }
};

int Main()
{
    KooLiXP object;
    return object.Run();
}
