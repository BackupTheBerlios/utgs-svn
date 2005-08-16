#ifndef __INCLUDED_USELESS_XML_CONNECTIONS_H__
#define __INCLUDED_USELESS_XML_CONNECTIONS_H__

#include "Useless/XML/Resources/CreateFromXML.h"

namespace Useless {

Resource* CreateConnectionBase  ( XMLIterator i, XMLCreatorEnv *env);
Resource* XMLTieAction          ( XMLIterator i, XMLCreatorEnv *env);
Resource* CreateMultiTab        ( XMLIterator i, XMLCreatorEnv *env);
Resource* CreateScrollRelation  ( XMLIterator i, XMLCreatorEnv *env);
Resource* CreateShowColumns     ( XMLIterator i, XMLCreatorEnv *env);

static RegisterXMLCreator reg_create_connection_base(
        "connections::base", &CreateConnectionBase);

static RegisterXMLCreator reg_xml_on_click(
        "connections::onclick", &XMLTieAction);

static RegisterXMLCreator reg_xml_on_key(
        "connections::onkey", &XMLTieAction);

static RegisterXMLCreator reg_xml_handler(
        "connections::handler", &XMLTieAction);

static RegisterXMLCreator reg_xml_multitab(
        "connections::multitab", &CreateMultiTab);

static RegisterXMLCreator reg_xml_scroll_relation(
        "connections::scroll", &CreateScrollRelation);

static RegisterXMLCreator reg_xml_show_columns(
        "connections::show_columns", &CreateShowColumns);


// Multi-screen-menu concept
void SetCurrentScreen   ( class Widget * main_widget );
    
};//namespace Useless
#endif//__INCLUDED_USELESS_XML_CONNECTIONS_H__
