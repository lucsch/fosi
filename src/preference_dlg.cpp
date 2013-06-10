/***************************************************************************
 preferencedlg.cpp
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#include "preference_dlg.h"
#include <wx/bookctrl.h>
#include "preferencepanel.h"

BEGIN_EVENT_TABLE(PreferenceDlg, wxDialog)
EVT_BUTTON(wxID_OK, PreferenceDlg::OnSave)
END_EVENT_TABLE()


void PreferenceDlg::OnSave(wxCommandEvent & event) {
	wxBookCtrlBase * notebook = GetBookCtrl();
	wxASSERT(notebook);
	for (unsigned int i = 0; i<notebook->GetPageCount(); i++) {
		PreferencePanel * myPanel = (PreferencePanel*) notebook->GetPage(i);
		wxASSERT(myPanel);
		myPanel->SaveSettings();
	}
	event.Skip();
}


PreferenceDlg::PreferenceDlg(wxWindow * parent) : wxPropertySheetDialog(parent, wxID_ANY, _("Preferences")) {
	// generic panel
	wxBookCtrlBase * notebook = GetBookCtrl();
	wxASSERT(notebook);
	PreferencePanelMain * myGeneralPanel = new PreferencePanelMain(notebook);
	notebook->AddPage(myGeneralPanel, _("General"), true);
    
    PreferencePanelInternet * myInternetPanel = new PreferencePanelInternet(notebook);
    notebook->AddPage(myInternetPanel, _("Updates"));
	
    /*wxStaticText* m_staticText6;
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Some settings need a restart to take effect. "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	GetInnerSizer()->Add( m_staticText6, 0, wxLEFT | wxRIGHT, 5 );*/
	CreateButtons();
	LayoutDialog();
}



PreferenceDlg::~PreferenceDlg() {
}

