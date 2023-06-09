/***************************************************************************
 plintvieweroverlay
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PLINT_VIEWER_OVERLAY_H_
#define _PLINT_VIEWER_OVERLAY_H_

#include "vrvieweroverlay.h"

const wxString PLINT_OVERLAY_NAME = _T("PLINT_OVERLAY");
const wxColour PLINT_OVERLAY_COLOR = *wxGREEN;
const wxColour PLING_OVERLAY_COLOR2 = *wxBLUE;
const int PLINT_OVERLAY_SIZE = 6;

WX_DECLARE_LIST(wxPoint2DDouble, wxRealPointList);


class PlIntViewerOverlay : public vrViewerOverlay {
private:
    wxRealPointList m_Pts;
    vrViewerDisplay * m_Display;
    long m_AddedPoints;
    
public:
    PlIntViewerOverlay(vrViewerDisplay * viewer, const wxString & name = PLINT_OVERLAY_NAME);
    virtual ~PlIntViewerOverlay();
    
    wxPoint GetPixelPoint (int index);
    wxPoint2DDouble GetRealPoint (int index);
    bool AddPixelPoint (wxPoint pxpoint, bool refresh = false);
    bool UpdatePixelPoint (wxPoint pxpoint, int index);
    void ClearPoints (bool refresh = false);
    long GetPointCount () {return m_Pts.GetCount();}
    long GetPointAdded () {return m_AddedPoints;}
    
    virtual bool DrawOverlay(wxPaintDC * dc);
};


#endif


