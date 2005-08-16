#include <koolib/xml/XMLChunksUseless.h>
#include <gl/GL.h> //@TODO: needed for PainterProxy::SubCanvasPaint
#include "Useless/Graphic/Planes/MemSurface.h" //needed by QueryPixelColor

namespace XMLProgram {

    using namespace Useless;
    using namespace XMLFactory;


    /*******************************

      PointListProxy

    */
    PointListProxy::PointListProxy( const Useless::PointList &points ): _points( points )
    {
        add_methods( this )
            .def("AddPoint", AddPoint, "pos")
            .def("Size", Size)
            .def("At", At, "index")
            ;
    }

    PointListProxy::~PointListProxy()
    {
    }
        
    void PointListProxy::AddPoint( const Useless::Pos &pos )
    {
        _points.push_back( pos );
    }
    
    int  PointListProxy::Size()
    {
        return _points.size();
    }
    
    Useless::Pos PointListProxy::At( int index )
    {
        if ( index < 0 ) { throw Error("At: invalid index"); }
        if ( index >= _points.size() ) { throw Error("At: index exceeds array"); }
        return _points[ index ];
    }


    /*******************************

      RectListProxy

    */
    RectListProxy::RectListProxy( const Useless::RectList &rects ): _rects( rects )
    {
        add_methods( this )
            .def("AddRect", AddRect, "rect")
            .def("Size", Size)
            .def("At", At, "index")
            ;
    }

    RectListProxy::~RectListProxy()
    {
    }
        
    void RectListProxy::AddRect( const Useless::Rect &rect )
    {
        _rects.push_back( rect );
    }
    
    int  RectListProxy::Size()
    {
        return _rects.size();
    }
    
    Useless::Rect RectListProxy::At( int index )
    {
        if ( index < 0 ) { throw Error("At: invalid index"); }
        if ( index >= _rects.size() ) { throw Error("At: index exceeds array"); }
        return _rects[ index ];
    }


    /*******************************

      ScreenProxy

    */
    ScreenProxy::ScreenProxy( SPointer< Screen > screen ): _spScreen( screen )
    {
        add_methods( _spScreen.get(), this )
            .def("Refresh", &Screen::Refresh )
            .def("Show", &Screen::Show )
            .def("Hide", &Screen::Hide )
            .def("Maximize", &Screen::Maximize )
            .def("Minimize", &Screen::Minimize )
            .def("Restore", &Screen::Restore )
            .def("ShowCursor", &Screen::ShowCursor )
            .def("HideCursor", &Screen::HideCursor )
            .def("GrabFocus", &Screen::GrabFocus )
            .def("InterceptCursor", &Screen::InterceptCursor )
            .def("ReleaseCursor", &Screen::ReleaseCursor )
            .def("SetCursorPos", &Screen::SetCursorPos, "x", "y" )
            .def("GetCursorPos", unconst_method_cast( &Screen::GetCursorPos ))
            .def("Reposition", &Screen::Reposition, "x", "y" )
            .def("Resize", &Screen::Resize, "w", "h" )
            .def("ShowDecorations", &Screen::ShowDecorations )
            .def("HideDecorations", &Screen::HideDecorations )
            .def("GetWindowRect", unconst_method_cast( &Screen::GetWindowRect ))
            .def("GetClientRect", unconst_method_cast( &Screen::GetClientRect ))
            .def("AllowSizing", &Screen::AllowSizing, "allow" )
            .def("NoSizing", unconst_method_cast( &Screen::NoSizing ))
            .def("AllowClear", &Screen::AllowClear, "allow" )
            .def("NoClear", unconst_method_cast( &Screen::NoClear ))
            .def("AllowCursorWrapping", &Screen::AllowCursorWrapping, "allow" )
            .def("NoCursorWrapping", unconst_method_cast( &Screen::NoCursorWrapping ))
            .def("GetTitle", unconst_method_cast( &Screen::GetTitle ))
            // signals
            .add("OnCreate", make_signal_0_chunk( _spScreen->OnCreate ))
            .add("OnDestroy", make_signal_0_chunk( _spScreen->OnDestroy ))
            .add("OnActivate", make_signal_0_chunk( _spScreen->OnActivate ))
            .add("OnDeactivate", make_signal_0_chunk( _spScreen->OnDeactivate ))
            .add("OnPaint", make_signal_0_chunk( _spScreen->OnPaint ))
            .add("OnMove", make_signal_2_chunk( _spScreen->OnMove ))
            .add("OnResize", make_signal_2_chunk( _spScreen->OnResize ))
            .add("OnIdle", make_signal_2_chunk( _spScreen->OnIdle ))
            .add("OnKeyPress", make_signal_1_chunk( _spScreen->OnKeyPress ))
            .add("OnKeyRelease", make_signal_1_chunk( _spScreen->OnKeyRelease ))
            .add("OnChar", make_signal_1_chunk( _spScreen->OnChar ))
            .add("OnMouseBtn", make_signal_2_chunk( _spScreen->OnMouseBtn ))
            .add("OnDoubleClick", make_signal_1_chunk( _spScreen->OnDoubleClick ))
            .add("OnMouseMove", make_signal_2_chunk( _spScreen->OnMouseMove ))
            .add("OnMouseWheel", make_signal_2_chunk( _spScreen->OnMouseWheel ))
            ;
    }

    ScreenProxy::~ScreenProxy()
    {
    }
    

    /*******************************

      ImageProxy

    */
    ImageProxy::ImageProxy( SPointer< ImageBase > spImage ): _spImage( spImage )
    {
        add_methods( _spImage.get(), this )
            .def("GetWidth", unconst_method_cast( &ImageBase::GetWidth ))
            .def("GetHeight", unconst_method_cast( &ImageBase::GetHeight ))
            .def("GetDestWidth", unconst_method_cast( &ImageBase::GetDestWidth ))
            .def("GetDestHeight", unconst_method_cast( &ImageBase::GetDestHeight ))
            .def("SetDestWidth", &ImageBase::SetDestWidth, "width" )
            .def("SetDestHeight", &ImageBase::SetDestHeight, "height" )
            ;

        add_methods( this )
            .def("QueryPixelColor", QueryPixelColor, "x", "y" )
            ;
    }

    ImageProxy::~ImageProxy()
    {
    }
    
    IChunkPtr ImageProxy::QueryPixelColor( int x, int y )
    {
        const Surface *pSurface = _spImage->GetSurface();
        // Validate surface
        if ( 0 == dynamic_cast< const MemSurface *>( pSurface )) // - @TODO: CreateReader is not supported by GLTextureSurface - so we use MemSurface
        {
            _spImage->Cooperate( MemSurface() );
            pSurface = _spImage->GetSurface();
        }
        int red, green, blue, alpha;
        {   // Read COLOR
            SPointer< PixelTransfer > pixTrans = pSurface->CreateReader( Surface::COLOR );
            pixTrans->SetDestination( _spImage->GetWidth(), _spImage->GetHeight(), 1, 32 );
            pixTrans->Transform();
            NormalPixel data;
            pixTrans->Store( (unsigned char *)&data, Rect( x, y, 1, 1 ));
            red = data.c.r;
            green = data.c.g;
            blue = data.c.b;
            // Read ALPHA
            SPointer< PixelTransfer > pixTrans1 = pSurface->CreateReader( Surface::ALPHA );
            pixTrans1->SetDestination( _spImage->GetWidth(), _spImage->GetHeight(), 1, 8 );
            pixTrans1->Transform();
            unsigned char data1;
            pixTrans1->Store( &data1, Rect( x, y, 1, 1 ));
            alpha = data1;
        }
        IBlockPtr block = XMLProgram::CreateBlock();
        block->AddChunk(L"red", XMLProgram::CreateValue( red ));
        block->AddChunk(L"green", XMLProgram::CreateValue( green ));
        block->AddChunk(L"blue", XMLProgram::CreateValue( blue ));
        block->AddChunk(L"alpha", XMLProgram::CreateValue( alpha ));
        return block.get();
    }
    
    
    /*******************************

      SampleProxy

    */
    SampleProxy::SampleProxy( Useless::SPointer< Useless::Sample > spSample ): _spSample( spSample )
    {
        add_methods( this )
            .def("GetDuration", GetDuration )
            ;
    }

    SampleProxy::~SampleProxy()
    {
    }

    float SampleProxy::GetDuration()
    {
        Snd::Properties p;
        _spSample->GetProperties( p );
        return (p.length * p.sample_rate);
    }
    
    
    
    /*******************************

      ChannelProxy

    */
    ChannelProxy::ChannelProxy( const Useless::Channel &channel ): _channel( channel )
    {
        add_methods( this )
            .def("Play", Play)
            .def("LoopPlay", LoopPlay)
            .def("Stop", Stop)
            .def("SetVolume", SetVolume, "volume")
            .def("SetPan", SetPan, "pan")
            .def("IsPlaying", IsPlaying )
            ;
    }

    ChannelProxy::~ChannelProxy()
    {
    }
        
    void ChannelProxy::Play()
    {
        _channel.Play( false );
    }

    void ChannelProxy::LoopPlay()
    {
        _channel.Play( true );
    }

    void ChannelProxy::Stop()
    {
        _channel.Stop();
    }

    void ChannelProxy::SetVolume( int volume )
    {
        _channel.SetVolume( volume );
    }

    void ChannelProxy::SetPan( int pan )
    {
        _channel.SetPan( pan );
    }

    int  ChannelProxy::IsPlaying()
    {
        return _channel.IsPlaying();
    }
    
#ifdef USELESS_HAS_DSHOW    
    /*******************************

      DSMediaProxy

    */

    DSMediaProxy::DSMediaProxy( Useless::SPointer< Useless::DSMedia > spMedia ): _spMedia( spMedia )
    {
        add_methods( _spMedia.get(), this )
            .def("Play", &DSMedia::Play )
            .def("Pause", &DSMedia::Pause )
            .def("Stop", &DSMedia::Stop )
            .def("IsPlaying", &DSMedia::IsPlaying )
            .def("IsPaused", &DSMedia::IsPaused )
            .def("IsVideo", unconst_method_cast( &DSMedia::IsVideo ))
            .def("IsAudio", unconst_method_cast( &DSMedia::IsAudio ))
            .def("GetTime", unconst_method_cast( &DSMedia::GetTime ))
            .def("GetDuration", unconst_method_cast( &DSMedia::GetDuration ))
            .def("SetTime", &DSMedia::SetTime, "time")
            .def("GetFPS", unconst_method_cast( &DSMedia::GetFPS ))
            ;

        add_methods( this )
            .def("IsFinished", IsFinished )
            .def("GetImage", GetImage )
            .def("SetVolume", SetVolume, "volume" )
            ;
    }

    DSMediaProxy::~DSMediaProxy()
    {
    }

    IChunkPtr DSMediaProxy::GetImage()
    {
        return new ImageProxy( new ImageBuffer( _spMedia->_surface ));
    }

    void DSMediaProxy::SetVolume( int volume )
    {
        Snd::PlayFX fx;
        fx.volume = volume;
        _spMedia->_sample_data->SetFX( fx );
    }

    bool DSMediaProxy::IsFinished()
    {
        return ( _spMedia->IsPlaying() && ( _spMedia->GetTime() >= _spMedia->GetDuration() - 0.01 ));
    }
#endif

#ifdef USELESS_HAS_VORBIS    
    /*******************************

      OggVorbisProxy

    */

    OggVorbisProxy::OggVorbisProxy( Useless::SPointer< Useless::IFile > file, Useless::SPointer< Useless::SoundCard > card )
    {
        _file = file;
        _spOggVorbis = new OggVorbisDecoder( *_file, *card );

        add_methods( _spOggVorbis.get(), this )
            .def("Play", &OggVorbisDecoder::Play )
            .def("Stop", &OggVorbisDecoder::Stop )
            .def("IsFinished", unconst_method_cast( &OggVorbisDecoder::EoS ))
            ;

        add_methods( this )
            .def("GetChannel", GetChannel )
            ;
    }

    OggVorbisProxy::~OggVorbisProxy()
    {
    }
    
    IChunkPtr OggVorbisProxy::GetChannel()
    {
        return new ChannelProxy( _spOggVorbis->GetChannel() );
    }
#endif


    /*******************************

      PainterProxy

    */

    PainterProxy::PainterProxy( const WidgetPainter &painter ):_painter( painter )
    {
        add_methods( this )
            .def("RepaintWidget", RepaintWidget)
            .def("Blit", Blit)
            .def("FastBlit", FastBlit)
            .def("FastDrawPolygon", FastDrawPolygon )
            .def("FastMultiBlit", FastMultiBlit )
            .def("FillRect", FillRect)
            .def("DrawRect", DrawRect)
            .def("DrawLine", DrawLine)
            .def("DrawPolygon", DrawPolygon)
            .def("MultiBlit", MultiBlit)
            .def("SubCanvasPaint", SubCanvasPaint)
            ;
    }

    PainterProxy::~PainterProxy()
    {
    }

    void PainterProxy::RepaintWidget()
    {
        Widget *widget = _painter.GetWidget();
        dynamic_cast_call< CompoundWidget >( widget, &CompoundWidget::DoRepaint, _painter );
    }

    void PainterProxy::FastBlit( Node __unused__, ExecutionState &_state )
    {
        IChunk *pImage =_state._currentBlock->GetChunk( L"Imag" );
        if ( !pImage ) { throw Error("FastBlit: expects 'Imag'"); }
        ImageProxy *pImageProx = dynamic_cast< ImageProxy *>( pImage );
        if ( !pImageProx ) { throw Error("FastBlit: expects 'Imag' to be type of Image");}            
        IChunk *pCanv = _state._currentBlock->GetChunk( L"Canv" );
        IChunk *pTexc = _state._currentBlock->GetChunk( L"Texc" );
        SPointer< ImageBase > spImg = pImageProx->_spImage;
        if ( 0 != pTexc )
        {
            int x = attribute_of< int >( pTexc, L"x" );
            int y = attribute_of< int >( pTexc, L"y" );
            int w = attribute_of< int >( pTexc, L"w" );
            int h = attribute_of< int >( pTexc, L"h" );
            SPointer< IGraphics > cp = spImg->Copy( Rect(x, y, w, h));
            spImg.Reset( dynamic_cast< ImageBase *>( cp.Release() ));
        }
        if ( !pCanv ) { throw Error("FastBlit: expects 'Canv'");}            
        int x = attribute_of< int >( pCanv, L"x" );
        int y = attribute_of< int >( pCanv, L"y" );
        int w = attribute_of< int >( pCanv, L"w" );
        int h = attribute_of< int >( pCanv, L"h" );
        spImg->SetDestWidth( w );
        spImg->SetDestHeight( h );
        _painter.ProjectImage( x, y, *spImg );
    }
    
    void PainterProxy::FastDrawPolygon( Node __unused__, ExecutionState &state )
    {
        IChunk *pColor = state._currentBlock->GetChunk(L"color");
        if ( !pColor ) { throw Error("FastDrawPolygon: expects 'color'"); }
        IChunk *pVertices = state._currentBlock->GetChunk(L"vertices");
        if ( !pVertices ) { throw Error("FastDrawPolygon: expects 'vertices'"); }
        PointListProxy *vertices = dynamic_cast< PointListProxy *>( pVertices );
        if ( !vertices ) { throw Error("FastDrawPolygon: excpects 'vertices' to be type of PointList"); }
        _painter.PaintPolygon( vertices->_points, value_of< int >( pColor ));
    }

    void PainterProxy::FastMultiBlit( Node __unused__, ExecutionState &state )
    {
        IChunk *pImage = state._currentBlock->GetChunk( L"Imag" );
        if ( !pImage ) { throw Error("FastMultiBlit: expects 'Imag'"); }
        ImageProxy *pImageProx = dynamic_cast< ImageProxy *>( pImage );
        if ( !pImageProx ) { throw Error("FastMultiBlit: expects 'Imag' to be type of Image");}            
        IChunk *pPositions = state._currentBlock->GetChunk(L"positions");
        if ( !pPositions ) { throw Error("FastMultiBlit: expects 'positions'"); }
        PointListProxy *pPointList = dynamic_cast< PointListProxy *>( pPositions );
        if ( !pPointList ) { throw Error("FastMultiBlit: expects 'positions' to be type of PointList"); }        
        IChunk *pRects = state._currentBlock->GetChunk(L"rects");
        if ( !pRects ) { throw Error("FastMultiBlit: expects 'rects'"); }
        RectListProxy *pRectList = dynamic_cast< RectListProxy *>( pRects );
        if ( !pRectList ) { throw Error("FastMultiBlit: expects 'rects' to be type of RectList"); }
        _painter.ProjectImages( *pImageProx->_spImage, pPointList->_points, pRectList->_rects );

    }

    void PainterProxy::Blit( Node _node, ExecutionState &_state )
    {
        Attr< TextAnsi, true, wchar_t > _img(L"img");
        Attr< int, false, wchar_t > _where(L"where");
        Attr< int, false, wchar_t > _from(L"from");

        GetAttr( _img, _node, _state );

        SPointer< ImageBase > img = MakeImage( get_value( _img ) );

        int x=0, y=0, w=0, h=0;

        if ( GetMultiAttr( _from, _node, _state ))
        {
            int cx=0, cy=0, cw=0, ch=0;
            cx = _from++;
            cy = _from++;
            cw = _from++;
            ch = _from++;
            SPointer< IGraphics > cp = img->Copy( Rect(cx, cy, cw, ch));
            img.Reset( dynamic_cast< ImageBase *>( cp.Release() ));
        }

        if ( GetMultiAttr( _where, _node, _state ))
        {
            x = _where++;
            y = _where++;
            if ( !!_where )
            {
                w = _where++;
                h = _where++;
                img->SetDestWidth( w );
                img->SetDestHeight( h );
            }
        }

        _painter.ProjectImage( x, y, *img );
    }

    void PainterProxy::FillRect( Node _node, ExecutionState &_state )
    {
        Attr< int, true, wchar_t > _where(L"where");
        Attr< Color, true, wchar_t >   _color(L"color");

        GetMultiAttr( _where, _node, _state );
        GetAttr( _color, _node, _state );

        int x = _where++;
        int y = _where++;
        int w = _where++;
        int h = _where++;

        _painter.PaintRectangle( _color->GetInteger(), Rect(x, y, w, h));
    }

    void PainterProxy::DrawRect( Node _node, ExecutionState &_state )
    {
        Attr< int, true, wchar_t >     _where(L"where");
        Attr< Color, true, wchar_t >   _color(L"color");

        GetMultiAttr( _where, _node, _state );
        GetAttr( _color, _node, _state );

        int x = _where++;
        int y = _where++;
        int w = _where++;
        int h = _where++;
        int c = _color->GetInteger();

        _painter.PaintBoundary( Rect(x, y, w, h), c, c, c, c);
    }

    void PainterProxy::DrawLine( Node _node, ExecutionState &_state )
    {
        Attr< int, true, wchar_t >     _from(L"from");
        Attr< int, true, wchar_t >     _to(L"to");
        Attr< Color, true, wchar_t >   _color(L"color");
        Attr< int, true, wchar_t >     _thickness(L"thickness");

        GetMultiAttr( _from, _node, _state );
        GetMultiAttr( _to, _node, _state );
        GetAttr( _color, _node, _state );
        GetAttr( _thickness, _node, _state );

        int x1 = _from++;
        int y1 = _from++;
        int x2 = _to++;
        int y2 = _to++;
        int c = _color->GetInteger();

        _painter.PaintLine( Pos(x1,y1), Pos(x2,y2), c, *_thickness );
    }

    void PainterProxy::DrawPolygon( Node _node, ExecutionState &_state )
    {
        Attr< Color, true, wchar_t > _color(L"color");

        GetAttr( _color, _node, _state );

        std::vector< Pos > vertices;
        IChunkPtr pObject = XMLProgram::GetChunk(L"vertices", _state);

        IChunk *pCurrent = pObject.get();
        while ( !IsEmpty( pCurrent ))
        {
            IChunk *pHead = pCurrent->GetChunk( FOURCC_LIST_HEAD );
            Attr< int, true, wchar_t > _x(L"x");
            Attr< int, true, wchar_t > _y(L"y");
            (*pHead) >> _x;
            (*pHead) >> _y;
            vertices.push_back( Pos( *_x, *_y ));
            pCurrent = pCurrent->GetChunk( FOURCC_LIST_TAIL );
        }

        _painter.PaintPolygon( vertices, _color->GetInteger() );
    }

    void PainterProxy::MultiBlit( Node _node, ExecutionState &_state )
    {
        Attr< TextAnsi, true, wchar_t > _img(L"img");

        GetAttr( _img, _node, _state );

        SPointer< ImageBase > img = MakeImage( get_value( _img ) );

        PointList positions;
        RectList  rects;

        IChunkPtr pObject1 = XMLProgram::GetChunk(L"positions", _state);
        IChunkPtr pObject2 = XMLProgram::GetChunk(L"rects", _state);

        IChunk *pCurrent1 = pObject1.get();
        IChunk *pCurrent2 = pObject2.get();

        while ( !IsEmpty( pCurrent1 ) && !IsEmpty( pCurrent2 ))
        {
            IChunk *pHead1 = pCurrent1->GetChunk( FOURCC_LIST_HEAD );
            IChunk *pHead2 = pCurrent2->GetChunk( FOURCC_LIST_HEAD );
            Attr< int, true, wchar_t > _x(L"x");
            Attr< int, true, wchar_t > _y(L"y");
            Attr< int, true, wchar_t > _cx(L"x");
            Attr< int, true, wchar_t > _cy(L"y");
            Attr< int, true, wchar_t > _cw(L"w");
            Attr< int, true, wchar_t > _ch(L"h");
            (*pHead1) >> _x;
            (*pHead1) >> _y;
            (*pHead2) >> _cx;
            (*pHead2) >> _cy;
            (*pHead2) >> _cw;
            (*pHead2) >> _ch;
            positions.push_back( Pos( *_x, *_y ));
            rects.push_back( Rect( *_cx, *_cy, *_cw, *_ch ));
            pCurrent1 = pCurrent1->GetChunk( FOURCC_LIST_TAIL );
            pCurrent2 = pCurrent2->GetChunk( FOURCC_LIST_TAIL );
        }

        _painter.ProjectImages( *img, positions, rects );
    }

    void PainterProxy::SubCanvasPaint( Node __unused__, ExecutionState &state )
    {
        IChunk *pFunc   = state._currentBlock->GetChunk(L"func");
        if ( !pFunc ) { throw Error("SubCanvasPaint: expects 'func'"); }
        IChunk *pOffset = state._currentBlock->GetChunk(L"offset");
        IChunk *pColor  = state._currentBlock->GetChunk(L"color");
        IChunk *pAlpha  = state._currentBlock->GetChunk(L"alpha");
        IChunk *pBlending = state._currentBlock->GetChunk(L"blending");
        IChunk *pCrop   = state._currentBlock->GetChunk(L"crop");

        Pos offset(0,0);
        if ( pOffset ) {
            offset.x = value_of< int >( pOffset->GetChunk(L"x") );
            offset.y = value_of< int >( pOffset->GetChunk(L"y") );
        }

        PainterProxy *pProxy1 = new PainterProxy( WidgetPainter( _painter, offset ));

        SubScope newState( state );
        newState._currentBlock->AddChunk(L"__painter__", pProxy1 );

        if ( pCrop ) {
            Rect crop;
            crop.x = value_of< int >( pCrop->GetChunk(L"x") );
            crop.y = value_of< int >( pCrop->GetChunk(L"y") );
            crop.w = value_of< int >( pCrop->GetChunk(L"w") );
            crop.h = value_of< int >( pCrop->GetChunk(L"h") );
            pProxy1->_painter.Intersect( crop );
        }

        if ( pAlpha ) {
            pProxy1->_painter.MultiplyAlpha( value_of< int >( pAlpha ));
        }
        
        AdvancedPaint advPaint( _painter.GetWidget() );
        int APtouched = 0;
        
        if ( pBlending )
        {
            TextUtf8 blending = value_of< TextUtf8 >( pBlending );
            APtouched |= 1;
            if ( L"normal" == blending ) {
                advPaint.BlendFun( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, true );
            }
            else if ( L"replace" == blending ) {
                advPaint.BlendFun( GL_ONE, GL_ZERO, true );
            }
            else if ( L"add" == blending ) { // add RGBA
                advPaint.BlendFun( GL_SRC_ALPHA, GL_ONE, true );
            }
            else if ( L"multiply" == blending ) { // multiply RGBA
                advPaint.BlendFun( GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, true );
            }
            else if ( L"add-color" == blending ) { // add RGB
                advPaint.BlendFun( GL_ONE, GL_ONE_MINUS_SRC_ALPHA, true );
            }
            else if ( L"multiply-color" == blending ) { // multiply RGB
                advPaint.BlendFun( GL_DST_COLOR, GL_ZERO, true );
            }
            else if ( L"shadow" == blending ) {
                advPaint.BlendFun( GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, true );
            }
        }

        if ( pColor )
        {
            APtouched |= 2;
            NormalPixel color( value_of< int >( pColor ));
            advPaint.ConstColor( color.c.r, color.c.g, color.c.b );
        }

        pFunc->Execute( __unused__, newState );

        if ( APtouched & 1 )
        {
            advPaint.BlendFun( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, true );
        }
        if ( APtouched & 2 )
        {
            advPaint.ConstColor( 255, 255, 255 );
        }
    }

    
    
    
    /*******************************

      WidgetProxy

    */
    WidgetProxy::WidgetProxy( Widget *widget ): _widget( widget )
    {
        XMLProgram::add_methods( _widget, this )
            .def("SetPosition",     &Widget::SetPosition, "pos")
            .def("GetPosition",     unconst_method_cast( &Widget::GetPosition ))
            .def("SetBoundingRect", &Widget::SetBoundingRect, "bound")
            .def("GetBoundingRect", unconst_method_cast( &Widget::GetBoundingRect ))
            .def("Resize",      &Widget::Resize, "w", "h")
            .def("GetWidth",    unconst_method_cast( &Widget::GetWidth ))
            .def("GetHeight",   unconst_method_cast( &Widget::GetHeight ))
            .def("SetOpacity",  &Widget::SetOpacity, "opacity" )
            .def("GetOpacity",  unconst_method_cast( &Widget::GetOpacity ))
            .def("SetVisible",  &Widget::SetVisible, "visible")
            .def("IsVisible",   unconst_method_cast( &Widget::IsVisible ))
            .def("SetOnTop",    &Widget::SetOnTop, "ontop")
            .def("IsOnTop",     unconst_method_cast( &Widget::IsOnTop ))
            .def("SetPhantom",  &Widget::SetPhantom, "phantom" )
            .def("IsPhantom",   unconst_method_cast( &Widget::IsPhantom ))
            .def("FocusIntercept",  &Widget::FocusIntercept )
            .def("FocusRelease",    &Widget::FocusRelease )
            .def("HasFocus",    unconst_method_cast( &Widget::HasFocus ))
            .def("InputIsEager", unconst_method_cast( &Widget::InputIsEager ))
            .def("InputIncEager", (void (Widget::*)())&Widget::InputIncEager )
            .def("InputDecEager", (void (Widget::*)())&Widget::InputDecEager )
            .def("InputIntercept", (void (Widget::*)())&Widget::InputIntercept )
            .def("InputRelease", (void (Widget::*)())&Widget::InputRelease )
            .def("HasInput",    unconst_method_cast( &Widget::HasInput ))
            .def("DoInputReset", (void (Widget::*)())( &Widget::DoInputReset ))
            .def("SetDirty",    (void (Widget::*)())&Widget::SetDirty )
            .def("BeAlwaysDirty", &Widget::BeAlwaysDirty, "always")
            .def("IsAlwaysDirty", unconst_method_cast( &Widget::IsAlwaysDirty ))
            .def("GetAbsolutePos", unconst_method_cast( &Widget::GetAbsolutePos ), "relativePos")
            .def("GetRelativePos", unconst_method_cast( &Widget::GetRelativePos ), "absolutePos")
            .def("GetAbsoluteRect", unconst_method_cast( &Widget::GetAbsoluteRect ), "relativeRect")
            .def("GetRelativeRect", unconst_method_cast( &Widget::GetRelativeRect ), "absoluteRect")
            // Signals
            .add("OnVisible",   make_signal_1_chunk( widget->OnVisible ))
            .add("OnIdle",      make_signal_2_chunk( widget->OnIdle ))
            .add("OnResize",    make_signal_2_chunk( widget->OnResize ))
            .add("OnReposition",make_signal_1_chunk( widget->OnReposition ))
            .add("OnActorMove", make_signal_1_chunk( widget->OnActorMove ))
            .add("OnFocus",     make_signal_0_chunk( widget->OnFocus ))
            .add("OffFocus",    make_signal_0_chunk( widget->OffFocus ))
            ;

        if ( CompoundWidget *c = dynamic_cast< CompoundWidget *>( widget ))
        {
            XMLProgram::add_methods_block("Compound", c, this )
                .def_remap_1< void, const WidgetPainter & >("DoRepaint", &CompoundWidget::DoRepaint, "painter")
                ;
        
            XMLProgram::add_methods( _widget, this )
                .add("OnPaint", make_signal_1_chunk( c->OnPaint ))
                ;
        }
        
        if ( ActiveAreaWidget *c = dynamic_cast< ActiveAreaWidget *>( widget ))
        {
            XMLProgram::add_methods( _widget, this )
                .add("OnPaint", make_signal_1_chunk( c->OnPaint ))
                ;
        }

        if ( CreditsWidget *c = dynamic_cast< CreditsWidget *>( widget ))
        {
            XMLProgram::add_methods_block("Credits", c, this )
                .def("Finished", unconst_method_cast( &CreditsWidget::Finished ))
                .def("Restart", &CreditsWidget::Restart )
                ;
        }
        if ( IWActive *ic = dynamic_cast< IWActive *>( widget ))
        {
            ActiveWidget *c = &ic->Control();
            XMLProgram::add_methods_block( "Active", c, this )
                .def("SetActive", &ActiveWidget::SetActive, "active" )
                .def("IsActive", &ActiveAreaWidget::IsActive )

                .add("OnPress",     make_signal_1_chunk( c->OnPress[0] ))
                .add("OnRelease",   make_signal_1_chunk( c->OnRelease[0] ))
                .add("OnClick",     make_signal_1_chunk( c->OnClick[0] ))
                .add("OnDoubleClick", make_signal_1_chunk( c->OnDoubleClick[0] ))
                .add("OnMoveIn",  make_signal_1_chunk( c->OnMoveIn[0] ))
                .add("OnMoveOut", make_signal_1_chunk( c->OnMoveOut[0] ))
                .add("OnCatch",   make_signal_1_chunk( c->OnCatch[0] ))
                .add("OnDrag",    make_signal_3_chunk( c->OnDrag[0] ))
                .add("OnDrop",    make_signal_2_chunk( c->OnDrop[0] ))
                .add("WhileHold", make_signal_1_chunk( c->WhileHold[0] ))
                .add("WhileDrag", make_signal_1_chunk( c->WhileDrag[0] ))
                .add("OffPress",  make_signal_1_chunk( c->OffPress[0] ))

                .add("OnPress1",     make_signal_1_chunk( c->OnPress[1] ))
                .add("OnRelease1",   make_signal_1_chunk( c->OnRelease[1] ))
                .add("OnClick1",     make_signal_1_chunk( c->OnClick[1] ))
                .add("OnDoubleClick1", make_signal_1_chunk( c->OnDoubleClick[1] ))
                .add("OnMoveIn1",     make_signal_1_chunk( c->OnMoveIn[1] ))
                .add("OnMoveOut1",    make_signal_1_chunk( c->OnMoveOut[1] ))
                .add("OnCatch1",   make_signal_1_chunk( c->OnCatch[1] ))
                .add("OnDrag1",    make_signal_3_chunk( c->OnDrag[1] ))
                .add("OnDrop1",    make_signal_2_chunk( c->OnDrop[1] ))
                .add("WhileHold1", make_signal_1_chunk( c->WhileHold[1] ))
                .add("WhileDrag1", make_signal_1_chunk( c->WhileDrag[1] ))
                .add("OffPress1",  make_signal_1_chunk( c->OffPress[1] ))

                .add("OnPress2",     make_signal_1_chunk( c->OnPress[2] ))
                .add("OnRelease2",   make_signal_1_chunk( c->OnRelease[2] ))
                .add("OnClick2",     make_signal_1_chunk( c->OnClick[2] ))
                .add("OnDoubleClick2", make_signal_1_chunk( c->OnDoubleClick[2] ))
                .add("OnMoveIn2",     make_signal_1_chunk( c->OnMoveIn[2] ))
                .add("OnMoveOut2",    make_signal_1_chunk( c->OnMoveOut[2] ))
                .add("OnCatch2",   make_signal_1_chunk( c->OnCatch[2] ))
                .add("OnDrag2",    make_signal_3_chunk( c->OnDrag[2] ))
                .add("OnDrop2",    make_signal_2_chunk( c->OnDrop[2] ))
                .add("WhileHold2", make_signal_1_chunk( c->WhileHold[2] ))
                .add("WhileDrag2", make_signal_1_chunk( c->WhileDrag[2] ))
                .add("OffPress2",  make_signal_1_chunk( c->OffPress[2] ))

                .add("OnMoveOver",  make_signal_1_chunk( c->OnMoveOver ))
                .add("OnMoveOutside",  make_signal_1_chunk( c->OnMoveOutside ))
                .add("WhileMoving",  make_signal_3_chunk( c->WhileMoving ))
                .add("OnActivate",  make_signal_1_chunk( c->OnActivate ))
                .add("OnWheel", make_signal_2_chunk( c->OnWheel ))
                .add("OffWheel", make_signal_2_chunk( c->OffWheel ))
                .add("OnChar", make_signal_1_chunk( c->OnChar ))
                .add("OnKeyPress", make_signal_1_chunk( c->OnKeyPress ))
                .add("OnKeyRelease", make_signal_1_chunk( c->OnKeyRelease ))
                .add("WhileKey", make_signal_1_chunk( c->WhileKey ))
                .add("WhileOffKey", make_signal_1_chunk( c->WhileOffKey ))
                .add("OffChar", make_signal_1_chunk( c->OffChar ))
                .add("OffKeyPress", make_signal_1_chunk( c->OffKeyPress ))
                ;
        }
        if ( IWEnum *ic = dynamic_cast< IWEnum *>( widget ))
        {
            EnumWidget *c = &ic->Enumerator();
            XMLProgram::add_methods_block("Enum", c, this )
                .def("GetSelected", unconst_method_cast( &EnumWidget::GetSelected ))
                .def("SetSelected", &EnumWidget::SetSelected, "no")
                .add("OnChange", make_signal_1_chunk( c->OnChange ))
                .add("OnConfirm", make_signal_1_chunk( c->OnConfirm ))
                ;
        }
        if ( IWList *ic = dynamic_cast< IWList *>( widget ))
        {
            ListWidget *c = &ic->Items();
            XMLProgram::add_methods_block( "List", c, this )
                .def
                ("GetNumEntries", unconst_method_cast( &ListWidget::GetNumEntries ))
                .def
                ("ClearEntries", &ListWidget::ClearEntries )
                .def_remap_1< int, TextUtf8 > 
                ("GetItemNo", unconst_method_cast( &ListWidget::GetItemNo ), "text")
                .def_remap_1< void, TextUtf8 >
                ("Insert", &ListWidget::Insert, "text")
                .def_remap_1< TextUtf8, int > 
                ("GetText", unconst_method_cast( &ListWidget::GetText ), "no")
                .def_remap_2< void, TextUtf8, TextUtf8 >
                ("SetText", (void (ListWidget::*)(const Text &, const Text &))&ListWidget::SetText, "find", "set")
                .def("SetUserFlagForItem", &ListLogic::SetUserFlagForItem, "no", "flagNo", "value")
                .def("GetUserFlagForItem", unconst_method_cast( &ListLogic::GetUserFlagForItem ), "no", "flagNo" )
                ;
        }
        if ( TextField *c = dynamic_cast< TextField *>( widget ))
        {
            XMLProgram::add_methods_block( "TextField", c, this )                
                .def_remap_0< TextUtf8 >
                ("GetText", unconst_method_cast( &TextField::GetText ))                
                .def_remap_1< void, TextUtf8 >
                ("SetText", &TextField::SetText, "text" )
                .add("OnCarMove", make_signal_1_chunk( c->OnCarMove ))
                .add("OnTextChange", make_signal_1_chunk( c->OnTextChange ))
                .add("OnConfirm", make_signal_1_chunk( c->OnConfirm ))
                ;
        }
        if ( BaseTextWidget *c = dynamic_cast< BaseTextWidget *>( widget ))
        {
            XMLProgram::add_methods_block( "Text", c, this )
                .def_remap_1< void, TextUtf8 >
				("SetText", &BaseTextWidget::SetText, "text")
                .def_remap_0< TextUtf8 >
				("GetText", unconst_method_cast( &BaseTextWidget::GetText ))
                .def("Clear", &BaseTextWidget::Clear )
                ;
        }
        if ( AdvTextBase *c = dynamic_cast< AdvTextBase *>( widget ))
        {
            XMLProgram::add_methods_block( "Text", c, this )
                .def_remap_1< void, TextUtf8 >
				("SetText", &AdvTextBase::SetText, "text")
                .def_remap_0< TextUtf8 >
				("GetText", unconst_method_cast( &AdvTextBase::GetText ))
                .def("Clear", &AdvTextBase::Clear )
                ;
        }
        if ( IWCaption *c = dynamic_cast< IWCaption *>( widget ))
        {
            CaptionWidget &p = c->Caption();
            XMLProgram::add_methods_block( "Caption", &p, this )
                .def_remap_1< void, TextUtf8 >
                                ( "SetCaption", &CaptionWidget::SetCaption, "text")
                .def_remap_0< TextUtf8 >
                                ( "GetCaption", unconst_method_cast( &CaptionWidget::GetCaption ))
                ;
        }
        if ( ScoreRoller *c = dynamic_cast< ScoreRoller *>( widget ))
        {
            XMLProgram::add_methods_block( "ScoreRoller", c, this )
                .def
                ("SetScore", &ScoreRoller::SetScore, "score")
                .def
                ("GetScore", &ScoreRoller::GetScore )
                .def_remap_1< void, double >
                ("SetRollSpeed", &ScoreRoller::SetRollSpeed, "speed")
                .def_remap_0< double >
                ("GetRollSpeed", unconst_method_cast( &ScoreRoller::GetRollSpeed ))
                ;
        }
        if ( LoomWidget *c = dynamic_cast< LoomWidget *>( widget ))
        {
            XMLProgram::add_methods_block( "Loom", c, this )                
                .def("LoomUp", &LoomWidget::LoomUp )
                .def("Vanish", &LoomWidget::Vanish )
                .def("IsUp", unconst_method_cast( &LoomWidget::IsUp ))
                .def("LoomVanish", &LoomWidget::LoomVanish )
                .def("SetMinVisible", &LoomWidget::SetMinVisible, "numPixels" )
                .add("OnLoomStop", make_signal_0_chunk( c->OnLoomStop ))
                .add("OnVanishStop", make_signal_0_chunk( c->OnVanishStop ))
                ;
        }
        if ( ImageWidget *c = dynamic_cast< ImageWidget *>( widget ))
        {
            this->AddChunk(L"Image", new ImageWidgetProxy( c ));
        }
    }

    WidgetProxy::~WidgetProxy()
    {
    }
   
    // Helper - ImageWidget wrapper

    ImageWidgetProxy::ImageWidgetProxy( ImageWidget *w ): _imgWidget( w )
    {
        XMLProgram::add_methods( this )
            .def("SetImageByName", &ImageWidgetProxy::SetImageByName, "name")
            .def("SetSkinByName", &ImageWidgetProxy::SetSkinByName, "name")
            ;
    }

    ImageWidgetProxy::~ImageWidgetProxy()
    {
    }

    void ImageWidgetProxy::SetImageByName( std::string name )
    {
        _imgWidget->SetSkin( *MakeImage( name ));
    }

    void ImageWidgetProxy::SetSkinByName( std::string name )
    {
        _imgWidget->SetSkin( QuerySkin( name ));
    }
    
    /*******************************

      LayoutProxy

    */
    LayoutProxy::LayoutProxy( LayoutBase *layout ): _layout( layout )
    {
        add_methods( this )
            .def("SetWeight", &LayoutProxy::SetWeight, "widget", "weight")
            .def("SetMaxSize", &LayoutProxy::SetMaxSize, "widget", "maxsize")
            .def("SetMinSize", &LayoutProxy::SetMinSize, "widget", "minsize")
            .def("SetOverlap", &LayoutProxy::SetOverlap, "widget", "overlap")
            ;
        add_methods( _layout, this )
            .def("Fill", &LayoutBase::Fill )
            ;
    }

    LayoutProxy::~LayoutProxy()
    {
    }

    void LayoutProxy::SetWeight( Widget *widget, int weight )
    {
        (*_layout) << widget;
        Weight_ attr( weight ); (*_layout) << attr;
    }

    void LayoutProxy::SetMaxSize( Widget *widget, int maxsize )
    {
        (*_layout) << widget;
        MaxSize_ attr( maxsize ); (*_layout) << attr;
    }

    void LayoutProxy::SetMinSize( Widget *widget, int minsize )
    {
        (*_layout) << widget;
        MinSize_ attr( minsize ); (*_layout) << attr;
    }

    void LayoutProxy::SetOverlap( Widget *widget, int overlap )
    {
        (*_layout) << widget;
        Overlap_ attr( overlap ); (*_layout) << attr;
    }

    /*******************************

      WorkspaceProxy

    */
    WorkspaceProxy::WorkspaceProxy( Workspace *workspace ): _workspace( workspace )
    {
        add_methods( _workspace, this )
            .def("GetWorkspaceRect", unconst_method_cast( &Workspace::GetWorkspaceRect ))
            .def("ShowAtPointer", &Workspace::ShowAtPointer, "widget" )
            .def("ShowAtPosition", &Workspace::ShowAtPosition, "pos", "widget" )
            .def("WhereIsPointer", unconst_method_cast( &Workspace::WhereIsPointer ))
            .def("SetPreviousCursor", &Workspace::SetPreviousCursor )
            .def("ShowCursor", &Workspace::ShowCursor )
            .def("HideCursor", &Workspace::HideCursor )
            .def("IsCursorShown", unconst_method_cast( &Workspace::IsCursorShown ))
            .def("IsKeyPressed", unconst_method_cast( &Workspace::IsKeyPressed ), "key_code")
            .def("GetWidth", unconst_method_cast( &Workspace::GetWidth ))
            .def("GetHeight", unconst_method_cast( &Workspace::GetHeight ))
            ;

        add_methods( this )
            .def("SetCursor", &WorkspaceProxy::SetCursor, "name" )
            ;
    }

    WorkspaceProxy::~WorkspaceProxy()
    {
    }

    void WorkspaceProxy::SetCursor( std::string name )
    {
        SPointer< MouseCursor > cursor = MakeMouseCursor( name );
        _workspace->SetCursor( *cursor );
    }

    
};//XMLProgram

