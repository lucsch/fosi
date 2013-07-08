/***************************************************************************
 profile_dlg
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "profile_dlg.h"
#include "vroomgis.h"
#include "vrlayervector.h"





Profile_DLG::Profile_DLG( wxWindow* parent, vrViewerLayerManager * viewermanager, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ){
    _CreateControls();
    
    wxASSERT(viewermanager);
    for (unsigned int i = 0; i< viewermanager->GetCount(); i++) {
        vrLayer * myLayer =  viewermanager->GetRenderer(i)->GetLayer();
        wxASSERT(myLayer);
        
        if (myLayer->GetType() > vrDRIVER_UNKNOWN  && myLayer->GetType() <= vrDRIVER_VECTOR_MEMORY) {
            // add polygon mask
            vrLayerVector * myLayerVector = static_cast<vrLayerVector*>(myLayer);
            if (wkbFlatten(myLayerVector->GetGeometryType()) == wkbLineString) {
                m_InputLineCtrl->Append(myLayerVector->GetDisplayName().GetFullPath());
            }
        }
        else if (myLayer->GetType() > vrDRIVER_VECTOR_MEMORY && myLayer->GetType() <= vrDRIVER_RASTER_SGRD7){
            m_InputDEMCtrl->Append(myLayer->GetDisplayName().GetFullPath());
        }
    }
    m_TextFilePickerCtrl->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITextFilePickerCtrl ), NULL, this );
    m_BtnOkCtrl->Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITBtnOk ), NULL, this );

}



Profile_DLG::~Profile_DLG(){
    m_TextFilePickerCtrl->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITextFilePickerCtrl ), NULL, this );
    m_BtnOkCtrl->Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( Profile_DLG::OnUpdateUITBtnOk ), NULL, this );


}



void Profile_DLG::OnUpdateUITextFilePickerCtrl( wxUpdateUIEvent& event ){
    event.Enable(m_TextFileOptionCtrl->GetValue());
}



void Profile_DLG::OnUpdateUITBtnOk( wxUpdateUIEvent& event ){
    if (m_InputLineCtrl->GetStringSelection() == wxEmptyString) {
        event.Enable(false);
        return;
    }
    
    if (m_ExportNotebookCtrl->GetSelection() == 0 &&
        m_TextFileOptionCtrl->GetValue() == true &&
        m_TextFilePickerCtrl->GetFileName().IsOk() == false) {
        event.Enable(false);
        return;
    }
    
    if (m_ExportNotebookCtrl->GetSelection() == 1 &&
        m_VectorFilePickerCtrl->GetFileName().IsOk() == false) {
        event.Enable(false);
        return;
    }
    
    event.Enable(true);
}



bool Profile_DLG::TransferDataFromWindow(){
    m_Params.m_FileInputDEM = m_InputDEMCtrl->GetStringSelection();
    m_Params.m_FileInputVector = m_InputLineCtrl->GetStringSelection();
    if (m_Params.m_FileInputVector.GetExt() == wxEmptyString) {
        m_Params.m_FileInputVector.SetExt (_T("memory"));
    }
    m_Params.m_FileOutputText = m_TextFilePickerCtrl->GetFileName();
    m_Params.m_FileOutputVector = m_VectorFilePickerCtrl->GetFileName();
    
    m_Params.m_AllValues = m_FeaturesAllCtrl->GetValue();
    m_Params.m_ExportTypeFormat = ProfileParams::PROFILE_EXPORT_TEXT;
    if (m_ExportNotebookCtrl->GetSelection() == 1) {
        m_Params.m_ExportTypeFormat = ProfileParams::PROFILE_EXPORT_VECTOR;
    }
    
    m_Params.m_ExportTypeTextFormat = ProfileParams::PROFILE_TEXT_CLIPBOARD;
    if (m_TextFileOptionCtrl->GetValue() == true) {
        m_Params.m_ExportTypeTextFormat = ProfileParams::PROFILE_TEXT_FILE;
    }
    
    m_Params.m_ExportTypeText = ProfileParams::PROFILE_TEXT_DISTANCE_HEIGHT;
    if (m_XYZCtrl->GetValue() == true) {
        m_Params.m_ExportTypeText = ProfileParams::PROFILE_TEXT_XYZ;
    }
    
    m_Params.m_ExportTypeVector = ProfileParams::PROFILE_VECTOR_POINT;
    if (m_3dLineCtrl->GetValue() == true) {
        m_Params.m_ExportTypeVector = ProfileParams::PROFILE_VECTOR_LINE;
    }
    
    return true;
}



void Profile_DLG::_CreateControls(){
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Source") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText* m_staticText14;
	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Input DEM:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText14->Wrap( -1 );
	fgSizer2->Add( m_staticText14, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_InputDEMCtrl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 350,-1 ), 0, NULL, 0 );
	fgSizer2->Add( m_InputDEMCtrl, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Input Line:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_InputLineCtrl = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0, NULL, 0 );
	m_InputLineCtrl->SetMinSize( wxSize( 250,-1 ) );
	
	fgSizer2->Add( m_InputLineCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	
	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );
	
	
	sbSizer1->Add( bSizer9, 0, wxEXPAND, 5 );
	
	m_FeaturesAllCtrl = new wxRadioButton( this, wxID_ANY, _("All features"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_FeaturesAllCtrl->SetValue( true );
	sbSizer1->Add( m_FeaturesAllCtrl, 0, wxALL, 5 );
	
	m_FeaturesSelectedCtrl = new wxRadioButton( this, wxID_ANY, _("Only selected features"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer1->Add( m_FeaturesSelectedCtrl, 0, wxALL, 5 );
	
	
	bSizer8->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	
	m_ExportNotebookCtrl = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_PanelText = new wxPanel( m_ExportNotebookCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer12;
	sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( m_PanelText, wxID_ANY, _("Options") ), wxVERTICAL );
	
	m_DistanceHeightCtrl = new wxRadioButton( m_PanelText, wxID_ANY, _("Distance and height"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_DistanceHeightCtrl->SetValue( true );
	sbSizer12->Add( m_DistanceHeightCtrl, 0, wxALL, 5 );
	
	m_XYZCtrl = new wxRadioButton( m_PanelText, wxID_ANY, _("XYZ"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer12->Add( m_XYZCtrl, 0, wxALL, 5 );
	
	
	bSizer10->Add( sbSizer12, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer13;
	sbSizer13 = new wxStaticBoxSizer( new wxStaticBox( m_PanelText, wxID_ANY, _("Output") ), wxVERTICAL );
	
	m_ClipBoardCtrl = new wxRadioButton( m_PanelText, wxID_ANY, _("Clipboard"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_ClipBoardCtrl->SetValue( true );
	sbSizer13->Add( m_ClipBoardCtrl, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );
	
	m_TextFileOptionCtrl = new wxRadioButton( m_PanelText, wxID_ANY, _("File"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( m_TextFileOptionCtrl, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_TextFilePickerCtrl = new wxFilePickerCtrl( m_PanelText, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.*"), wxDefaultPosition, wxSize( -1,-1 ), wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	bSizer11->Add( m_TextFilePickerCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
	sbSizer13->Add( bSizer11, 0, wxEXPAND, 5 );
	
	
	bSizer10->Add( sbSizer13, 1, wxEXPAND|wxALL, 5 );
	
	
	m_PanelText->SetSizer( bSizer10 );
	m_PanelText->Layout();
	bSizer10->Fit( m_PanelText );
	m_ExportNotebookCtrl->AddPage( m_PanelText, _("Text Export"), false );
	m_PanelVector = new wxPanel( m_ExportNotebookCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticBoxSizer* sbSizer121;
	sbSizer121 = new wxStaticBoxSizer( new wxStaticBox( m_PanelVector, wxID_ANY, _("Options") ), wxVERTICAL );
	
	m_3DPointCtrl = new wxRadioButton( m_PanelVector, wxID_ANY, _("Points"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	m_3DPointCtrl->SetValue( true );
	sbSizer121->Add( m_3DPointCtrl, 0, wxALL, 5 );
	
	m_3dLineCtrl = new wxRadioButton( m_PanelVector, wxID_ANY, _("3D Line"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer121->Add( m_3dLineCtrl, 0, wxALL, 5 );
	
	
	bSizer13->Add( sbSizer121, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticText* m_staticText111;
	m_staticText111 = new wxStaticText( m_PanelVector, wxID_ANY, _("File:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText111->Wrap( -1 );
	bSizer111->Add( m_staticText111, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_VectorFilePickerCtrl = new wxFilePickerCtrl( m_PanelVector, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.*"), wxDefaultPosition, wxSize( -1,-1 ), wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE|wxFLP_USE_TEXTCTRL );
	bSizer111->Add( m_VectorFilePickerCtrl, 1, wxALL|wxEXPAND, 5 );
	
	
	bSizer13->Add( bSizer111, 0, wxEXPAND, 5 );
	
	
	bSizer13->Add( 0, 0, 1, wxEXPAND, 5 );
	
	
	m_PanelVector->SetSizer( bSizer13 );
	m_PanelVector->Layout();
	bSizer13->Fit( m_PanelVector );
	m_ExportNotebookCtrl->AddPage( m_PanelVector, _("Vector Export"), false );
	
	bSizer8->Add( m_ExportNotebookCtrl, 1, wxEXPAND | wxALL, 5 );
	
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1Cancel;
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_BtnOkCtrl = new wxButton( this, wxID_OK );
	m_sdbSizer1->AddButton( m_BtnOkCtrl );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	
	bSizer8->Add( m_sdbSizer1, 0, wxALL|wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer8 );
	this->Layout();
	bSizer8->Fit( this );
	
	this->Centre( wxBOTH );
}


