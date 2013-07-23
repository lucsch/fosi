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
#include "plintoperation.h"


PlInt_DLG::PlInt_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString & name ) : wxFrame( parent, id, title, pos, size, style, name )
{
    m_ViewerLayerManager = viewermanager;
    m_DemListCtrlSelectedText = wxEmptyString;
    m_VectorListCtrlSelectedText = wxEmptyString;
    
    _CreateControls();
	
    // Connect Events
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler( PlInt_DLG::OnWindowFocus ) );
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_DLG::OnClose ) );
	m_EditPtsBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlInt_DLG::OnCreatePlaneIntersection ), NULL, this );
	m_EditPtsBtn->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateUICreateIntersection ), NULL, this );
}

PlInt_DLG::~PlInt_DLG()
{
	// Disconnect Events
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler( PlInt_DLG::OnWindowFocus ) );
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_DLG::OnClose ) );
	m_EditPtsBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlInt_DLG::OnCreatePlaneIntersection ), NULL, this );
	m_EditPtsBtn->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateUICreateIntersection ), NULL, this );
}



void PlInt_DLG::OnClose( wxCloseEvent& event ) {
    m_ViewerLayerManager->GetDisplay()->ClearOverlayArray();
    m_ViewerLayerManager->GetDisplay()->Refresh();
    m_ViewerLayerManager->GetDisplay()->Update();
    Destroy();
}



void PlInt_DLG::OnUpdateUICreateIntersection( wxUpdateUIEvent& event ) {
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


void PlInt_DLG::OnCreatePlaneIntersection( wxCommandEvent& event ) {
    
    vrViewerDisplay * myDisplay = m_ViewerLayerManager->GetDisplay();
    PlIntViewerOverlay * myOverlay = static_cast<PlIntViewerOverlay*>(myDisplay->GetOverlayByName(PLINT_OVERLAY_NAME));
    wxASSERT(myDisplay);
        // create overlay if not existing
        if (myOverlay == NULL) {
            myOverlay = new PlIntViewerOverlay (myDisplay);
            myDisplay->GetOverlayArrayRef()->Add(myOverlay);
        }
        myOverlay->SetVisible(true);
        myOverlay->ClearPoints();
 

    // display tool dialog and hide this one
    Hide();
    
    PlInt_Tool_DLG * myToolDlg = new PlInt_Tool_DLG(this);
    myToolDlg->Show();
    
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
	
	
	sbSizer4->Add( bSizer9, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	m_EditPtsBtn = new wxButton( m_panel1, wxID_ANY, _("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_EditPtsBtn, 0, wxALL, 5 );
	
	
	m_panel1->SetSizer( bSizer1 );
	m_panel1->Layout();
	bSizer1->Fit( m_panel1 );
	bSizer4->Add( m_panel1, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer4 );
	this->Layout();
	bSizer4->Fit( this );
}








PlInt_Tool_DLG::PlInt_Tool_DLG( PlInt_DLG * parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString & name ) : wxFrame( parent, id, title, pos, size, style, name ){
    m_ParentDlg = parent;
    m_Operation = NULL;
    m_Overlay = static_cast<PlIntViewerOverlay*>(m_ParentDlg->m_ViewerLayerManager->GetDisplay()->GetOverlayByName(PLINT_OVERLAY_NAME));
    wxASSERT(m_Overlay);
    _CreateControls();
    
    vrRenderer * myRenderer = m_ParentDlg->m_ViewerLayerManager->GetRenderer(m_ParentDlg->m_DemListCtrl->GetStringSelection());
    wxASSERT(myRenderer);
    vrLayerRasterGDAL * myLayer = static_cast<vrLayerRasterGDAL* >(myRenderer->GetLayer());
    m_Operation = new PlIntOperation(myLayer, 0);
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_Tool_DLG::OnClose ) );
	this->Connect( wxEVT_IDLE, wxIdleEventHandler( PlInt_Tool_DLG::OnIdleProcess ) );
	m_EditPtsBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlInt_Tool_DLG::OnEditPoints ), NULL, this );
	m_ClearPtsBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlInt_Tool_DLG::OnClearPoints ), NULL, this );
}



PlInt_Tool_DLG::~PlInt_Tool_DLG()
{
    wxDELETE(m_Operation);
    
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_Tool_DLG::OnClose ) );
	this->Disconnect( wxEVT_IDLE, wxIdleEventHandler( PlInt_Tool_DLG::OnIdleProcess ) );
	m_EditPtsBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlInt_Tool_DLG::OnEditPoints ), NULL, this );
	m_ClearPtsBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlInt_Tool_DLG::OnClearPoints ), NULL, this );
	
}


void PlInt_Tool_DLG::OnClose( wxCloseEvent& event ) {
    GetParent()->Show();
    Destroy();
}



void PlInt_Tool_DLG::OnIdleProcess( wxIdleEvent& event ) {
    if (m_Operation == NULL) {
        return;
    }
    
    double dip  = 0;
    double azimut = 0;
    if(m_Operation->GetPlaneInfo(dip, azimut) == false){
        return;
    }
    
    
    

}


void PlInt_Tool_DLG::OnEditPoints( wxCommandEvent& event ) {
    vrViewerDisplay * myDisplay = m_ParentDlg->m_ViewerLayerManager->GetDisplay();
    wxASSERT(myDisplay);
    PlIntToolEdit * myEditTool  = new PlIntToolEdit (myDisplay, m_Overlay);
    myDisplay->SetTool(myEditTool);
}


void PlInt_Tool_DLG::OnClearPoints( wxCommandEvent& event ) {
    m_Overlay->ClearPoints(true);
}


void PlInt_Tool_DLG::_CreateControls(){
 	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxPanel* m_panel2;
	m_panel2 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( m_panel2, wxID_ANY, _("Information") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText7;
	m_staticText7 = new wxStaticText( m_panel2, wxID_ANY, _("Direction:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL, 5 );
	
	m_DirTxtCtrl = new wxStaticText( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_DirTxtCtrl->Wrap( -1 );
	fgSizer1->Add( m_DirTxtCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxStaticText* m_DipTxtCtrl;
	m_DipTxtCtrl = new wxStaticText( m_panel2, wxID_ANY, _("Dip:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DipTxtCtrl->Wrap( -1 );
	fgSizer1->Add( m_DipTxtCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DipCtrl = new wxSpinCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 200,-1 ), wxSP_ARROW_KEYS, 0, 10, 0 );
	fgSizer1->Add( m_DipCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
    m_DipCtrl->Enable(m_ParentDlg->m_2PointsCtrl->GetValue());
	
	wxStaticText* m_staticText8;
	m_staticText8 = new wxStaticText( m_panel2, wxID_ANY, _("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer1->Add( m_staticText8, 0, wxALL, 5 );
	
	m_PtsTxtCtrl = new wxStaticText( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_PtsTxtCtrl->Wrap( -1 );
	fgSizer1->Add( m_PtsTxtCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	
	sbSizer10->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	
	bSizer6->Add( sbSizer10, 0, wxEXPAND|wxALL, 5 );
	
	m_LivePreviewCtrl = new wxCheckBox( m_panel2, wxID_ANY, _("Live preview"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_LivePreviewCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	m_EditPtsBtn = new wxButton( m_panel2, wxID_ANY, _("Edit Points"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_EditPtsBtn, 0, wxALL, 5 );
	
	
	bSizer7->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_ClearPtsBtn = new wxButton( m_panel2, wxID_ANY, _("Clear Points"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_ClearPtsBtn, 0, wxALL, 5 );
	
	
	bSizer6->Add( bSizer7, 0, wxEXPAND, 5 );
	
	
	m_panel2->SetSizer( bSizer6 );
	m_panel2->Layout();
	bSizer6->Fit( m_panel2 );
	bSizer5->Add( m_panel2, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer5 );
	this->Layout();
	bSizer5->Fit( this );
}






