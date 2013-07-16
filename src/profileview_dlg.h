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

#include "vroomgis.h"

class ProfileView_DLG : public wxFrame
{
public:
    ProfileView_DLG( wxWindow* parent, vrLayerManager * layermanager, wxWindowID id = wxID_ANY, const wxString& title = _("Profile View"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
    ~ProfileView_DLG();
    
private:
    vrLayerManager * m_LayerManagerRef;
    vrViewerLayerManager * m_ViewerLayerManager;
    vrViewerDisplay * m_Display;
    
    void _CreateControls();
    
    void OnClose( wxCloseEvent& event ) ;
};


#endif


