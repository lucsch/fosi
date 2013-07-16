/***************************************************************************
 profileview_dlg
 -------------------
 copyright            : (C) 2013 CREALP Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "profileview_dlg.h"



ProfileView_DLG::ProfileView_DLG( wxWindow* parent, vrLayerManager * layermanager, vrLayer * profile, const vrArrayLayer & raster, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style ){
    m_LayerManagerRef = layermanager;
    m_Display = NULL;
    
    _CreateControls();
    
    m_ViewerLayerManager = new vrViewerLayerManager(m_LayerManagerRef, this, m_Display, NULL);
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ProfileView_DLG::OnClose ) );
}



ProfileView_DLG::~ProfileView_DLG(){
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( ProfileView_DLG::OnClose ) );
	
}



void ProfileView_DLG::OnClose( wxCloseEvent& event ) {
    m_LayerManagerRef->RemoveViewerLayerManager(m_ViewerLayerManager);
    Destroy();
}



void ProfileView_DLG::_CreateControls(){
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxPanel* m_panel1;
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_Display = new vrViewerDisplay( m_panel1);
	bSizer2->Add( m_Display, 1, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	wxStatusBar* m_statusBar1 = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
    m_Display->SetStatusCoordinates(m_statusBar1, 0, _("No coordinates"));
    m_Display->SetNoDataText(_("No Profile Data"));
}



