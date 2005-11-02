#include "UselessPch.h"

#include "Useless/Graphic/Device/GL/wGL/WGLScreen.h"
#include "Useless/Graphic/Planes/ImageBuffer.h"
#include "Useless/ErrorConfig.h"
#define WGLERR(e) APP_ERROR(e,"WGLScreen");

namespace Useless {

    extern bool G_EnableDDrawScr;

    // Used to start fullscreen via DDraw.
    IDirectDraw7 *G_pDDraw = 0;

    WGLScreen::WGLScreen( const char *title): Window( title ), m_active( false )
    {

        // aviod refreshing with windows background color
        AllowClear(0);
        
        // Screen surface has these attributes:
        m_prop.primary = true;
        m_prop.renderable = true;
        m_prop.color = true;
        m_prop.zbuffer = true;
        m_prop.num_surfaces = 2;
    }

    WGLScreen::~WGLScreen()
    {
        Close();
    }

    void WGLScreen::Open( int width, int height, int bpp, float refresh )
    {
        Close();
        
        m_windowed = false;
        m_prop.width = width;
        m_prop.height = height;
        m_prop.pixelformat = ImageFormat::R8G8B8;

        HWND hwnd = Window::GetHandle();
        HDC hdc = GetDC( hwnd );

        SetWindowLong( hwnd, GWL_STYLE, WS_POPUP);

        if ( !G_EnableDDrawScr )
        {
            InitDevMode( width, height, bpp, refresh );
            _glContextSurface.Reset( new WGLSurface( hdc, m_prop ) );
            
            // Signals below are used only in GDI fullscreen.
            // They are not available under DDraw fullscreen.
            Tie2Signal( OnActivate, this, &WGLScreen::SlotFullscreenActive );
            Tie2Signal( OnDeactivate, this, &WGLScreen::SlotFullscreenInactive );
        }
        else
        {
            InitDDraw( width, height, bpp, refresh );
            _glContextSurface.Reset( new WGLSurface( hdc, m_prop ) );    
        }
        
        _glContextSurface->GetProperties( &m_prop );

        Tie2Signal( OnResize, this, SlotResize );
        Window::Show();
    }

    void WGLScreen::OpenWindowed( int width, int height)
    {
        Close();
        
        m_windowed  = true;
        m_prop.width = width;
        m_prop.height = height;
        m_prop.pixelformat = ImageFormat::R8G8B8;
        
        Rect dr = GetWindowRect() - GetClientRect();
        Window::Resize( width + dr.GetW(), height + dr.GetH() );

        HWND hwnd = Window::GetHandle();
        HDC hdc = GetDC( hwnd );
        
        _glContextSurface.Reset( new WGLSurface( hdc, m_prop ) );
        
        Tie2Signal( OnResize, this, SlotResize );
        Window::Show();
    }

    void WGLScreen::InitDevMode( int width, int height, int bpp, float refresh )
    {
        HWND hwnd = Window::GetHandle();
        HDC hdc = GetDC( hwnd );
        
        memset( &m_devmode, 0L, sizeof(DEVMODE));
        m_devmode.dmSize = sizeof(DEVMODE);

        int maxRefresh = 0;
        int i,j;
        for ( i=0,j=0;; ++i)
        {
            if ( !::EnumDisplaySettings( 0, ++i, &m_devmode ))
            {
                break;
            }
            if ( m_devmode.dmBitsPerPel==bpp &&
                    m_devmode.dmPelsWidth==width &&
                    m_devmode.dmPelsHeight==height )
            {
                if ( m_devmode.dmDisplayFrequency == refresh )
                {
                    break;
                }

                if ( m_devmode.dmDisplayFrequency > maxRefresh )
                {
                    maxRefresh = m_devmode.dmDisplayFrequency;
                    j=i;
                }
            }
        }
        ::EnumDisplaySettings( 0, j, &m_devmode );
        m_devmode.dmFields |= DM_DISPLAYFREQUENCY;
    }

    void WGLScreen::InitDDraw( int width, int height, int bpp, float refresh )
    {
        HWND hwnd = Window::GetHandle();
        HDC hdc = GetDC( hwnd );
        
        DirectDrawCreateEx( NULL, (void**)&G_pDDraw, IID_IDirectDraw7, NULL);
        SetWindowLong( hwnd, GWL_STYLE, WS_POPUP);
        G_pDDraw->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
        G_pDDraw->SetDisplayMode( width, height, bpp, refresh, 0);
    }

    void WGLScreen::SlotFullscreenActive()
    {
        if ( m_active )
        {
            return;
        }
        m_active = true;
        if ( !m_windowed && !G_EnableDDrawScr )
        {
            Window::Resize( m_prop.width, m_prop.height );
            Window::Reposition( 0, 0);

            long res = ChangeDisplaySettingsEx( 0, &m_devmode, 0, CDS_FULLSCREEN, 0);
            switch(res)
            {
                case DISP_CHANGE_SUCCESSFUL: break;
                case DISP_CHANGE_BADFLAGS: WGLERR("DISP_CHANGE_BADFLAGS"); break;
                case DISP_CHANGE_BADMODE: WGLERR("DISP_CHANGE_BADMODE"); break;
                case DISP_CHANGE_BADPARAM: WGLERR("DISP_CHANGE_BADPARAM"); break;
                case DISP_CHANGE_FAILED: WGLERR("DISP_CHANGE_FAILED"); break;
                case DISP_CHANGE_RESTART: WGLERR("DISP_CHANGE_RESTART"); break;
                default: WGLERR("Unknown reason."); break;
            };
        }
    }

    void WGLScreen::SlotFullscreenInactive()
    {
        if ( !m_active )
        {
            return;
        }
        m_active = false;
        if ( !m_windowed && !G_EnableDDrawScr )
        {
            Minimize();
            ::ChangeDisplaySettingsEx( NULL, NULL, NULL, 0, NULL );
        }
    }
            
    void WGLScreen::SlotResize( int w, int h )
    {
        if ( w > 0 && h > 0 )
        {
            _glContextSurface->SetSize( w, h );
            _glContextSurface->GetProperties( &m_prop );
        }
    }

    void WGLScreen::Close()
    {
        if ( !!_glContextSurface )
        {
            HDC hdc = _glContextSurface->_devContext;
            _glContextSurface.Reset();
            ::ReleaseDC( GetHandle(), hdc );
        }

        if ( 0 != G_pDDraw )
        {
            G_pDDraw->Release();
            G_pDDraw = 0;
        }
    }

    void WGLScreen::Swap()
    {
        if ( m_windowed || m_active )
        {
            SwapBuffers( _glContextSurface->_devContext );
            _glContextSurface->GLFreeGarbage();
            GLUpdateMovieSamples( _glContextSurface.get() );
        }
    }

    void WGLScreen::Copy()
    {
        if ( m_windowed || m_active )
        {
            SwapBuffers( _glContextSurface->_devContext );
        }
    }

    Point<double> WGLScreen::GetZoomFactors() const
    {
        return Point<double>(1,1);
    }

    void WGLScreen::Clear( NormalPixel color, const Useless::Rect &area )
    {
        if ( m_windowed || m_active )
        {
            _glContextSurface->Clear( color.u, area );
        }
    }

    void WGLScreen::SetClipper( const RectList &rect_list )
    {
        if ( m_windowed || m_active )
        {
            _glContextSurface->SetClipper( rect_list );
        }
    }

    RectList WGLScreen::GetClipper( const Rect &crop ) const
    {
        if ( m_windowed || m_active )
        {
            return _glContextSurface->GetClipper( crop);
        }
        else
        {
            return RectList();
        }
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
        if ( !m_windowed && !m_active )
        {
            return;
        }

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
