#define USELESS_API __declspec(dllimport)

#pragma warning(disable:4244)
#pragma warning(disable:4251)
#pragma warning(disable:4267)
#pragma warning(disable:4275)

#include "Useless/UselessConfig.h"

#include "Useless/File/VIFS.h"
#include "Useless/System/Application.h"
#include "Useless/System/Window.h"
#include "Useless/System/factory/CreateScreen.h"
#include "Useless/System/factory/CreateSound.h"
#include "Useless/Graphic/FileIO/PNGLoader.h"
#include "Useless/Graphic/Planes/TransparentImage.h"
#include "Useless/Graphic/Planes/Painter.h"
#include "Useless/Graphic/Device/GVM_Context.h"
#include "Useless/Graphic/Device/GVM_Message.h"
#include "Useless/Sound/FileIO/WaveLoader.h"
#include "Useless/Sound/SampleFile.h"
#include "Useless/GUI/Text/Font.h"
#include "Useless/GUI/Skins/FrameSkin.h"
#include "Useless/XML/Resources/CreateGUIResources.h"

using namespace Useless;

void OnExit()
{
	Resources::Kill();
}


struct Test
{
    TransparentImage       m_Image;
    SampleFile  m_audioSample;
    Font		m_font;
    SPointer< FrameSkin	> m_frame;

    Test()
    {
        m_Image = TransparentImage("../Data/Image.png", "../Data/Image.png");
        m_audioSample = SampleFile("../Data/Audio.wav");
        m_font = Font( "../Data/Font.png", "../Data/Font.png", "../Data/Font.desc" );
        m_frame = new FrameSkin("../Data/Frame.png", Rect(0,0,128,128), Rect(16,16,100,100) );

        CreateFromXMLFile("../Data/File.xml");
    }

    void DoPaint( Screen *screen )
    {
        screen->Clear( 0xFF00A0FF );
        m_Image.BlitTo( 0, 0, *screen );
        Render( Painter(*screen) );
        screen->Swap();
    }

    void Render( Painter &paint )
    {
        m_frame->SetSize( Dim2i( 256, 256 ));
        m_frame->Paint( paint, Pos(480,0) );
        m_font.Paint( paint, "Hello World", Pos(512,100));

        using namespace GVM;
        paint.SetOrtho2D( -4, -4, 4, 4 );
        Message msg =
            ScaleMsg( GAL::P3f( 0.25, 0.25, 0.25 ) )+
            ShapeMsg( TRIANGLE_STRIP, 
                    ColorMsg( GAL::P4f( 1.0, 0.0, 0.0, 1.0) ) +
                    VertexMsg( GAL::P3f( 0,0,1) ) +
                    VertexMsg( GAL::P3f( 1,0,0) ) +
                    VertexMsg( GAL::P3f( 0,1,0) ) +
                    VertexMsg( GAL::P3f( 1,1,0) )
                    );
        paint.Invoke( msg );
    }

    void DoSound( SoundCard *card )
    {
        card->Play( m_audioSample );
    }
};


int Main()
{
    SPointer< Screen > screen = CreateScreen("Hello");
    SPointer< SoundCard > card = Creator< SoundCard >::Create( *screen );
    screen->OpenWindowed( 800, 600 );
    screen->Clear( 0xFF0000FF );
    screen->Swap();


    Test test;
    screen->OnPaint.TieUnary( &test, &Test::DoPaint, screen.get() );
    screen->OnMouseBtn.TieUnary( &test, &Test::DoSound, card.get() );
    screen->OnPaint.Send();

    Application::Run();
    return 0;
}
