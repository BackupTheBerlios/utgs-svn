#include "UselessPch.h"

#include "Useless/Graphic/Device/GL/wGL/WGLScreen.h"
#include "Useless/Graphic/Planes/ImageBuffer.h"
#include "Useless/ErrorConfig.h"
#define WGLERR(e) APP_ERROR(e,"WGLScreen");

namespace Useless {

extern bool G_EnableDDrawScr;

IDirectDraw7 *G_pDDraw = 0;

void GLUpdateMovieSamples( GLTextureSurfaceAllocator *pTexAlloc );


WGLScreen::WGLScreen( const char *title): Window( title )
{
    // aviod refreshing with windows background color
    AllowClear(0);
}

WGLScreen::~WGLScreen()
{
    Close();
}

void WGLScreen::Open( int width, int height, int bpp, float refresh )
{
    Close();

    HWND hwnd = Window::GetHandle();
    HDC hdc = GetDC( hwnd );

    SetWindowLong( hwnd, GWL_STYLE, WS_POPUP);
    Window::Resize( width, height);

if ( !G_EnableDDrawScr )
{
    DEVMODE devmode;
    memset( &devmode, 0L, sizeof(DEVMODE));
    devmode.dmSize = sizeof(DEVMODE);

    int maxRefresh = 0;
    int i,j;
    for ( i=0,j=0;; ++i)
    {
        if ( !::EnumDisplaySettings( 0, ++i, &devmode ))
        {
            break;
        }
        if ( devmode.dmBitsPerPel==bpp &&
             devmode.dmPelsWidth==width &&
             devmode.dmPelsHeight==height )
        {
            if ( devmode.dmDisplayFrequency > maxRefresh )
            {
                maxRefresh = devmode.dmDisplayFrequency;
                j=i;
            }

            if ( devmode.dmDisplayFrequency == refresh )
            {
                break;
            }
        }
    }
    ::EnumDisplaySettings( 0, j, &devmode );
    devmode.dmFields |= DM_DISPLAYFREQUENCY;

    long res=ChangeDisplaySettingsEx( 0, &devmode, 0, CDS_FULLSCREEN, 0);
    switch(res)
    {
    case DISP_CHANGE_BADFLAGS: WGLERR("DISP_CHANGE_BADFLAGS"); break;
    case DISP_CHANGE_BADMODE: WGLERR("DISP_CHANGE_BADMODE"); break;
    case DISP_CHANGE_BADPARAM: WGLERR("DISP_CHANGE_BADPARAM"); break;
    case DISP_CHANGE_FAILED: WGLERR("DISP_CHANGE_FAILED"); break;
    case DISP_CHANGE_RESTART: WGLERR("DISP_CHANGE_RESTART"); break;
    };

    int w=::GetSystemMetrics( SM_CXSCREEN);
    int h=::GetSystemMetrics( SM_CYSCREEN);
    Window::Reposition( (w-width)/2, (h-height)/2);
}
else
{
    DirectDrawCreateEx( NULL, (void**)&G_pDDraw, IID_IDirectDraw7, NULL);
    SetWindowLong( hwnd, GWL_STYLE, WS_POPUP);
    G_pDDraw->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
    G_pDDraw->SetDisplayMode( width, height, bpp, 0, 0);
}

    Surf::Properties prop;
    prop.width = width;
    prop.height = height;
    _glContextSurface.Reset( new WGLSurface( prop ) );    
    _glContextSurface->_devContext = hdc;

    InitOpenGL();
    
    _glContextSurface->SetClipper( Rect(0, 0, width, height ));
    Window::Show();
}

void WGLScreen::OpenWindowed( int width, int height)
{
    Close();

    HWND hwnd = Window::GetHandle();
    HDC hdc = GetDC( hwnd );

    Rect dr = GetWindowRect() - GetClientRect();
    
    hdc = GetDC( hwnd);
    Window::Resize( width + dr.GetW(), height + dr.GetH() );
    
    Surf::Properties prop;
    prop.width = width;
    prop.height = height;
    _glContextSurface.Reset( new WGLSurface( prop ) );    
    _glContextSurface->_devContext = hdc;

    InitOpenGL();
    
    _glContextSurface->SetClipper( Rect(0, 0, width, height ));
    Window::Show();
}

void WGLScreen::InitOpenGL()
{
    // Initialize OpenGL
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory( &pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int iFormat = ChoosePixelFormat( _glContextSurface->_devContext, &pfd);
    SetPixelFormat( _glContextSurface->_devContext, iFormat, &pfd);
    _glContextSurface->_glContext = wglCreateContext( _glContextSurface->_devContext );
    _glContextSurface->MakeCurrent();
    Tie2Signal( OnResize, _glContextSurface.get(), &GLContextSurface::SetSize );
}

void WGLScreen::Close()
{
    _glContextSurface.Reset();

    if ( 0 != G_pDDraw )
    {
        G_pDDraw->Release();
        G_pDDraw = 0;
    }
}

void WGLScreen::Swap()
{
    SwapBuffers( _glContextSurface->_devContext );
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor( 1.0, 0.0, 0.0, 1.0);
    _glContextSurface->GLFreeGarbage();
    GLUpdateMovieSamples( _glContextSurface.get() );
}

void WGLScreen::Copy()
{
    SwapBuffers( _glContextSurface->_devContext );
}

Point<double> WGLScreen::GetZoomFactors() const
{
    return Point<double>(1,1);
}

void WGLScreen::Clear( NormalPixel color, const Useless::Rect &area )
{
    _glContextSurface->Clear( color.u, area );
}

void WGLScreen::SetClipper( const RectList &rect_list )
{
    _glContextSurface->SetClipper( rect_list );
}

RectList WGLScreen::GetClipper( const Rect &crop ) const
{
    return _glContextSurface->GetClipper( crop);
}


ImageBuffer WGLScreen::GetBackBuffer()
{
    assert(0);
    return ImageBuffer();
}

SPointer<IGraphics> WGLScreen::QueryIGraphics( const Rect &cut )
{
    assert(0);
    return new ImageBuffer();
}


void WGLScreen::PrintFPS( int x, int y, double dt )
{
    if ( dt )
    {
        char text[20];
        sprintf(text, "FPS: %5.2f", 1000.0/dt );
        PrintText(x,y,text);
    }
}

void WGLScreen::PrintNum(int x, int y, int fixed, int fraction)
{
    char text[40];
    ZeroMemory(&text,40);
    sprintf(text, "%8i.%8i", fixed, fraction );
    PrintText(x,y,text);
}

void WGLScreen::PrintText(int x, int y, const char* text)
{
    /*glDisable( GL_BLEND );
    glDisable( GL_SCISSOR_TEST );
    glColor4f( 1.0, 0.0, 0.0, 1.0 );
    
    glLoadIdentity();
    glRasterPos2f( 0, 0);
    glListBase( 1000 );
    glCallLists( strlen( text ), GL_UNSIGNED_BYTE, text );
    glListBase( 0 );*/
    
    // Initialize default font
    HFONT font = ::CreateFont
    (
        15,
        8,
        0,
        0,
        0,
        false,
        false,
        false,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        FF_ROMAN,
        NULL
    );
    
    TCHAR_string textout( ToTCHAR( text));

    HDC hdc = _glContextSurface->_devContext;

    COLORREF c=::GetNearestColor( hdc, RGB( 0xFF, 0x00, 0x00 ));
    HGDIOBJ lastObj = ::SelectObject  ( hdc, font );
    ::SetBkMode     ( hdc, TRANSPARENT );
    ::SetBkColor    ( hdc, 0L );
    ::SetTextColor  ( hdc, c);
    ::TextOut       ( hdc, x, y, textout.c_str(), textout.length() );
    ::SelectObject  ( hdc, lastObj );
    ::DeleteObject  ( font );
}

};//namespace Useless
