#ifndef __INCLUDED_USELESS_NAMEDFONTSET_H__
#define __INCLUDED_USELESS_NAMEDFONTSET_H__

#include "Useless/GUI/Text/FontSet.h"

namespace Useless {

class NamedFontSet : public FontSet
{
public:
              NamedFontSet();
     virtual ~NamedFontSet();

        void  Insert( const Text &symbolic_name, const Font &font );

        //! Method will overwrite previous name
        void    SetName( int id, const Text &symbolic_name );
         int  GetByName(         const Text &symbolic_name ) const;


        void  Remove( const Text &symbolic_name )          { Remove(GetByName(symbolic_name)); }

        bool  Exists( const Text &symbolic_name )  const;

        Font &GetFont( const Text &symbolic_name )         { return GetFont(GetByName(symbolic_name)); }
  const Font &GetFont( const Text &symbolic_name ) const   { return GetFont(GetByName(symbolic_name)); }

        void  Paint( const Text          &symbolic_name, 
                     const Painter       &painter,
                     const Text          &text,
                     const Pos           &pos     =Pos()
                   )                        { Paint( GetByName(symbolic_name), painter, text, pos ); }

         int  GetWidth ( const Text &sn, const Text& text) const { return GetWidth(GetByName(sn), text); }
         int  GetWidth ( const Text &sn, UniCode code)     const { return GetWidth(GetByName(sn), code); }
         int  GetHeight( const Text &sn)                   const { return GetHeight(GetByName(sn));      }



private:
    typedef AssocVector< Text, int> NameMap;
    NameMap                         _symbolic_names;
};

};//namespace Useless

#endif//__INCLUDED_USELESS_NAMEDFONTSET_H__
