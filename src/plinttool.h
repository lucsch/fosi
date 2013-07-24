/***************************************************************************
 plinttool.h
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PLINTTOOL_H_
#define _PLINTTOOL_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/overlay.h>
#include "vrdisplaytool.h"

const int PLINT_TOOL_ID = wxID_HIGHEST + 30;

class vrViewerDisplay;
class PlIntViewerOverlay;

/*************************************************************************************//**
@brief Plane intersection editing tool
@author Lucien Schreiber 
@date 04 july 2013
 *****************************************************************************************/
class PlIntToolEdit : public vrDisplayTool {
private:
    PlIntViewerOverlay * m_Overlay;
    
public:
    PlIntToolEdit(vrViewerDisplay * display, PlIntViewerOverlay * overlay, int toolid = PLINT_TOOL_ID);
    virtual ~PlIntToolEdit();
    
    virtual bool MouseDown(const wxMouseEvent & event){return false;}
    virtual bool MouseUp(const wxMouseEvent & event);
    virtual bool MouseMove(const wxMouseEvent & event);
};

#endif


