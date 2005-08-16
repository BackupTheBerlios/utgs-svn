#ifndef __INCLUDED_USELESS_BUTTON_WIDGET_H__
#define __INCLUDED_USELESS_BUTTON_WIDGET_H__

/*
 *    $Id: ButtonWidget.h,v 1.9 2002/09/12 14:27:36 koolas Exp $
 *
 *    NAME
 *        ButtonWidget
 *
 *    PURPOSE
 *        Generic graphic button (NOT resizeable)
 *    
 *    AUTHOR(S)
 *        KooLAS <koolas@iavision.com>
 * 
*/

#pragma warning(disable:4250) // inherit via dominance
#include "Useless/GUI/Widgets/TemplateWidget.h"
#include "Useless/GUI/Widgets/FacedWidget.h"
#include "Useless/GUI/Logic/ButtonLogic.h"
#include "Useless/GUI/Logic/KeyButtonLogic.h"

/*! \ingroup Widgets
 *  Just a simple Button widget.
 */
namespace Useless {

typedef TemplateWidget< FacedWidget, ButtonLogic > NoKeyButton;
typedef TemplateWidget< NoKeyButton, KeyButtonLogic > ButtonWidget;

};//namespace Useless

#endif//__INCLUDED_USELESS_BUTTON_WIDGET_H__
