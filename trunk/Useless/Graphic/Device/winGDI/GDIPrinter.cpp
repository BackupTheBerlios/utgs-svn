#include "UselessPch.h"

#include "GDIPrinter.h"
#include "Useless/Graphic/Planes/IGraphics.h"
#include "Useless/Graphic/Planes/MemSurface.h"
#include "Useless/Graphic/Device/Surface.h"
#include <stdio.h>

namespace Useless {

    namespace {
        LogError( FILE *fDump )
        {
            LPVOID lpMsgBuf;
            FormatMessage( 
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                    FORMAT_MESSAGE_FROM_SYSTEM | 
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR) &lpMsgBuf,
                    0,
                    NULL 
                    );
            _ftprintf( fDump, _T("Error: %s\n"), lpMsgBuf );
            LocalFree( lpMsgBuf );
        }
    };//unnamed
    
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
        FILE *fDump = fopen("Printers.log", "a+");
        _ftprintf( fDump, _T("[Printer]\n") );
        
        ::DEVMODE devmode;
        std::memset( &devmode, 0, sizeof( ::DEVMODE ));
        devmode.dmSize = sizeof( ::DEVMODE );
        devmode.dmFields = DM_PAPERSIZE;
        devmode.dmPaperSize = DMPAPER_A4;

        while( true )
        {
#define USELESS_GDI_PRINTER_CREATE_DC( _Driver, _Name, _Devmode ) \
            _ftprintf( fDump, _T("CreateDC( %s, %s, NULL, %0x )\n"), _Driver, _Name, _Devmode ); \
            m_hdc = ::CreateDC( _Driver, _Name, NULL, _Devmode ); \
            if ( !!m_hdc ) { break; } \
            LogError( fDump );

            USELESS_GDI_PRINTER_CREATE_DC( m_info.m_driver.c_str(), m_info.m_name.c_str(), &devmode );
            USELESS_GDI_PRINTER_CREATE_DC( _T("WINSPOOL"), m_info.m_name.c_str(), &devmode );
            USELESS_GDI_PRINTER_CREATE_DC( NULL, m_info.m_name.c_str(), &devmode );
            
            USELESS_GDI_PRINTER_CREATE_DC( m_info.m_driver.c_str(), m_info.m_name.c_str(), NULL );
            USELESS_GDI_PRINTER_CREATE_DC( _T("WINSPOOL"), m_info.m_name.c_str(), NULL );
            USELESS_GDI_PRINTER_CREATE_DC( NULL, m_info.m_name.c_str(), NULL );
        }
        
        GDIPrintManager::Instance().RegisterGDIPrinter( this );
        
        m_sizeOnPaperX = ::GetDeviceCaps( m_hdc, HORZRES );
        m_sizeOnPaperY = ::GetDeviceCaps( m_hdc, VERTRES );
        m_xLeft = ::GetDeviceCaps( m_hdc, PHYSICALOFFSETX );
        m_yTop = ::GetDeviceCaps( m_hdc, PHYSICALOFFSETY );
        m_inchOnPaperX = ::GetDeviceCaps( m_hdc, LOGPIXELSX );
        m_inchOnPaperY = ::GetDeviceCaps( m_hdc, LOGPIXELSY );
        m_sizeInchesX = m_sizeOnPaperX / (float)m_inchOnPaperX;
        m_sizeInchesY = m_sizeOnPaperY / (float)m_inchOnPaperY;
        
        _ftprintf( fDump, _T("m_info = {Printer: %s; Port: %s; Driver: %s; Local: %s}\n"),
                m_info.m_name.c_str(), m_info.m_port.c_str(), m_info.m_driver.c_str(),
                (m_info.m_local ? _T("TRUE") : _T("FALSE") ));
        _ftprintf( fDump, _T("m_hdc = %0x\n"), m_hdc );
        _ftprintf( fDump, _T("m_sizeOnPaperX = %d\n"), m_sizeOnPaperX );
        _ftprintf( fDump, _T("m_sizeOnPaperY = %d\n"), m_sizeOnPaperY );
        _ftprintf( fDump, _T("m_xLeft = %d\n"), m_xLeft );
        _ftprintf( fDump, _T("m_yTop = %d\n"), m_yTop );
        _ftprintf( fDump, _T("m_inchOnPaperX = %d\n"), m_inchOnPaperX );
        _ftprintf( fDump, _T("m_inchOnPaperY = %d\n"), m_inchOnPaperY );
        _ftprintf( fDump, _T("m_sizeInchesX = %f\n"), m_sizeInchesX );
        _ftprintf( fDump, _T("m_sizeInchesY = %f\n"), m_sizeInchesY );
        
        fclose( fDump );
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

        ::EnumPrinters( PRINTER_ENUM_LOCAL, NULL, 2, NULL, 0, &cbNeeded1, &cReturned1 );
        ::EnumPrinters( PRINTER_ENUM_CONNECTIONS, NULL, 2, NULL, 0, &cbNeeded2, &cReturned2 );

        printerEnum.resize( cbNeeded1 + cbNeeded2 );
        ::PRINTER_INFO_2 *pInfo1 = (::PRINTER_INFO_2 *)&printerEnum[0];
        ::PRINTER_INFO_2 *pInfo2 = (::PRINTER_INFO_2 *)&printerEnum[cbNeeded1];

        ::EnumPrinters( PRINTER_ENUM_LOCAL, NULL, 2, &printerEnum[0], cbNeeded1, &cbNeeded1, &cReturned1 );
        ::EnumPrinters( PRINTER_ENUM_CONNECTIONS, NULL, 2, &printerEnum[cbNeeded1], cbNeeded2, &cbNeeded2, &cReturned2 );

        m_printers.resize( cReturned1 + cReturned2 );

        FILE *fDump = fopen("Printers.log", "a+");
        _ftprintf( fDump, _T("[Printers]\n") );

        for ( int i=0; i < cReturned1; ++i )
        {
            GDIPrinterInfo &info = m_printers[ i ];
            info.m_name = pInfo1[i].pPrinterName;
            info.m_port = pInfo1[i].pPortName;
            info.m_driver = pInfo1[i].pDriverName;
            info.m_local = true;
            _ftprintf( fDump, _T("Printer: %s; Port: %s; Driver: %s; Local: TRUE\n"),
                    info.m_name.c_str(), info.m_port.c_str(), info.m_driver.c_str() );
        }

        for ( int i=0; i < cReturned2; ++i )
        {
            GDIPrinterInfo &info = m_printers[ cReturned1 + i ];
            info.m_name = pInfo2[i].pPrinterName;
            info.m_port = pInfo2[i].pPortName;
            info.m_driver = pInfo2[i].pDriverName;
            info.m_local = false;
            _ftprintf( fDump, _T("Printer: %s Port: %s Driver: %s Local: FALSE\n"),
                    info.m_name.c_str(), info.m_port.c_str(), info.m_driver.c_str() );
        }

        fclose( fDump );

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

        FILE *fDump = fopen("Printers.log", "a+");
        TCHAR lpReturnedString[256];
        DWORD dwRes = GetProfileString( _T("windows"), _T("device"), _T("none"), lpReturnedString, 256 );
        _ftprintf( fDump, _T("windows.device = %s\n"), lpReturnedString );
        fclose( fDump );
        std::basic_string< TCHAR > strDefaultPrn( lpReturnedString );
        int comaName = strDefaultPrn.find(_T(","));
        int comaDrv = strDefaultPrn.find(_T(","), comaName+1 );
        GDIPrinterInfo prnInfo;
        prnInfo.m_name = strDefaultPrn.substr(0, comaName);
        prnInfo.m_driver = strDefaultPrn.substr( comaName+1, comaDrv-comaName-1 );
        prnInfo.m_port = strDefaultPrn.substr( comaDrv+1, -1 );
        pPrinter = new GDIPrinter( prnInfo );
        if ( 0 != pPrinter->m_hdc )
        {
            return pPrinter;
        }
        delete pPrinter;

        for ( GDIPrintManager::const_iterator it = GDIPrintManager::Instance().begin();
                it != GDIPrintManager::Instance().end(); ++it )
        {
            if ( (*it).m_port != _T("MSFAX:") )
            {
                pPrinter = new GDIPrinter( *it );
                if ( 0 != pPrinter->m_hdc )
                {
                    break;
                }
                delete pPrinter;
            }
        }
        return pPrinter;
    }

};//Useless

