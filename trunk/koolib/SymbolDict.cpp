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
last mod: $Id: koolas Exp $

 ********************************************************************/

#include <koolib/SymbolDict.h>
#include <koolib/FourCC.h>

std::wstring SymbolDict::s_NoSymbol;

SymbolDict::SymbolDict()
{
    AddSymbol( L"head", FOURCC_LIST_HEAD );
    AddSymbol( L"tail", FOURCC_LIST_TAIL );
}

SymbolDict::~SymbolDict()
{
}

unsigned int
SymbolDict::AddSymbol    ( const std::wstring &name, unsigned int preferedId )
{
    Names::iterator itName = _names.find( name );
    
    if ( itName != _names.end() )
    {
        return (*itName).second;
    }
    
    if ( NO_SYMBOL == preferedId )
    {
        wchar_t fchars[5] = {0,};
        std::memcpy( fchars, &name[0], std::min( (int)name.size(), (int)4 ));
        preferedId = Make4CC( fchars );
    }
        
    Numbers::iterator itNum = _numbers.find( preferedId );

    while ( itNum != _numbers.end() )
    {
        itNum = _numbers.find( ++preferedId );
    }

    _numbers[ preferedId ] = name;
    _names[ name ] = preferedId;

    return preferedId;
}

const std::wstring &
SymbolDict::GetSymbolName( unsigned int symbolId ) const
{
    Numbers::const_iterator it = _numbers.find( symbolId );

    if ( _numbers.end() == it )
    {
        return s_NoSymbol;
    }
    else
    {
        return (*it).second;
    }
}
    
unsigned int
SymbolDict::GetSymbolId  ( const std::wstring &name ) const
{
    Names::const_iterator it = _names.find( name );

    if ( _names.end() == it )
    {
        return 0;
    }
    else
    {
        return (*it).second;
    }
}

