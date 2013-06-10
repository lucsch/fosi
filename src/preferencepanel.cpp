/***************************************************************************
 preferencepanel.cpp
 -------------------
 copyright            : (C) 2013 Lucien Schreiber 
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/


#include "preferencepanel.h"
#include "preference_dlg.h"


PreferencePanel::PreferencePanel(wxWindow * parent) : 
wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL) {
	m_Parent = parent;
	
}

PreferencePanel::~PreferencePanel() {
}


bool PreferencePanel::SaveSettings() {
	return false;
}




PreferencePanelMain::PreferencePanelMain(wxWindow * parent) : 
PreferencePanel(parent) {
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText * m_staticText2 = new wxStaticText( this, wxID_ANY, _("Units:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString m_choice1Choices[] = { _("Meters"), _("Degree Minute Second") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_UnitChoiceCtrl = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 );
	fgSizer1->Add( m_UnitChoiceCtrl, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticText* m_staticText3 = new wxStaticText( this, wxID_ANY, _("Background color:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText3, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_BackgroundColourCtrl = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_BackgroundColourCtrl, 0, wxALL, 5 );
	
	
	wxStaticText* m_staticText4 = new wxStaticText( this, wxID_ANY, _("Selection color:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	fgSizer1->Add( m_staticText4, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SelectionColourCtrl = new wxColourPickerCtrl(this, wxID_ANY, *wxRED, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer1->Add( m_SelectionColourCtrl, 0, wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_RelativePathCtrl = new wxCheckBox( this, wxID_ANY, _("Save relative path "), wxDefaultPosition, wxDefaultSize, 0 );
	m_RelativePathCtrl->SetValue(true); 
	fgSizer1->Add( m_RelativePathCtrl, 0, wxALL, 5 );
	
	
	fgSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_BuildOverviewCtrl = new wxCheckBox( this, wxID_ANY, _("Build overviews"), wxDefaultPosition, wxDefaultSize, 0 );
	m_BuildOverviewCtrl->SetValue(true); 
	fgSizer1->Add( m_BuildOverviewCtrl, 0, wxALL, 5 );
	
	
	bSizer5->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer5 );
	
	_LoadSettings();
	
}

PreferencePanelMain::~PreferencePanelMain() {
}


void PreferencePanelMain::_LoadSettings() {
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	wxASSERT(myConfig);
	myConfig->SetPath("GENERAL");
	
	long myUnitChoice = myConfig->ReadLong("PREF_UNIT_TYPE", 0);
	m_UnitChoiceCtrl->SetSelection(myUnitChoice);
	
	wxColour myColor = myConfig->ReadObject("PREF_BACKGROUND_COLOUR", *wxWHITE);
	m_BackgroundColourCtrl->SetColour(myColor);
	
	wxColour mySelColour = myConfig->ReadObject("SELECTION_COLOUR", *wxRED);
	m_SelectionColourCtrl->SetColour(mySelColour);
	
	bool myRelativePath = myConfig->ReadBool("SAVE_RELATIVE_PATH", true);
	m_RelativePathCtrl->SetValue(myRelativePath);
	
	bool myBuildOverview = myConfig->ReadBool("BUILD_OVERVIEW", true);
	m_BuildOverviewCtrl->SetValue(myBuildOverview);
	
	myConfig->SetPath("..");
	
}


bool PreferencePanelMain::SaveSettings() {
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	if (myConfig == NULL) {
		return false;
	}
	
	myConfig->SetPath("GENERAL");
	myConfig->Write("PREF_BACKGROUND_COLOUR", m_BackgroundColourCtrl->GetColour());
	myConfig->Write("SELECTION_COLOUR", m_SelectionColourCtrl->GetColour());
	myConfig->Write("PREF_UNIT_TYPE", m_UnitChoiceCtrl->GetSelection());
	myConfig->Write("SAVE_RELATIVE_PATH", m_RelativePathCtrl->GetValue());
	myConfig->Write("BUILD_OVERVIEW", m_BuildOverviewCtrl->GetValue());
	
	myConfig->SetPath("..");
	return true;
}














PreferencePanelInternet::PreferencePanelInternet(wxWindow * parent) : 
PreferencePanel(parent) {
    wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );
	
	m_CheckAtStartup = new wxCheckBox( this, wxID_ANY, _("Check for updates at startup"), wxDefaultPosition, wxDefaultSize, 0 );
	m_CheckAtStartup->SetValue(true); 
	bSizer6->Add( m_CheckAtStartup, 0, wxALL, 5 );
	
	wxStaticText* m_staticText5;
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Proxy informations:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer6->Add( m_staticText5, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_ProxyInfo = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_ProxyInfo, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	wxStaticText* m_staticText6;
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Please use the following form: myproxy.com:8080"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer6->Add( m_staticText6, 0, wxALL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );
	
	bSizer6->Add( bSizer7, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer6 );
	
	_LoadSettings();
	
}

PreferencePanelInternet::~PreferencePanelInternet() {
}


void PreferencePanelInternet::_LoadSettings() {
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	wxASSERT(myConfig);
	myConfig->SetPath("INTERNET");
	
	bool myCheckAtStartup = myConfig->ReadBool("CHECK_AT_STARTUP", true);
    m_CheckAtStartup->SetValue(myCheckAtStartup);
    
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    m_ProxyInfo->SetValue(myProxyInfo);    
	myConfig->SetPath("..");
	
}


bool PreferencePanelInternet::SaveSettings() {
	wxConfigBase * myConfig = wxConfigBase::Get(false);
	if (myConfig == NULL) {
		return false;
	}
    
	myConfig->SetPath("INTERNET");
    myConfig->Write("CHECK_AT_STARTUP", m_CheckAtStartup->GetValue());
    myConfig->Write("PROXY_INFO", m_ProxyInfo->GetValue());	
	myConfig->SetPath("..");
	return true;
}


