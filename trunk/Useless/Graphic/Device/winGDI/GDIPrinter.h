#ifndef __INCLUDED_GDI_PRINTER_H__
#define __INCLUDED_GDI_PRINTER_H__

#include "Useless/Util/SPointer.h"
#include "Useless/Graphic/Rect.h"
#include <windows.h>
#include <vector>
#include <set>
#include <string>
#include <assert.h>

namespace Useless {

    struct GDIPage
    {
        GDIPage( ::HDC hdc );
        ~GDIPage();

        void BlitImage( const Rect &onPaper, class IGraphics &image );
        void Finish();

        ::HDC m_hdc;
    };

    struct GDIDocument
    {
        GDIDocument( ::HDC hdc, const std::basic_string< wchar_t > &title );
        ~GDIDocument();

        SPointer< GDIPage > CreateGDIPage();
        void Print();

        ::DOCINFO m_doc;
        ::HDC     m_hdc;
    };

    struct GDIPrinterInfo
    {
        std::basic_string< ::TCHAR > m_name;
        std::basic_string< ::TCHAR > m_port;
        std::basic_string< ::TCHAR > m_driver;
        bool                         m_local;
    };

    struct GDIPrinter
    {
        GDIPrinter( const GDIPrinterInfo &info );
        ~GDIPrinter();

        ::BOOL OnAbort( int iError );

        SPointer< GDIDocument > CreateGDIDocument( const std::basic_string< wchar_t > &title );

        GDIPrinterInfo m_info;
        ::HDC          m_hdc;
        
        int m_sizeOnPaperX;
        int m_sizeOnPaperY;
        int m_xLeft;
        int m_yTop;
        int m_inchOnPaperX;
        int m_inchOnPaperY;
        float m_sizeInchesX;
        float m_sizeInchesY;
    };

    struct GDIPrintManager
    {
        protected:
            GDIPrintManager();
            ~GDIPrintManager();

            static ::BOOL AbortProc( ::HDC hdc, int iError );

            std::vector< GDIPrinterInfo >  m_printers;
            std::set< GDIPrinter * >       m_printerInstances;

            friend class GDIPrinter;
            void RegisterGDIPrinter( GDIPrinter *printer );
            void UnregisterGDIPrinter( GDIPrinter *printer );

        public:
            typedef std::vector< GDIPrinterInfo >::const_iterator const_iterator;
            const_iterator begin() const { return m_printers.begin(); }
            const_iterator end() const { return m_printers.end(); }

            static GDIPrintManager & Instance();
    };

    SPointer< GDIPrinter > CreateGDIPrinter();

};//Useless

#endif//__INCLUDED_GDI_PRINTER_H__
