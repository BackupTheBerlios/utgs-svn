#include <koolib/xml/XMLModuleBuiltIn.h>
#include <koolib/xml/XMLChunksMethod.h>
#include <koolib/Random.h>
#include <cmath>
#define M_PI 3.14159265358979323846

namespace XMLProgram {
    
    namespace XML_IMPORT
    {
        /*! Mathematical functions
         */
        struct MATH : XMLCodeBlock
        {
            MATH()
            {
                add_methods( this )
                    .def("sin", sin, "x")
                    .def("cos", cos, "x")
                    .def("asin", asin, "x")
                    .def("acos", acos, "x")
                    .def("atan2", atan2, "x", "y")
                    .def("fmin", fmin, "a", "b")
                    .def("fmax", fmax, "a", "b")
                    .def("min", min, "a", "b")
                    .def("max", max, "a", "b")
                    .def("ceil", ceil, "x")
                    .def("floor", floor, "x")
                    .def("mod", mod, "x", "n")
                    .def("log", log, "a", "x")
                    .def("exp", exp, "a", "x")
                    .def("sign", sign, "x")
                    .def("abs", abs, "x")
                    .add("PI", CreateValue( M_PI ))
                    .add("PI2", CreateValue( M_PI/2.0 ))
                    .add("PI4", CreateValue( M_PI/4.0 ))
                    .add("2PI", CreateValue( 2.0*M_PI ))
                    ;
            }

            double sin( double x ) { return std::sin( x ); }
            double cos( double x ) { return std::cos( x ); }
            double asin( double x ) { return std::asin( x ); }
            double acos( double x ) { return std::acos( x ); }
            double atan2( double x, double y ) { return std::atan2( y, x ); }
            double fmin( double a, double b ) { return std::min( a, b ); }
            double fmax( double a, double b ) { return std::max( a, b ); }
            int min( int a, int b ) { return std::min( a, b ); }
            int max( int a, int b ) { return std::max( a, b ); }
            int ceil( double x ) { return std::ceil( x ); }
            int floor( double x ) { return std::floor( x ); }
            double mod( double x, double n )
            {
                while ( x < 0 )
                {
                    x += n;
                }
                return (x - std::floor( x / n ) * n);
            }
            double exp( double a, double x )
            {
                return std::exp( x ) * std::log( a );
            }
            double log( double a, double x )
            {
                return std::log( x ) / std::log( a );
            }
            double abs( double x )
            {
                return (x < 0 ? -x : x);
            }
            double sign( double x )
            {
                return (x < 0 ? -1 : 1);
            }
        };

        /*! Randomizer functions
         */
        struct RANDOM : XMLCodeBlock
        {
            RANDOM()
            {
                add_methods( this )
                    .def("SampleRange", SampleRange, "first", "last" )
                    .def("Sample", Sample )
                    .def("Shuffle", Shuffle )
                    ;
            }

            int SampleRange( int first, int last )
            {
                if ( first + 1 < last )
                {
                    return Random::SampleRange( first, last );
                }
                else
                {
                    return first;
                }
            }

            void Sample( Node __unused__, ExecutionState &state )
            {
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                std::vector< IChunkPtr > objects;
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    objects.push_back( pList->GetChunk( FOURCC_LIST_HEAD ));
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                }
                int i = SampleRange( 0, objects.size() );
                state.SetResult( objects[i].get() );
            }

            void Shuffle( Node __unused__, ExecutionState &state )
            {
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                typedef std::vector< IChunkPtr > Objects; Objects objects;
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    objects.push_back( pList->GetChunk( FOURCC_LIST_HEAD ));
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                }
                Random::Shuffle( objects.begin(), objects.end() );
                IChunkPtr pPrev = CreateEmpty();
                for ( Objects::iterator it=objects.begin(); it!=objects.end(); ++it )
                {
                    pPrev = CreateList( (*it).get(), pPrev.get() );
                }
                state.SetResult( pPrev.get() );
            }
        };

        struct LIST : XMLCodeBlock
        {
            LIST()
            {
                add_methods( this )
                    .def("Size", Size )
                    .def("At", At )
                    .def("Sort", Sort )
                    .def("Reverse", Reverse )
                    .def("Last", Last )
                    ;
            }

            void Size( Node __unused__, ExecutionState &state )
            {
                int size = 0;
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    ++size;
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                }
                state.SetResult( CreateValue( size ));
            }

            void At( Node __unused__, ExecutionState &state )
            {
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                IChunkPtr pIndex = XMLProgram::GetChunk(L"index", state);
                int n = value_of< int >( pIndex.get() );
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    if ( 0 == n-- )
                    {
                        state.SetResult( pList->GetChunk( FOURCC_LIST_HEAD ));
                        return;
                    }
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                }
                state.SetResult( CreateEmpty() );
            }

            struct OpCmp
            {
                IChunkPtr _pPred;
                mutable SubScope  _newState;
                TextUtf8  _first;
                TextUtf8  _second;
                OpCmp( ExecutionState &state, IChunkPtr pPred, 
                        const TextUtf8 &first, const TextUtf8 &second )
                    : _pPred( pPred ), _newState( state ), _first( first ), _second( second )
                {
                }
                bool operator ()( const IChunkPtr &first, const IChunkPtr &second ) const
                {
                    _newState.Reuse();
                    _newState._currentBlock->AddChunk( _first, first.get() );
                    _newState._currentBlock->AddChunk( _second, second.get() );
                    _pPred->Execute( Node(), _newState );
                    return (1 == value_of< int >( _newState.GetResult() ));
                }
            };

            void Sort( Node __unused__, ExecutionState &state )
            {
                SubScope newState( state );
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                IChunkPtr pPred = XMLProgram::GetChunk(L"pred", state);
                IChunkPtr pFirst = XMLProgram::GetChunk(L"first", state);
                IChunkPtr pSecond = XMLProgram::GetChunk(L"second", state);
                IChunkPtr pReverse = XMLProgram::GetChunk(L"reverse", state);
                OpCmp op( state, pPred, value_of< TextUtf8 >( pFirst.get() ), value_of< TextUtf8 >( pSecond.get() ));
                typedef std::multiset< IChunkPtr, OpCmp > Bins; Bins bins( op );
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    bins.insert( pList->GetChunk( FOURCC_LIST_HEAD ));
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                    newState.Reuse();
                }
                IChunkPtr pPrev = CreateEmpty();
                if ( IsEmpty( pReverse.get() ) || 0 == value_of< int >( pReverse.get() ))
                {
                    for ( Bins::reverse_iterator it=bins.rbegin(); it!=bins.rend(); ++it )
                    {
                        pPrev = CreateList( (*it).get(), pPrev.get() );
                    }
                }
                else
                {
                    for ( Bins::iterator it=bins.begin(); it!=bins.end(); ++it )
                    {
                        pPrev = CreateList( (*it).get(), pPrev.get() );
                    }
                }
                state.SetResult( pPrev.get() );
            }

            void Reverse( Node __unused__, ExecutionState &state )
            {
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                IChunkPtr pPrev = CreateEmpty();
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    pPrev = CreateList( pList->GetChunk( FOURCC_LIST_HEAD ), pPrev.get() );
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                }
                state.SetResult( pPrev.get() );
            }

            void Last( Node __unused__, ExecutionState &state )
            {
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                IChunkPtr pTail = CreateEmpty();
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    pTail = pList;
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );                    
                }
                state.SetResult( pTail.get() );
            }
        };

        struct CHARACTER: XMLCodeBlock
        {
            CHARACTER()
            {
                add_methods( this )
                    .add("SP", CreateValue( TextUtf8(L" ") ))
                    .add("NL", CreateValue( TextUtf8(L"\n") ))
                    .add("CR", CreateValue( TextUtf8(L"\r") ))
                    .add("TS", CreateValue( TextUtf8(L"\t") ))
                    .def_remap_1< int, TextUtf8 >("ToInteger", ToInteger, "char")
                    .def("FromInteger", FromInteger, "index")
                    ;
            }

            int ToInteger( const TextUtf8 &x )
            {
                return x[0];
            }

            TextUtf8 FromInteger( int index )
            {
                TextUtf8 x; x += (wchar_t)index;
                return x;
            }
        };

        struct STRING: XMLCodeBlock
        {
            STRING()
            {
                add_methods( this )
                    .def_remap_1< int, TextUtf8 >("Length", Length, "text" )
                    .def_remap_2< TextUtf8, TextUtf8, int >("At", At, "text", "index" )
                    .def("Cat", Cat )
                    .def("find", find, "text", "pattern", "pos")
                    .def("rfind", rfind, "text", "pattern", "pos")
                    .def("substr", substr, "text", "start", "end")
                    ;
            }

            int Length( const TextUtf8 &text )
            {
                return text.length();
            }

            TextUtf8 At( const TextUtf8 &text, int index )
            {
                TextUtf8 x; if ( index < text.size() ) { x += text[ index ]; }
                return x;
            }

            void Cat( Node __unused__, ExecutionState &state )
            {
                IChunkPtr pList = XMLProgram::GetChunk(L"list", state);
                IChunkPtr pSepa = XMLProgram::GetChunk(L"sepa", state);
                TextUtf8 sepa;
                if ( !IsEmpty( pSepa.get() ))
                {
                    sepa = value_of< TextUtf8 >( pSepa.get() );
                }
                TextUtf8 result;
                while ( !IsEmpty( pList.get() ) )
                {
                    pList = Force( pList, state );
                    if ( IsEmpty( pList.get() )) { break; }
                    if ( !result.empty() ) { result += sepa; }
                    result += value_of< TextUtf8 >( pList->GetChunk( FOURCC_LIST_HEAD ) );
                    pList = pList->GetChunk( FOURCC_LIST_TAIL );
                }
                state.SetResult( CreateValue( result ));
            }

            int find( TextUtf8 text, TextUtf8 pattern, int pos )
            {
                return text.find( pattern, pos );
            }

            int rfind( TextUtf8 text, TextUtf8 pattern, int pos )
            {
                return text.rfind( pattern, pos );
            }

            TextUtf8 substr( TextUtf8 text, int start, int end )
            {
                return text.substr( start, end );
            }
        };

        // defined elsewhere
        struct KOMPILER;


    };// namespace XML_IMPORT
    
    USELESS_XML_TAG_DICT_DEFINE( xtdImportBuiltIn, ExecutionState );
    
    static USELESS_XML_TAG_DICT_Extend( xtdImportBuiltIn, ExecutionState, BasicProgram,
            .AddReg< XML_IMPORT::MATH       >("Math")
            .AddReg< XML_IMPORT::RANDOM     >("Random")
            .AddReg< XML_IMPORT::LIST       >("List")
            .AddReg< XML_IMPORT::CHARACTER  >("Character")
            .AddReg< XML_IMPORT::STRING     >("String")
            .AddReg< XML_IMPORT::KOMPILER   >("Kompiler")
            );
    
    static USELESS_XML_TAG_DICT_AddBase( xtdImport, ExecutionState, xtdImportBuiltIn );

};//namespace XMLProgram

namespace XMLFactory {
    using namespace XMLProgram;

#define DEFINE_MODULE_IMPORT( _modName ) USELESS_XML_TAG_HANDLER( _modName, ExecutionState ) \
{\
    _state.SetResult( new _modName() ); \
}

DEFINE_MODULE_IMPORT( XML_IMPORT::MATH );
DEFINE_MODULE_IMPORT( XML_IMPORT::RANDOM );
DEFINE_MODULE_IMPORT( XML_IMPORT::LIST );
DEFINE_MODULE_IMPORT( XML_IMPORT::CHARACTER );
DEFINE_MODULE_IMPORT( XML_IMPORT::STRING );

};//XMLFactory
