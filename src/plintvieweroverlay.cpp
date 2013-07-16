/***************************************************************************
 plintvieweroverlay
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plintvieweroverlay.h"
#include "vrviewerdisplay.h"
#include "vrcoordinate.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(wxRealPointList);


PlIntViewerOverlay::PlIntViewerOverlay(vrViewerDisplay * viewer, const wxString & name) : vrViewerOverlay(name) {
    m_Display = viewer;
}



PlIntViewerOverlay::~PlIntViewerOverlay() {
    ClearPoints();
}


wxPoint PlIntViewerOverlay::GetPixelPoint (int index){
    wxPoint myReturnPxPt = wxDefaultPosition;
    if (index < 0 || index  >= m_Pts.GetCount() ) {
        wxLogError(_("Index out of bounds!"));
        return myReturnPxPt;
    }
    
    wxASSERT(m_Display);
    m_Display->GetCoordinate()->ConvertToPixels(*m_Pts[index], myReturnPxPt);
    return myReturnPxPt;
}



bool PlIntViewerOverlay::AddPixelPoint (wxPoint pxpoint, bool refresh){
    if (m_Pts.GetCount() >= 2) {
        return false;
    }
    
    wxPoint2DDouble myRealPt = wxDefaultPosition;
    wxASSERT(m_Display);
    m_Display->GetCoordinate()->ConvertFromPixels(pxpoint, myRealPt);
    m_Pts.push_back(new wxPoint2DDouble(myRealPt));
    
    if (refresh == true){
        m_Display->Refresh();
        m_Display->Update();
    }
    
    return true;
}



void PlIntViewerOverlay::ClearPoints (bool refresh){
    m_Pts.DeleteContents(true);
    m_Pts.clear();
    
    if (refresh == true) {
        m_Display->Refresh();
        m_Display->Update();
    }
}



bool PlIntViewerOverlay::DrawOverlay(wxPaintDC * dc) {
    wxASSERT(dc);
    wxPen myPen (PLINT_OVERLAY_COLOR, PLINT_OVERLAY_SIZE);
    dc->SetPen(myPen);
    
    
    for (unsigned int i = 0; i< m_Pts.GetCount(); i++) {
        wxPoint myPt = GetPixelPoint(i);
        if (myPt == wxDefaultPosition) {
            continue;
        }
#ifdef __WXMSW__
        dc->DrawLine(myPt.x, myPt.y, myPt.x + 0.1, myPt.y + 0.1);
#else
        dc->DrawLine(myPt.x, myPt.y, myPt.x, myPt.y);
#endif
    }
    return true;
}
