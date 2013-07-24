/***************************************************************************
 plinttool.h
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plinttool.h"
#include "plintvieweroverlay.h"
#include "vrviewerdisplay.h"
#include "vroomgis_bmp.h"

PlIntToolEdit::PlIntToolEdit(vrViewerDisplay * display, PlIntViewerOverlay * overlay, int toolid) {
    m_Overlay = overlay;
    wxImage myEditCursorImg = _img_cursor_editing->ConvertToImage();
    myEditCursorImg.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 8);
    myEditCursorImg.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 8);
    Create(display, toolid, "Editing", wxCursor(myEditCursorImg));
}



PlIntToolEdit::~PlIntToolEdit() {
}




bool PlIntToolEdit::MouseUp(const wxMouseEvent & event) {
    wxASSERT(m_Overlay);
    m_Overlay->AddPixelPoint(event.GetPosition(), true);
    return true;
}


bool PlIntToolEdit::MouseMove(const wxMouseEvent & event){
    wxASSERT(m_Overlay);
    if (m_Overlay->GetPointCount() != 2) {
        return true;
    }
    
    m_Overlay->UpdatePixelPoint(event.GetPosition(), 2);
    return true;
}

