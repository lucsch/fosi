/***************************************************************************
 profileview_dlg
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PROFILE_VIEW_DLG_H_
#define _PROFILE_VIEW_DLG_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/gdicmn.h>


#include "vroomgis.h"
#include "vrlayervector.h"


class ProfileViewData {
public:
    vrLayerVectorOGR * m_ProfileSource;
    vrArrayLayer m_RasterLayers;
    
    ProfileViewData() {
        m_ProfileSource = NULL;
    }
};


class ProfileView_DLG : public wxFrame
{
public:
    ProfileView_DLG( wxWindow* parent, vrLayerManager * layermanager, vrLayer * profile, const vrArrayLayer & raster, wxWindowID id = wxID_ANY, const wxString& title = _("Profile View"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    ~ProfileView_DLG();
    
private:
    vrLayerManager * m_LayerManagerRef;
    vrViewerLayerManager * m_ViewerLayerManager;
    vrViewerDisplay * m_Display;
    wxKeyboardState m_KeyBoardState;
    
    wxArrayString m_CreatedProfileName;
    
    void _CreateControls();
    void _CreateMenu();
    void _CreateToolBar();
    
    void OnClose( wxCloseEvent& event ) ;
    void OnZoomToFit( wxCommandEvent& event ) ;
    void OnToolZoom ( wxCommandEvent & event );
    void OnToolPan ( wxCommandEvent & event );
    
    void OnToolZoomAction ( wxCommandEvent & event );
    void OnToolPanAction ( wxCommandEvent & event );
    
    void OnKeyDown(wxKeyEvent & event);
    void OnKeyUp(wxKeyEvent & event) ;
    
    void OnCloseMenu( wxCommandEvent& event ) ;
    
    DECLARE_EVENT_TABLE();

};


#endif


