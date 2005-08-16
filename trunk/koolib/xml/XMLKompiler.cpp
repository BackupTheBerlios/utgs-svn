/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE KooLAS C++ Template & Class Library     *
 * CALLED KooLib SOURCE CODE.                                       *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE KooLib SOURCE CODE IS (C) COPYRIGHT 2004                     *
 * by the Rados³aw Kolasiñski (koolas@sus.univ.szczecin.pl)         *
 *                                                                  *
 ********************************************************************

function:
last mod: $Id: $

 ********************************************************************/

#include <koolib/xml/XMLKompiler.h>

namespace XMLFactory {
    
    using namespace XMLProgram;

    static USELESS_XML_TAG_DICT( xtdProgramCompile, ExecutionState,
            // symbols
            .AddReg< CXML::LET >("let")
            .AddReg< CXML::SET >("set")
            .AddReg< CXML::GET >("get")            
            .AddReg< CXML::LOOKUP >("lookup")
            .AddReg< CXML::REGISTER >("register")
            .AddReg< CXML::IS_NOT_EMPTY >("is-not-empty")
            .AddReg< CXML::IS_DEFINED   >("is-defined")
            .AddReg< CXML::ERROR >("error")
            .AddReg< CXML::NOCOMPILE >("nocompile")

            // functions
            .AddReg< CXML::FUNCTION  >("function")
            .AddReg< CXML::CALL      >("call")
            .AddReg< CXML::LAZY_CALL >("lazy-call")
            
            // newer functions
            .AddReg< CXML::APPLY     >("apply")
            .AddReg< CXML::CURRY     >("curry")
            .AddReg< CXML::LISTNODE  >("listnode")

            // block
            .AddReg< CXML::BLOCK >("block")
            .AddReg< CXML::EXTRACT >("extract")
            .AddReg< CXML::DOIN >("doin")
            .AddReg< CXML::DO >("do")

            // list operations
            .AddReg< CXML::EMPTY    >("empty")
            .AddReg< CXML::LIST_NODE>("list-node")
            .AddReg< CXML::LIST     >("list")
            .AddReg< CXML::MAP  >("map")
            .AddReg< CXML::FOLD >("fold")
            .AddReg< CXML::SEEK >("seek")
            .AddReg< CXML::HEAD >("head")
            .AddReg< CXML::RANGE    >("range")
            .AddReg< CXML::ZIP  >("zip")

            // accumulative operators
            .AddReg< CXML::ADD  >("add")
            .AddReg< CXML::MUL  >("mul")
            .AddReg< CXML::FADD >("fadd")
            .AddReg< CXML::FMUL >("fmul")
            .AddReg< CXML::AND  >("and")
            .AddReg< CXML::OR   >("or")
            .AddReg< CXML::CAT  >("cat")
           
            // comparison 
            .AddReg< CXML::COMPARE  >("compare")
            .AddReg< CXML::FCOMPARE >("fcompare")
            .AddReg< CXML::STRCMP   >("strcmp")
            .AddReg< CXML::PTRCMP   >("ptrcmp")
            .AddReg< CXML::LESS     >("less")
            .AddReg< CXML::GREATER  >("greater")
            .AddReg< CXML::EQUAL    >("equal")
            
            // unary operators
            .AddReg< CXML::NOT              >("not")
            .AddReg< CXML::MINUS< int >     >("minus")
            .AddReg< CXML::MINUS< double >  >("fminus")
            .AddReg< CXML::RECIPROCAL       >("reciprocal")

            // primitives
            .AddReg< CXML::BOOLEAN  >("boolean")
            .AddReg< CXML::INTEGER  >("integer")
            .AddReg< CXML::FLOAT    >("float")
            .AddReg< CXML::STRING   >("string")

            // just copy xml nodes
            .AddReg< CXML::COPY     >("copy")

            // conditional execution
            .AddReg< CXML::IF       >("if")
            .AddReg< CXML::CHOOSE   >("choose")
            );
};

namespace XMLProgram {

    using namespace XMLFactory;
    
    namespace XML
    {
        struct COMPILE {};
    };

    template< class _Type >
    _Type * HookErrors( Node node, _Type *pChunk )
    {
        if ( CXML::CXmlErrors *err = dynamic_cast< CXML::CXmlErrors *>( pChunk ))
        {
            err->_line = node->_line;
            err->_source = node->_source;
        }
        return pChunk;
    }
    
    IChunkPtr SelectExecutionModel( IChunk *pChunk )
    {
        if ( 1 == pChunk->GetFourCC( FOURCC_NAME_CONST ))
        {
            return pChunk ;
        }
        else if ( 1 == pChunk->GetFourCC( FOURCC_NAME_GET ))
        {
            return pChunk ;
        }
        else
        {
            return new CXML::EvalInNewScope( pChunk );
        }
    }

    void HookDefinition( Node node, ExecutionState &state )
    {
        Attr< TextUtf8, false, wchar_t > _id(L"id");
        if ( GetAttr( _id, node, state ))
        {
            IChunk *pChunk = state.GetResult();
            state.SetResult( HookErrors( node, new CXML::LET( _id.str(), SelectExecutionModel( pChunk ).get() )));
        }
    }

    IChunkPtr Compile( Node node, ExecutionState &state )
    {
        SubScope newState( state );
        newState._currentDict = &xtdProgramCompile;

        IChunkPtr pEnd = new XMLEmpty();
        IChunkPtr pHead = pEnd;
        XMLListChunk *pLast = 0;

        for ( Node child( node.GetFirstChild() ); !!child; ++child )
        {
            newState.Execute( child );
            if ( !IsEmpty( newState.GetResult() ))
            {
                XMLListChunk *p = new XMLListChunk( newState.GetResult(), pEnd.get() );
                if ( 0 == pLast )
                {
                    pHead = p;
                }
                else
                {                
                    pLast->SetTail( p );
                }
                pLast = p;
            }
            newState.Reuse();
        }
        if ( IsEmpty( pHead.get() ))
        {
            return pEnd;
        }
        else if ( IsEmpty( pHead->GetChunk( FOURCC_LIST_TAIL )))
        {
            return pHead->GetChunk( FOURCC_LIST_HEAD );
        }
        else
        {
            return pHead;
        }
    }

    IChunkPtr CompileOneNode( Node node, ExecutionState &state )
    {
        SubScope newState( state );
        newState._currentDict = &xtdProgramCompile;
        newState.Execute( node );
        return newState.GetResult();
    }

    IChunkPtr CompileRightValue( Node node, ExecutionState &state )
    {
        Attr< TextUtf8, false, wchar_t > _select(L"select");
        Attr< TextUtf8, false, wchar_t > _force(L"force");
        if ( GetAttr( _select, node, state ))
        {
            return HookErrors( node, new CXML::GET( _select.str() ));
        }
        else if ( GetAttr( _force, node, state ))
        {
            return HookErrors( node, new CXML::CALL( _force.str(), new XMLEmpty() ));
        }
        else
        {
            return SelectExecutionModel( Compile( node, state ).get() ).get();
        }
    }
    
    IChunkPtr NoCompile( Node node, ExecutionState &state )
    {
        return new XMLCodeChunk( node );
    }

};//XMLProgram

namespace XMLFactory {

    using namespace XMLProgram;
    //---------------------------------------------------------

#define LOCAL_TAG_HANDLER( _name )  USELESS_XML_TAG_HANDLER( _name, ExecutionState )

    LOCAL_TAG_HANDLER( XML::COMPILE )
    {
        Attr< int, false, wchar_t > _run(L"run");
        if ( GetAttr( _run, _node, _state ) && 1 == *_run )
        {
            IChunkPtr compiled = Compile( _node, _state );
            compiled->Execute( Node(), _state );
        }
        else
        {
            _state.SetResult( Compile( _node, _state ).get() );
        }
    }

    LOCAL_TAG_HANDLER( CXML::NOCOMPILE )
    {
        Attr< int, false, wchar_t > _run(L"run");
        if ( GetAttr( _run, _node, _state ) && 1 == *_run )
        {
            IChunkPtr compiled = NoCompile( _node, _state );
            compiled->Execute( Node(), _state );
        }
        else
        {
            _state.SetResult( NoCompile( _node, _state ).get() );
        }
    }


    //---------------------------------------------------------
    //
    // Basic nodes.
    //

    /*
     * Define NEW symbol.
     *
     *  <let id="Name" [select="ChunkId"|force="ChunkId"]>[<!-- code -->]</let>
     */
    LOCAL_TAG_HANDLER( CXML::LET )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::LET( _id.str(), CompileRightValue( _node, _state ).get() )));
    }

    /*
     * Reset EXISTING symbol.
     *
     *  <set id="Name" [select="ChunkId"|force="ChunkId"]>[<!-- code -->]</let>
     */
    LOCAL_TAG_HANDLER( CXML::SET )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::SET( _id.str(), CompileRightValue( _node, _state ).get() )));
    }

    /*
     * Get EXISTING symbol.
     *
     *  <get id="Name"/>
     *
     */
    LOCAL_TAG_HANDLER( CXML::GET )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::GET( _id.str() )));
    }

    /*
     * Find symbol in EXISTING dict.
     *
     *  <lookup>
     *      <!-- let id="dict" -->
     *      <!-- let id="name" -->
     *  </lookup>
     */
    LOCAL_TAG_HANDLER( CXML::LOOKUP )
    {
        _state.SetResult( HookErrors( _node, new CXML::LOOKUP( Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }

    /*
     * Add symbol to EXISTING dict.
     *
     *  <register>
     *      <!-- let id="dict" -->
     *      <!-- let id="name" -->
     *      <!-- let id="value" -->
     *  </register>
     */
    LOCAL_TAG_HANDLER( CXML::REGISTER )
    {
        _state.SetResult( HookErrors( _node, new CXML::REGISTER( Compile( _node, _state ).get() )));
    }


    /*
     * Check if EXISTING symbol refers to <empty>.
     */
    LOCAL_TAG_HANDLER( CXML::IS_NOT_EMPTY )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::IS_NOT_EMPTY( _id.str() )));
    }

    /*
     * Check if symbol exists.
     */
    LOCAL_TAG_HANDLER( CXML::IS_DEFINED )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::IS_DEFINED( _id.str() )));
    }

    /*
     * Throw error message.
     */
    LOCAL_TAG_HANDLER( CXML::ERROR )
    {
        if ( _node.HasChildren() )
        {
            _state.SetResult( HookErrors( _node, new CXML::ERROR( Compile( _node, _state ).get() )));
        }
        else
        {
            _state.SetResult( HookErrors( _node, new CXML::ERROR(
                            new CXML::EvaluableValue< TextUtf8 >( _node->_value )
                            )));
        }
    }

    /*
     * Define NEW symbol refering to executable chunk.
     *
     *  <function id="Name" parameters="Name Name .. Name">
     *      <!-- code -->
     *  </function>
     */
    LOCAL_TAG_HANDLER( CXML::FUNCTION )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::LET( _id.str(), new CXML::ConstantValue( Compile( _node, _state ).get() ))));
    }

    /*
     * Invoke chunk associated with EXISTING symbol in place where <call> occures.
     *
     *  <call id="Name">
     *      <!-- params -->
     *  </call>
     *
     *  (*)Note: <!-- params --> are defined in invocation scope.
     */
    LOCAL_TAG_HANDLER( CXML::CALL )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        CXML::CALL *pCall = new CXML::CALL( _id.str(), Compile( _node, _state ).get() );
        _state.SetResult( HookErrors( _node, pCall ));
    }
    
    /*
     * Invoke chunk associated with EXISTING symbol in place where <call> occures.
     *
     *  <apply id="Name" select="Name">
     *      <!-- params -->
     *  </apply>
     *
     *  (*)Note: <!-- params --> are defined in invocation scope.
     */
    LOCAL_TAG_HANDLER( CXML::APPLY )
    {
        Attr< TextUtf8, true, wchar_t > _select(L"select");
        GetAttr( _select, _node, _state );
        CXML::CALL *pCall = new CXML::CALL( _select.str(), Compile( _node, _state ).get() );
        _state.SetResult( HookErrors( _node, pCall ));
        HookDefinition( _node, _state );
    }

    /*
     * Postpone invocation of chunk associated with EXISTING symbol.
     *
     *  <lazy-call id="Name">
     *      <!-- params -->
     *  </lazy-call>
     *
     * (*)Note: <!-- params --> are evaluated in place where <lazy-call> occures.
     */
    LOCAL_TAG_HANDLER( CXML::LAZY_CALL )
    {
        Attr< TextUtf8, true, wchar_t > _id(L"id");
        GetAttr( _id, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::LAZY_CALL( _id.str(), Compile( _node, _state ).get() )));
    }

    /*
     * Postpone invocation of chunk associated with EXISTING symbol.
     *
     *  <curry id="Name" select="Name">
     *      <!-- params -->
     *  </curry>
     *
     * (*)Note: <!-- params --> are evaluated in place where <curry> occures.
     */
    LOCAL_TAG_HANDLER( CXML::CURRY )
    {
        Attr< TextUtf8, true, wchar_t > _select(L"select");
        GetAttr( _select, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::LAZY_CALL( _select.str(), Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }


    /*
     * Create block.
     *
     *  <block [id="Name"]>
     *      <!-- let -->
     *      <!-- let -->
     *      ..
     *      <!-- let -->
     *  </block>
     */
    LOCAL_TAG_HANDLER( CXML::BLOCK )
    {
        _state.SetResult( new CXML::BLOCK( Compile( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    /*
     * Extract all symbols into curernt block.
     *
     *  <extract select="Name"/>
     */
    LOCAL_TAG_HANDLER( CXML::EXTRACT )
    {
        Attr< TextUtf8, true, wchar_t > _select(L"select");
        GetAttr( _select, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::EXTRACT( _select.str() )));
    }

    /* 
     * Execute <!-- code --> in block ChunkId.
     *
     *  <doin select="ChunkId" [blockResult="Boolean"]>
     *      <!-- code -->
     *  </doin>
     *
     * Execute <!-- code --> in block Force( ChunkId ).
     *
     *  <doin force="ChunkId" [blockResult="Boolean"]>
     *      <!-- code -->
     *  </doin>
     *
     * Execute <!-- code --> in Force( <!-- getter --> ).
     *
     *  <doin [blockResult="Boolean"]>
     *      <!-- getter -->
     *      <!-- code -->
     *  </doin>
     *
     * (*)Note: result is block if 'blockResult' = 1 or code result 
     *
     */
    LOCAL_TAG_HANDLER( CXML::DOIN )
    {
        Attr< TextUtf8, false, char > _select("select");
        Attr< TextUtf8, false, char > _force("force");
        Attr< int, false, char >      _blockResult("blockResult");

        bool blockResult = (_node >> _blockResult) && *_blockResult;
        
        if ( _node >> _select )
        {
            _state.SetResult( HookErrors( _node,
                        new CXML::DOIN(
                            new CXML::GET( _select.str() ),
                            Compile( _node, _state ).get(),
                            blockResult )));
        }
        else if ( _node >> _force )
        {
            _state.SetResult( HookErrors( _node,
                        new CXML::DOIN(
                            new CXML::LazyCall( new CXML::GET( _select.str() )),
                            Compile( _node, _state ).get(),
                            blockResult )));
        }
        else
        {
            IChunkPtr pInner = Compile( _node, _state );
            IChunkPtr pCond = pInner->GetChunk( FOURCC_LIST_HEAD );
            IChunkPtr pFunc = pInner->GetChunk( FOURCC_LIST_TAIL );
            _state.SetResult( HookErrors( _node,
                        new CXML::DOIN( pCond.get(), pFunc.get(), blockResult )));
        }
        HookDefinition( _node, _state );
    }

    /*
     * Execute <!-- code --> in new block
     *
     *  <do>
     *      <!-- code -->
     *  </do>
     *
     */
    LOCAL_TAG_HANDLER( CXML::DO )
    {
        _state.SetResult( HookErrors( _node, CompileRightValue( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }


    //---------------------------------------------------------
    //
    // Accumulatives operations
    //

#define CXML_ACCUM_HANDLER( _Name, _OpType ) \
    LOCAL_TAG_HANDLER( _Name ) \
    { \
        IChunkPtr pChunk = Compile( _node, _state ); \
            boost::intrusive_ptr< CXML::ValueAccumulator< _OpType > > \
            adder = new CXML::ValueAccumulator< _OpType >(); \
            while ( !IsEmpty( pChunk.get())) \
            { \
                adder->Add( pChunk->GetChunk( FOURCC_LIST_HEAD ) ); \
                    pChunk = pChunk->GetChunk( FOURCC_LIST_TAIL ); \
            } \
        _state.SetResult( HookErrors( _node, adder.get() )); \
        HookDefinition( _node, _state ); \
    }

    // <add><!-- integer --></add>
    CXML_ACCUM_HANDLER( CXML::ADD, CXML::OpADD< int > );

    // <mul><!-- integer --></mul>
    CXML_ACCUM_HANDLER( CXML::MUL, CXML::OpMUL< int > );

    // <fadd><!-- float --></fadd>
    CXML_ACCUM_HANDLER( CXML::FADD, CXML::OpADD< double > );

    // <fmul><!-- float --></fmul>
    CXML_ACCUM_HANDLER( CXML::FMUL, CXML::OpMUL< double > );

    // <and><!-- boolean --></and>
    CXML_ACCUM_HANDLER( CXML::AND, CXML::OpAND );

    // <or><!-- boolean --></or>
    CXML_ACCUM_HANDLER( CXML::OR, CXML::OpOR );

    // <compare><!-- integer --></compare>
    CXML_ACCUM_HANDLER( CXML::COMPARE, CXML::OpCOMPARE< int > );

    // <fcompare><!-- float --></fcompare>
    CXML_ACCUM_HANDLER( CXML::FCOMPARE, CXML::OpCOMPARE< double > );

    // <strcmp><!-- string --></strcmp>
    CXML_ACCUM_HANDLER( CXML::STRCMP, CXML::OpCOMPARE< TextUtf8 > );

    // <cat><!-- string --></cat>
    CXML_ACCUM_HANDLER( CXML::CAT, CXML::OpCAT );

    // <ptrcmp><!-- pointer --></ptrcmp>
    LOCAL_TAG_HANDLER( CXML::PTRCMP )
    {
        IChunkPtr ptr[2];
        int i=0;
        for ( Node child(_node.GetFirstChild()); !!child && i < 2; ++child, ++i )
        {
            ptr[i] = CompileOneNode( child, _state );
        }
        _state.SetResult( new CXML::PTRCMP( ptr[0].get(), ptr[1].get() ));
        HookDefinition( _node, _state );
    }

    // <less><!-- compare --></less>
    LOCAL_TAG_HANDLER( CXML::LESS )
    {
        _state.SetResult( new CXML::LESS( Compile( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    // <greater><!-- compare --></greater>
    LOCAL_TAG_HANDLER( CXML::GREATER )
    {
        _state.SetResult( new CXML::GREATER( Compile( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    // <equal><!-- compare --></equal>
    LOCAL_TAG_HANDLER( CXML::EQUAL )
    {
        _state.SetResult( new CXML::EQUAL( Compile( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    // <not><!-- boolean --></not>
    LOCAL_TAG_HANDLER( CXML::NOT )
    {
        _state.SetResult( new CXML::NOT( CompileRightValue( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    // <minus><!-- integer --></minus>
    LOCAL_TAG_HANDLER( CXML::MINUS< int > )
    {
        _state.SetResult( new CXML::MINUS< int >( CompileRightValue( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    // <fminus><!-- float --></fminus>
    LOCAL_TAG_HANDLER( CXML::MINUS< double > )
    {
        _state.SetResult( new CXML::MINUS< double >( CompileRightValue( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }

    // <reciprocal><!-- float --></reciprocal>
    LOCAL_TAG_HANDLER( CXML::RECIPROCAL )
    {
        _state.SetResult( new CXML::RECIPROCAL( CompileRightValue( _node, _state ).get() ));
        HookDefinition( _node, _state );
    }


    //---------------------------------------------------------
    //
    // Primitive types
    //

#define CXML_VALUE_HANDLER( _Name, _Type ) \
    LOCAL_TAG_HANDLER( _Name ) \
    { \
        Attr< TextUtf8, false, wchar_t > _select(L"select");\
            Attr< TextUtf8, false, wchar_t > _force(L"force");\
            Attr< _Type, false, wchar_t > _value(L"value"); \
            if ( GetAttr( _value, _node, _state )) \
            { \
                _state.SetResult( new CXML::ConstantValue( CreateValue( *_value ) )); \
            } \
            else if ( GetAttr( _select, _node, _state ))\
            {\
                _state.SetResult( new CXML::ValueConvertion< _Type >( new CXML::GET( _select.str() )));\
            }\
            else if ( GetAttr( _force, _node, _state ))\
            {\
                _state.SetResult( new CXML::ValueConvertion< _Type >( new CXML::LazyCall( new CXML::GET( _force.str() ))));\
            }\
            else if ( _node.HasChildren() ) \
            { \
                _state.SetResult( new CXML::ValueConvertion< _Type >( Compile( _node, _state ).get() )); \
            } \
            else \
            { \
                _state.SetResult( new CXML::EvaluableValue< _Type >( _node->_value )); \
            } \
        HookDefinition( _node, _state );\
    }

    /*
     *  <boolean|integer|float|string [id="Name"] [value="ConstValue"|select="ChunkId"|force="ChunkId"]>[Expression]</->
     */
    CXML_VALUE_HANDLER( CXML::BOOLEAN, int );
    CXML_VALUE_HANDLER( CXML::INTEGER, int );
    CXML_VALUE_HANDLER( CXML::FLOAT, double );
    CXML_VALUE_HANDLER( CXML::STRING, TextUtf8 );


    /*
     *  <copy>
     *      <!-- any xml nodes -->
     *  </copy>
     */
    LOCAL_TAG_HANDLER( CXML::COPY )
    {
        _state.SetResult( HookErrors( _node, new CXML::COPY( _node )));
        HookDefinition( _node, _state );
    }


    //---------------------------------------------------------
    //
    // List operations
    //

    /*
     * Create <empty>
     */
    LOCAL_TAG_HANDLER( CXML::EMPTY )
    {
        _state.SetResult( new CXML::ConstantValue( new XMLEmpty() ));
        HookDefinition( _node, _state );
    }

    /*
     * Create one list-node.
     *
     *  <list-node>
     *      <head>
     *          <!-- code -->
     *      </head>
     *      <tail>
     *          <!-- tail-code -->
     *      </tail>
     *  </list-node>
     *
     *  (*)Note: The result of <!-- tail-code --> is list node or function returning list node.
     */
    LOCAL_TAG_HANDLER( CXML::LIST_NODE )
    {
        IChunkPtr pHead, pTail;
        for ( Node child( _node.GetFirstChild() ); !!child; ++child )
        {
            if ( child->_name == "head" )
            {
                if ( pHead.get() ) { throw Useless::Error("<list-node> Expects exactly one 'head'"); }
                pHead = CompileRightValue( child, _state );
            }
            else if ( child->_name == "tail" )
            {
                if ( pTail.get() ) { throw Useless::Error("<list-node> Expects exactly one 'tail'"); }
                pTail = CompileRightValue( child, _state );
            }
        }
        _state.SetResult( HookErrors( _node, new CXML::LIST_NODE( pHead.get(), pTail.get() )));
    }

    /*
     * Create one list-node.
     *
     *  <listnode>
     *      <!-- let id="head" -->
     *      <!-- let id="tail" -->
     *  </listnode>
     *
     *  (*)Note: The result of <!-- tail --> is list node or function returning list node.
     */
    LOCAL_TAG_HANDLER( CXML::LISTNODE )
    {
        _state.SetResult( HookErrors( _node, new CXML::LISTNODE( Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }

    /*
     * Create unnamed list:
     *
     *  <list>
     *      <!-- code -->
     *      <!-- code -->
     *      ..
     *      <!-- code -->
     *  </list>
     *
     * Create list with name "$(id)".
     *
     *  <list id="Name">
     *      <!-- code -->
     *      <!-- code -->
     *      ..
     *      <!-- code -->
     *  </list>
     */
    LOCAL_TAG_HANDLER( CXML::LIST )
    {
        CXML::LIST_NODE *last = 0;
        IChunkPtr guard = new CXML::ConstantValue( new XMLEmpty() );
        IChunkPtr root = guard;
        for ( Node child( _node.GetFirstChild() ); !!child; ++child )
        {
            CXML::LIST_NODE *p = HookErrors( child, new CXML::LIST_NODE( CompileOneNode( child, _state ).get(), guard.get() ));
            if ( 0 == last )
            {
                root = p;
            }
            else
            {
                last->_pTail = p;
            }
            last = p;
        }
        _state.SetResult( HookErrors( _node, root.get() ));
        HookDefinition( _node, _state );
    }
    /*
     * Create a list from values returned by applying "func" for each element of "list".
     *
     *  <map id="Name" iterator="Name">
     *      <!-- let id="list" -->
     *      <!-- let id="func" -->
     *  </map>
     *
     */
    LOCAL_TAG_HANDLER( CXML::MAP )
    {
        Attr< TextUtf8, true, wchar_t > _iterator(L"iterator");
        GetAttr( _iterator, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::MAP( _iterator.str(), Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }

    /*
     * For each element of "list" apply function "func", with accumulator equal to "accum".
     * Chunk "func" must accept parameters "$(iterator)" and "$(accumulator)".
     *
     *  <fold iterator="Name" accumulator="Name">
     *      <!-- let id="list" -->
     *      <!-- let id="func" -->
     *      <!-- let id="accum" -->
     *  </fold>
     *
     */
    LOCAL_TAG_HANDLER( CXML::FOLD )
    {
        Attr< TextUtf8, true, wchar_t > _iterator(L"iterator");
        Attr< TextUtf8, true, wchar_t > _accumulator(L"accumulator");
        GetAttr( _iterator, _node, _state );
        GetAttr( _accumulator, _node, _state );
        _state.SetResult( HookErrors( _node, new CXML::FOLD( _iterator.str(), _accumulator.str(), Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }

    /*
     * Obtain list after "count" of elements.
     *
     *  <seek>
     *      <!-- let id="count" -->
     *      <!-- let id="list" -->
     *  </seek>
     */
    LOCAL_TAG_HANDLER( CXML::SEEK )
    {
        _state.SetResult( HookErrors( _node, new CXML::SEEK( Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }
    
    /*
     * Create list of first "count" of elements.
     *
     *  <head>
     *      <!-- let id="count" -->
     *      <!-- let id="list" -->
     *  </head>
     */
    LOCAL_TAG_HANDLER( CXML::HEAD )
    {
        _state.SetResult( HookErrors( _node, new CXML::HEAD( Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }
    
    /*
     * Create list of integers in range ["first", "first + "count").
     *
     *  <head>
     *      <!-- let id="first" -->
     *      <!-- let id="count" -->
     *  </head>
     */
    LOCAL_TAG_HANDLER( CXML::RANGE )
    {
        _state.SetResult( HookErrors( _node, new CXML::RANGE( Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }

    /*
     * Create zip to iterate over parallel lists.
     *
     *  <zip>
     *      <!-- list-code -->
     *      <!-- list-code -->
     *      ..
     *      <!-- list-code -->
     *  </zip>
     */
    LOCAL_TAG_HANDLER( CXML::ZIP )
    {
        _state.SetResult( HookErrors( _node, new CXML::ZIP( Compile( _node, _state ).get() )));
        HookDefinition( _node, _state );
    }



    //---------------------------------------------------------
    //
    // Conditional code execution
    //

    /* 
     * Execute <!-- code --> if EvaluateBoolean( EvaluateString( Expression )) is 1.
     *
     *  <if test="Expression">
     *      <!-- code -->
     *  </if>
     *
     * Execute <!-- code --> if value_of< int >( ChunkId ) is 1.
     *
     *  <if select="ChunkId">
     *      <!-- code -->
     *  </if>
     *
     * Execute <!-- code --> if value_of< int >( Force( ChunkId )) is 1.
     *
     *  <if force="ChunkId">
     *      <!-- code -->
     *  </if>
     *
     * Execute <!-- code --> if value_of< int >( Force( <!-- boolean --> )) is 1.
     *
     *  <if>
     *      <!-- boolean -->
     *      <!-- code -->
     *  </if>
     *
     * (*)Note: When <!-- code --> is not executed, resultant value is <empty>.
     *
     */
    LOCAL_TAG_HANDLER( CXML::IF )
    {
        Attr< TextUtf8, false, char > _cond("test");
        Attr< TextUtf8, false, char > _select("select");
        Attr< TextUtf8, false, char > _force("force");
        if ( _node >> _cond )
        {
            _state.SetResult( HookErrors( _node, new CXML::IF( new CXML::EvaluableValue< int >( _cond.str() ), Compile( _node, _state ).get() )));
        }
        else if ( _node >> _select )
        {
            _state.SetResult( HookErrors( _node, new CXML::IF( new CXML::GET( _select.str() ), Compile( _node, _state ).get() )));
        }
        else if ( _node >> _force )
        {
            _state.SetResult( HookErrors( _node, new CXML::IF( new CXML::LazyCall( new CXML::GET( _select.str() )), Compile( _node, _state ).get() )));
        }
        else
        {
            IChunkPtr pInner = Compile( _node, _state );
            IChunkPtr pCond = pInner->GetChunk( FOURCC_LIST_HEAD );
            IChunkPtr pFunc = pInner->GetChunk( FOURCC_LIST_TAIL );
            _state.SetResult( HookErrors( _node, new CXML::IF( pCond.get(), pFunc.get() )));
        }
    }

    /*
     * Similar to <if> with difference <choose> allows multiple conditions.
     * Each case is enclosed in <!-- when --> node.
     * Default case is enclosed in <otherwise> node.
     *
     *  <choose>
     *      <!-- when -->
     *      <otherwise>
     *          <!-- code -->
     *      </otherwise>
     *  </choose>
     *
     *  where <!-- when --> is one of:
     *
     *  <when test="Expression">
     *      <!-- code -->
     *  </when>
     *
     *  <when select="ChunkId">
     *      <!-- code -->
     *  </when>
     *
     *  <when force="ChunkId">
     *      <!-- code -->
     *  </when>
     *
     *  <when>
     *      <!-- boolean -->
     *      <!-- code -->
     *  </when>
     *
     *
     */
    LOCAL_TAG_HANDLER( CXML::CHOOSE )
    {
        boost::intrusive_ptr< CXML::CHOOSE > pChoose = new CXML::CHOOSE;
        for ( Node child( _node.GetFirstChild()); !!child; ++child )
        {
            if ( child->_name == "when" )
            {
                Attr< TextUtf8, false, char > _test("test");
                Attr< TextUtf8, false, char > _select("select");
                Attr< TextUtf8, false, char > _force("force");
                if ( child >> _test )
                {
                    pChoose->AddOption( new CXML::EvaluableValue< int >( _test.str() ), Compile( child, _state ));
                }
                else if ( child >> _select )
                {
                    pChoose->AddOption( HookErrors( child, new CXML::GET( _select.str() )), Compile( child, _state ));
                }
                else if ( child >> _force )
                {
                    pChoose->AddOption( new CXML::LazyCall( HookErrors( child, new CXML::GET( _force.str() ))), Compile( child, _state ));
                }
                else
                {
                    IChunkPtr pInner = Compile( child, _state );
                    IChunkPtr pCond = pInner->GetChunk( FOURCC_LIST_HEAD );
                    IChunkPtr pFunc = pInner->GetChunk( FOURCC_LIST_TAIL );
                    pChoose->AddOption( pCond.get(), pFunc.get() );
                }
            }
            else
            {
                if ( child->_name != "otherwise" ) { throw Useless::Error("<choose> May contain <when> and <otherwise>"); }
                pChoose->SetOtherwise( Compile( child, _state ));
            }
        }
        _state.SetResult( HookErrors( _node, pChoose.get() ));
        HookDefinition( _node, _state );
    }

    //---------------------------------------------------------

};//XMLFactory
