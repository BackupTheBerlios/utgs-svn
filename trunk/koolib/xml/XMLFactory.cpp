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
  last mod: $Id: XMLFactory.cpp,v 1.1.1.1 2004/06/02 19:35:52 koolas Exp $

 ********************************************************************/


#include "XMLFactory.h"
#include "XMLFactoryUtil.h"
#include "Useless/System/DbgOut.h"

namespace XMLFactory {
    
    int OnXMLTagDictInit( const std::string &name )
    {
        Useless::print_dbg("Initialized XML Tag Dictionary for: %s\n", name.c_str());
        if ( FILE *f = fopen("koolib.log", "r"))
        {
            fclose(f);
            f = fopen("koolib.log", "a+");
            fprintf( f, "Initialized XML Tag Dictionary for: %s\n", name.c_str());
            fclose(f);
        }
        return 0;
    }

};


namespace Debug {

    const char * get_xml_node_name( XMLFactory::Node *node )
    {
        return ((*node)->_name.c_str());
    }

    const wchar_t * get_xml_node_value( XMLFactory::Node *node )
    {
        return ((*node)->_value.c_str());
    }

    void do_list_xml_node( XMLFactory::Node *node, const char *leadString )
    {
        Useless::print_dbg("%sXML node:\n%s\tname : \"%s\"\n%s\tvalue : \"%S\"\n%s\tattibutes :%s\n"
                , leadString, leadString
                , (*node)->_name.c_str(), leadString, (*node)->_value.c_str()
                , leadString, leadString );

        for ( Useless::XMLAttributes::const_iterator
                itAttr = (*node)->_attributes.begin();
                itAttr != (*node)->_attributes.end();
                ++itAttr )
        {
            Useless::print_dbg("%s\t\t\"%s\" => \"%s\"\n", leadString, (*itAttr).first.c_str(), (*itAttr).second.c_str() );
        }
        Useless::print_dbg("\n");
    }

    void do_list_xml_subtree( XMLFactory::Node *pnode, std::string leadString, int numLevels )
    {
        XMLFactory::Node node( *pnode );
        for ( node.StepInto(); !!node; ++node )
        {
            do_list_xml_node( &node, leadString.c_str() );
            if ( numLevels > 0 )
            {
                do_list_xml_subtree( &node, leadString+"\t", numLevels-1 );
            }
        }
    }

    void list_xml_node( XMLFactory::Node *node )
    {
        do_list_xml_node( node, "" );
    }

    void list_xml_subtree( XMLFactory::Node *node, int numLevels )
    {
        do_list_xml_node( node, "" );
        if ( numLevels > 0 )
        {
            do_list_xml_subtree( node, "\t", numLevels-1 );
        }
    }



};//XMLFactory
