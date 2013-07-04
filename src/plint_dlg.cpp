/***************************************************************************
 plint_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plint_dlg.h"
#include "vroomgis.h"
#include "vrlayervector.h"
#include "vrlayerraster.h"
#include "plintvieweroverlay.h"
#include "plinttool.h"

PlInt_DLG::PlInt_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString & name ) : wxFrame( parent, id, title, pos, size, style, name )
{
    m_ViewerLayerManager = viewermanager;
    m_DemListCtrlSelectedText = wxEmptyString;
    m_VectorListCtrlSelectedText = wxEmptyString;
    
    _CreateControls();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_DLG::OnClose ) );
    this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( PlInt_DLG::OnWindowFocus ) );
	m_DipCtrl->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateDipCtrl ), NULL, this );
	m_EditPtsBtn->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( PlInt_DLG::OnEditPoints ), NULL, this );
	m_EditPtsBtn->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateUIEditPoints ), NULL, this );
}

PlInt_DLG::~PlInt_DLG()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_DLG::OnClose ) );
    this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( PlInt_DLG::OnWindowFocus ) );
	m_DipCtrl->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateDipCtrl ), NULL, this );
	m_EditPtsBtn->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( PlInt_DLG::OnEditPoints ), NULL, this );
    m_EditPtsBtn->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateUIEditPoints ), NULL, this );
}



void PlInt_DLG::OnClose( wxCloseEvent& event ) {
    m_ViewerLayerManager->GetDisplay()->ClearOverlayArray();
    m_ViewerLayerManager->GetDisplay()->Refresh();
    m_ViewerLayerManager->GetDisplay()->Update();
    Destroy();
}



void PlInt_DLG::OnUpdateDipCtrl( wxUpdateUIEvent& event ) {
    event.Enable(m_2PointsCtrl->GetValue());
}



void PlInt_DLG::OnEditPoints( wxCommandEvent& event ) {
    m_DemListCtrl->Enable(!event.IsChecked());
    m_VectorListCtrl->Enable(!event.IsChecked());
    m_3PointsCtrl->Enable(!event.IsChecked());
    m_2PointsCtrl->Enable(!event.IsChecked());
    
    vrViewerDisplay * myDisplay = m_ViewerLayerManager->GetDisplay();
    PlIntViewerOverlay * myOverlay = static_cast<PlIntViewerOverlay*>(myDisplay->GetOverlayByName(PLINT_OVERLAY_NAME));
    wxASSERT(myDisplay);
    if (event.IsChecked() == true) { // start edition
        // create overlay if not existing
        if (myOverlay == NULL) {
            myOverlay = new PlIntViewerOverlay (myDisplay);
            myDisplay->GetOverlayArrayRef()->Add(myOverlay);
        }        
        myOverlay->SetVisible(true);
        myOverlay->ClearPoints();
        myDisplay->SetTool(new PlIntToolEdit(myDisplay, myOverlay));
    }
    else {
        wxASSERT(myOverlay);
        myOverlay->SetVisible(false);
        myDisplay->SetToolDefault();
    }
    myDisplay->Refresh();
    myDisplay->Update();
}



void PlInt_DLG::OnUpdateUIEditPoints( wxUpdateUIEvent& event ) {
    if (m_DemListCtrl->GetStringSelection() == wxEmptyString) {
        event.Enable(false);
        return;
    }
    
    if (m_VectorListCtrl->GetStringSelection() == wxEmptyString) {
        event.Enable(false);
        return;
    }
    
    event.Enable(true);
}


void PlInt_DLG::OnWindowFocus( wxActivateEvent& event ) {
    if (event.GetActive() == false) {
        return;
    }
    
    if (m_VectorListCtrl->IsEnabled() && m_DemListCtrl->IsEnabled()) {
        m_DemListCtrlSelectedText = m_DemListCtrl->GetStringSelection();
        m_VectorListCtrlSelectedText = m_VectorListCtrl->GetStringSelection();
        
        m_DemListCtrl->Clear();
        m_VectorListCtrl->Clear();
        
        // load layername into controls
        wxASSERT(m_ViewerLayerManager);
        for (unsigned int i = 0; i< m_ViewerLayerManager->GetCount(); i++) {
            vrLayer * myLayer =  m_ViewerLayerManager->GetRenderer(i)->GetLayer();
            wxASSERT(myLayer);
            
            if (myLayer->GetType() > vrDRIVER_UNKNOWN  && myLayer->GetType() <= vrDRIVER_VECTOR_MEMORY) {
                // add polygon mask
                vrLayerVector * myLayerVector = static_cast<vrLayerVector*>(myLayer);
                if (wkbFlatten(myLayerVector->GetGeometryType()) == wkbLineString) {
                    m_VectorListCtrl->Append(myLayerVector->GetDisplayName().GetFullPath());
                }
            }
            
            else if (myLayer->GetType() > vrDRIVER_VECTOR_MEMORY && myLayer->GetType() <= vrDRIVER_RASTER_SGRD7){
                m_DemListCtrl->Append(myLayer->GetDisplayName().GetFullPath());
            }
        }
        
        m_DemListCtrl->SetSelection(m_DemListCtrl->FindString(m_DemListCtrlSelectedText));
        m_VectorListCtrl->SetSelection(m_VectorListCtrl->FindString(m_VectorListCtrlSelectedText));
    }
}




void PlInt_DLG::_CreateControls(){
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	wxPanel* m_panel1;
	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Input") ), wxHORIZONTAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( m_panel1, wxID_ANY, _("Input DEM:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sbSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DemListCtrl = new wxComboBox( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_DemListCtrl->SetMinSize( wxSize( 250,-1 ) );
	
	sbSizer1->Add( m_DemListCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Output") ), wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( m_panel1, wxID_ANY, _("Line Output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_VectorListCtrl = new wxComboBox( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer2->Add( m_VectorListCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer2->Add( bSizer2, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Options") ), wxVERTICAL );
	
	m_3PointsCtrl = new wxRadioButton( m_panel1, wxID_ANY, _("Use 3 points"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_3PointsCtrl->SetValue( true );
	sbSizer4->Add( m_3PointsCtrl, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_2PointsCtrl = new wxRadioButton( m_panel1, wxID_ANY, _("Use 2 points and a dip"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_2PointsCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DipCtrl = new wxSpinCtrl( m_panel1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	bSizer9->Add( m_DipCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer4->Add( bSizer9, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Operations") ), wxHORIZONTAL );
	
	m_LivePreviewCtrl = new wxCheckBox( m_panel1, wxID_ANY, _("Live preview"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer9->Add( m_LivePreviewCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer9->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_EditPtsBtn = new wxToggleButton( m_panel1, wxID_ANY, _("Edit Points"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer9->Add( m_EditPtsBtn, 0, wxALL, 5 );
	
	
	bSizer1->Add( sbSizer9, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panel1, wxID_ANY, _("Information") ), wxHORIZONTAL );
	
	m_DirTxtCtrl = new wxStaticText( m_panel1, wxID_ANY, _("Direction:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DirTxtCtrl->Wrap( -1 );
	sbSizer10->Add( m_DirTxtCtrl, 0, wxALL, 5 );
	
	
	sbSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_DipTxtCtrl = new wxStaticText( m_panel1, wxID_ANY, _("Dip:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DipTxtCtrl->Wrap( -1 );
	sbSizer10->Add( m_DipTxtCtrl, 0, wxALL, 5 );
	
	
	sbSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_PtsTxtCtrl = new wxStaticText( m_panel1, wxID_ANY, _("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PtsTxtCtrl->Wrap( -1 );
	sbSizer10->Add( m_PtsTxtCtrl, 0, wxALL, 5 );
	
	
	bSizer1->Add( sbSizer10, 0, wxEXPAND|wxALL, 5 );
	
	
	m_panel1->SetSizer( bSizer1 );
	m_panel1->Layout();
	bSizer1->Fit( m_panel1 );
	bSizer4->Add( m_panel1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	bSizer4->Fit( this );
}
