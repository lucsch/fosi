/***************************************************************************
 plint_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "plint_dlg.h"


PlInt_DLG::PlInt_DLG( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style, const wxString & name ) : wxDialog( parent, id, title, pos, size, style, name )
{
    _CreateControls();
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_DLG::OnClose ) );
	m_DipCtrl->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateDipCtrl ), NULL, this );
	m_EditPtsBtn->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( PlInt_DLG::OnEditPoints ), NULL, this );
	m_EditPtsBtn->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateUIEditPoints ), NULL, this );
}

PlInt_DLG::~PlInt_DLG()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( PlInt_DLG::OnClose ) );
	m_DipCtrl->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateDipCtrl ), NULL, this );
	m_EditPtsBtn->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( PlInt_DLG::OnEditPoints ), NULL, this );
    m_EditPtsBtn->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( PlInt_DLG::OnUpdateUIEditPoints ), NULL, this );
}



void PlInt_DLG::OnClose( wxCloseEvent& event ) {
    Destroy();
}



void PlInt_DLG::OnUpdateDipCtrl( wxUpdateUIEvent& event ) {
    event.Enable(m_2PointsCtrl->GetValue());
}



void PlInt_DLG::OnEditPoints( wxCommandEvent& event ) {

}



void PlInt_DLG::OnUpdateUIEditPoints( wxUpdateUIEvent& event ) {
    if (m_DemListCtrl->GetStringSelection().IsEmpty()) {
        event.Enable(false);
        return;
    }
    
    if (m_VectorListCtrl->GetStringSelection().IsEmpty()) {
        event.Enable(false);
        return;
    }
    
    event.Enable(true);
}



void PlInt_DLG::_CreateControls(){
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Input") ), wxHORIZONTAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Input DEM:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sbSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DemListCtrl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_DemListCtrl->SetMinSize( wxSize( 250,-1 ) );
	
	sbSizer1->Add( m_DemListCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer1->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Output") ), wxVERTICAL );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Line Output"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_VectorListCtrl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer2->Add( m_VectorListCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer2->Add( bSizer2, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options") ), wxVERTICAL );
	
	m_3PointsCtrl = new wxRadioButton( this, wxID_ANY, _("Use 3 points"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_3PointsCtrl->SetValue( true );
	sbSizer4->Add( m_3PointsCtrl, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	m_2PointsCtrl = new wxRadioButton( this, wxID_ANY, _("Use 2 points and a dip"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_2PointsCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_DipCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	bSizer9->Add( m_DipCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer4->Add( bSizer9, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer9;
	sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Operations") ), wxHORIZONTAL );
	
	m_LivePreviewCtrl = new wxCheckBox( this, wxID_ANY, _("Live preview"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer9->Add( m_LivePreviewCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer9->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_EditPtsBtn = new wxToggleButton( this, wxID_ANY, _("Edit Points"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer9->Add( m_EditPtsBtn, 0, wxALL, 5 );
	
	
	bSizer1->Add( sbSizer9, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer10;
	sbSizer10 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Information") ), wxHORIZONTAL );
	
	m_DirTxtCtrl = new wxStaticText( this, wxID_ANY, _("Direction:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DirTxtCtrl->Wrap( -1 );
	sbSizer10->Add( m_DirTxtCtrl, 0, wxALL, 5 );
	
	
	sbSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_DipTxtCtrl = new wxStaticText( this, wxID_ANY, _("Dip:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DipTxtCtrl->Wrap( -1 );
	sbSizer10->Add( m_DipTxtCtrl, 0, wxALL, 5 );
	
	
	sbSizer10->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_PtsTxtCtrl = new wxStaticText( this, wxID_ANY, _("Points:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PtsTxtCtrl->Wrap( -1 );
	sbSizer10->Add( m_PtsTxtCtrl, 0, wxALL, 5 );
	
	
	bSizer1->Add( sbSizer10, 0, wxEXPAND|wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	//this->Centre( wxBOTH );
}
