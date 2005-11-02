#include "UselessPch.h"

#include "GDIPrinter.h"
#include "Useless/Graphic/Planes/IGraphics.h"
#include "Useless/Graphic/Planes/MemSurface.h"
#include "Useless/Graphic/Device/Surface.h"

namespace Useless {
    
    GDIPage::GDIPage( ::HDC hdc ): m_hdc( hdc )
    {
        ::StartPage( m_hdc );
    }

    GDIPage::~GDIPage()
    {
    }

    void GDIPage::BlitImage( const Rect &onPaper, IGraphics &image )
    {
        const Surface *pS = image.GetSurface();
        if ( !pS )
        {
            image.Cooperate( MemSurface() );
            pS = image.GetSurface();
        }
        SPointer< PixelTransfer > pT = pS->CreateReader( Surface::COLOR );
        if ( !pT )
        {
            image.Cooperate( MemSurface() );
            pT = pS->CreateReader( Surface::COLOR );
        }
        int width, height, pixFmt, numColors;
        const void *palette;
        int bytesPerPel = 3;
        int outPixFmt = ImageFormat::R8G8B8;
        pT->GetSource( &width, &height, &pixFmt, &palette, &numColors );
        pT->SetDestination( width, height, bytesPerPel * width, outPixFmt );
        pT->Transform();
        Rect sR = image.GetRect();
        char *data = new char[ sR.GetW() * sR.GetH() * bytesPerPel + sizeof( ::BITMAPINFOHEADER ) ];
        ::BITMAPINFO *bitmap = (::BITMAPINFO*)(data);
        bitmap->bmiHeader.biSize = sizeof( ::BITMAPINFOHEADER );
        bitmap->bmiHeader.biWidth = sR.GetW();
        bitmap->bmiHeader.biHeight = sR.GetH();
        bitmap->bmiHeader.biPlanes = 1;
        bitmap->bmiHeader.biBitCount = 24;
        bitmap->bmiHeader.biCompression = BI_RGB;
        bitmap->bmiHeader.biSizeImage = sR.GetW() * sR.GetH() * bytesPerPel;
        bitmap->bmiHeader.biXPelsPerMeter = 0;
        bitmap->bmiHeader.biYPelsPerMeter = 0;
        bitmap->bmiHeader.biClrUsed = 0;
        bitmap->bmiHeader.biClrImportant = 0;
        pT->Store( bitmap->bmiColors, sR );
    
        ::StretchDIBits( m_hdc, onPaper.GetX(), onPaper.GetY(), onPaper.GetW(), onPaper.GetH(),
            0, 0, bitmap->bmiHeader.biWidth, bitmap->bmiHeader.biHeight, bitmap->bmiColors, bitmap, DIB_RGB_COLORS, SRCCOPY );

        delete data;
    }

    void GDIPage::Finish()
    {
        ::EndPage( m_hdc );
    }
    
    GDIDocument::GDIDocument( ::HDC hdc, const std::basic_string< wchar_t > &title ): m_hdc( hdc )
    {
        std::memset( &m_doc, 0, sizeof( ::DOCINFO ));
        m_doc.cbSize = sizeof( ::DOCINFO );
        m_doc.lpszDocName = title.c_str();
        m_doc.lpszOutput = NULL;
        m_doc.lpszDatatype = NULL;
        m_doc.fwType = 0;
        ::StartDoc( m_hdc, &m_doc );
    }

    GDIDocument::~GDIDocument()
    {
    }

    SPointer< GDIPage > GDIDocument::CreateGDIPage()
    {
        return new GDIPage( m_hdc );
    }

    void GDIDocument::Print()
    {
        ::EndDoc( m_hdc );
    }
        
    SPointer< GDIDocument > GDIPrinter::CreateGDIDocument( const std::basic_string< wchar_t > &title )
    {
        GDIDocument *pDoc = new GDIDocument( m_hdc, title );
        return pDoc;
    }

    GDIPrinter::GDIPrinter( const GDIPrinterInfo &info ): m_info( info ), m_hdc(0)
    {
        ::DEVMODE devmode;
        std::memset( &devmode, 0, sizeof( ::DEVMODE ));
        devmode.dmSize = sizeof( ::DEVMODE );
        devmode.dmFields = DM_PAPERSIZE;
        devmode.dmPaperSize = DMPAPER_A4;
        m_hdc = ::CreateDC( info.m_driver.c_str(), m_info.m_name.c_str(), NULL, &devmode );
        GDIPrintManager::Instance().RegisterGDIPrinter( this );
        
        m_sizeOnPaperX = ::GetDeviceCaps( m_hdc, HORZRES );
        m_sizeOnPaperY = ::GetDeviceCaps( m_hdc, VERTRES );
        m_xLeft = ::GetDeviceCaps( m_hdc, PHYSICALOFFSETX );
        m_yTop = ::GetDeviceCaps( m_hdc, PHYSICALOFFSETY );
        m_inchOnPaperX = ::GetDeviceCaps( m_hdc, LOGPIXELSX );
        m_inchOnPaperY = ::GetDeviceCaps( m_hdc, LOGPIXELSY );
        m_sizeInchesX = m_sizeOnPaperX / (float)m_inchOnPaperX;
        m_sizeInchesY = m_sizeOnPaperY / (float)m_inchOnPaperY;
  
    }

    GDIPrinter::~GDIPrinter()
    {
        GDIPrintManager::Instance().UnregisterGDIPrinter( this );
        ::DeleteDC( m_hdc );
    }

        
    ::BOOL GDIPrinter::OnAbort( int iError )
    {
        return TRUE;
    }


    GDIPrintManager::GDIPrintManager()
    {
        std::vector< ::BYTE > printerEnum;
        ::DWORD cbNeeded1, cbNeeded2;
        ::DWORD cReturned1, cReturned2;

        ::EnumPrinters( PRINTER_ENUM_NAME, NULL, 2, NULL, 0, &cbNeeded1, &cReturned1 );
        ::EnumPrinters( PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL, 0, &cbNeeded2, &cReturned2 );

        printerEnum.resize( cbNeeded1 + cbNeeded2 );
        ::PRINTER_INFO_2 *pInfo1 = (::PRINTER_INFO_2 *)&printerEnum[0];
        ::PRINTER_INFO_2 *pInfo2 = (::PRINTER_INFO_2 *)&printerEnum[cbNeeded1];

        ::EnumPrinters( PRINTER_ENUM_NAME, NULL, 2, &printerEnum[0], cbNeeded1, &cbNeeded1, &cReturned1 );
        ::EnumPrinters( PRINTER_ENUM_CONNECTIONS, NULL, 2, &printerEnum[cbNeeded1], cbNeeded2, &cbNeeded2, &cReturned2 );

        m_printers.resize( cReturned1 + cReturned2 );

        for ( int i=0; i < cReturned1; ++i )
        {
            GDIPrinterInfo &info = m_printers[ i ];
            info.m_name = pInfo1[i].pPrinterName;
            info.m_port = pInfo1[i].pPortName;
            info.m_driver = pInfo1[i].pDriverName;
            info.m_local = true;
        }

        for ( int i=0; i < cReturned2; ++i )
        {
            GDIPrinterInfo &info = m_printers[ cReturned1 + i ];
            info.m_name = pInfo2[i].pPrinterName;
            info.m_port = pInfo2[i].pPortName;
            info.m_driver = pInfo2[i].pDriverName;
            info.m_local = false;
        }

    }

    GDIPrintManager::~GDIPrintManager()
    {
    }

    ::BOOL GDIPrintManager::AbortProc( ::HDC hdc, int iError )
    {
        for ( std::set< GDIPrinter * >::iterator it = GDIPrintManager::Instance().m_printerInstances.begin();
                it != GDIPrintManager::Instance().m_printerInstances.end(); ++it )
        {
            if ( (*it)->m_hdc == hdc )
            {
                (*it)->OnAbort( iError );
            }
        }
        return TRUE;
    }

    void GDIPrintManager::RegisterGDIPrinter( GDIPrinter *printer )
    {
        m_printerInstances.insert( printer );
        //@TODO: setting abort proc one causes int StartDoc read violation 
        //SetAbortProc( printer->m_hdc, (ABORTPROC)&GDIPrintManager::AbortProc );
    }

    void GDIPrintManager::UnregisterGDIPrinter( GDIPrinter *printer )
    {
        std::set< GDIPrinter * >::iterator it = m_printerInstances.find( printer );
        if ( it != m_printerInstances.end() )
        {
            m_printerInstances.erase( it );
        }
    }

    GDIPrintManager & GDIPrintManager::Instance()
    {
        static GDIPrintManager s_manager;
        return s_manager;
    }

    SPointer< GDIPrinter > CreateGDIPrinter()
    {
        using namespace Useless;
        GDIPrinter *pPrinter = 0;

        for ( GDIPrintManager::const_iterator it = GDIPrintManager::Instance().begin();
                it != GDIPrintManager::Instance().end(); ++it )
        {
            if ( (*it).m_port != TEXT("MSFAX:") )
            {
                pPrinter = new GDIPrinter( *it );
                break;
            }
        }
        return pPrinter;
    }

};//Useless

