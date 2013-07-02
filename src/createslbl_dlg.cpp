/***************************************************************************
 createslbl_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "createslbl_dlg.h"
#include "vroomgis.h"
#include "vrlayervector.h"


CreateSLBL_DLG::CreateSLBL_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) :
wxDialog( parent, id, title, pos, size, style ){
    _CreateControls();
    
    wxASSERT(viewermanager);
    for (unsigned int i = 0; i< viewermanager->GetCount(); i++) {
        vrLayer * myLayer =  viewermanager->GetRenderer(i)->GetLayer();
        wxASSERT(myLayer);
        
        if (myLayer->GetType() > vrDRIVER_UNKNOWN  && myLayer->GetType() <= vrDRIVER_VECTOR_MEMORY) {
            // add polygon mask
            vrLayerVector * myLayerVector = static_cast<vrLayerVector*>(myLayer);
            if (wkbFlatten(myLayerVector->GetGeometryType()) == wkbPolygon) {
                m_MaskListCtrl->Append(myLayerVector->GetDisplayName().GetFullPath());
            }
        }
        
        else if (myLayer->GetType() > vrDRIVER_VECTOR_MEMORY && myLayer->GetType() <= vrDRIVER_RASTER_SGRD7){
            m_InputListCtrl->Append(myLayer->GetDisplayName().GetFullPath());
        }
    }
    
    
    m_InputBrowseBtnCtrl->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CreateSLBL_DLG::OnInputBrowseBtn ), NULL, this );
	m_MaskUseCtrl->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CreateSLBL_DLG::OnUseMaskCheck ), NULL, this );
	m_MaskBrowseBtnCtrl->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CreateSLBL_DLG::OnMaskBrowseBtn ), NULL, this );
}



CreateSLBL_DLG::~CreateSLBL_DLG(){
    m_InputBrowseBtnCtrl->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CreateSLBL_DLG::OnInputBrowseBtn ), NULL, this );
	m_MaskUseCtrl->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CreateSLBL_DLG::OnUseMaskCheck ), NULL, this );
	m_MaskBrowseBtnCtrl->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CreateSLBL_DLG::OnMaskBrowseBtn ), NULL, this );
}



bool CreateSLBL_DLG::TransferDataToWindow(){
    m_OptMinDiffCtrl->SetValue(m_Params.mindiff);
    m_OptMaxIterCtrl->SetValue(m_Params.maxiter);
    return true;
}



bool CreateSLBL_DLG::TransferDataFromWindow(){
    return true;
}



void CreateSLBL_DLG::OnInputBrowseBtn( wxCommandEvent& event ) {
    vrDrivers myDriver;
    wxString myWildCard = myDriver.GetWildcardsRaster();
    wxFileDialog myDlg(this, _("Open Input DEM"), wxEmptyString, wxEmptyString, myWildCard);
    if (myDlg.ShowModal() != wxID_OK) {
        return;
    }
    
    m_InputListCtrl->SetValue(myDlg.GetPath());
}


void CreateSLBL_DLG::OnUseMaskCheck( wxCommandEvent& event ) {
    m_MaskListCtrl->Enable(event.IsChecked());
    m_MaskBrowseBtnCtrl->Enable(event.IsChecked());
}


void CreateSLBL_DLG::OnMaskBrowseBtn( wxCommandEvent& event ) {
    vrDrivers myDriver;
    wxString myWildCard = myDriver.GetWildcardsVector();
    wxFileDialog myDlg(this, _("Open Mask Vector"), wxEmptyString, wxEmptyString, myWildCard);
    if (myDlg.ShowModal() != wxID_OK) {
        return;
    }
    
    m_MaskListCtrl->SetValue(myDlg.GetPath());
}



void CreateSLBL_DLG::_CreateControls(){
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Input") ), wxHORIZONTAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Input DEM:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	sbSizer1->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_InputListCtrl = new wxComboBox( this, wxID_ANY,wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_InputListCtrl->SetMinSize( wxSize( 350,-1 ) );
	
	sbSizer1->Add( m_InputListCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_InputBrowseBtnCtrl = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_InputBrowseBtnCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer1->Add( sbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Mask") ), wxVERTICAL );
	
	m_MaskUseCtrl = new wxCheckBox( this, wxID_ANY, _("Use Mask"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MaskUseCtrl->SetValue(true);
	sbSizer2->Add( m_MaskUseCtrl, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Input Mask:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer2->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MaskListCtrl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer2->Add( m_MaskListCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_MaskBrowseBtnCtrl = new wxButton( this, wxID_ANY, _("Browse"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_MaskBrowseBtnCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	sbSizer2->Add( bSizer2, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer2, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Options") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( this, wxID_ANY, _("Maximum iterations:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_OptMaxIterCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1000000, 0 );
	fgSizer1->Add( m_OptMaxIterCtrl, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Minimum difference:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_OptMinDiffCtrl = new wxSpinCtrlDouble( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 1000000, 0, 0.1);
	fgSizer1->Add( m_OptMinDiffCtrl, 0, wxALL|wxEXPAND, 5 );
	
	
	sbSizer3->Add( fgSizer1, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( sbSizer3, 0, wxEXPAND|wxALL, 5 );
	
	wxStaticBoxSizer* sbSizer4;
	sbSizer4 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Results") ), wxHORIZONTAL );
	
	wxStaticText* m_staticText5;
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Result DEM:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	sbSizer4->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
    vrDrivers myDriver;
	wxFilePickerCtrl* m_filePicker2;
	m_filePicker2 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, _("Save Result"), myDriver.GetWildcardsRaster(), wxDefaultPosition, wxDefaultSize, wxFLP_SAVE | wxFLP_OVERWRITE_PROMPT | wxFLP_USE_TEXTCTRL );
	sbSizer4->Add( m_filePicker2, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	
	bSizer1->Add( sbSizer4, 0, wxALL|wxEXPAND, 5 );
	
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizer1->Add( m_sdbSizer1, 1, wxEXPAND|wxALL, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	bSizer1->Fit( this );
	
	this->Centre( wxBOTH );
}
