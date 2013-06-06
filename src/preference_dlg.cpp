/***************************************************************************
 preferencedlg.cpp
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
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


PreferenceDlg::PreferenceDlg(wxWindow * parent, ArrayPlugin * plugins) : 
wxPropertySheetDialog(parent, wxID_ANY, _("Preferences")) {
	m_Plugins = plugins;
	wxASSERT(m_Plugins);
	
	
	
	// generic panel
	wxBookCtrlBase * notebook = GetBookCtrl();
	wxASSERT(notebook);
	PreferencePanelMain * myGeneralPanel = new PreferencePanelMain(notebook);
	notebook->AddPage(myGeneralPanel, _("General"), true);
    
    PreferencePanelInternet * myInternetPanel = new PreferencePanelInternet(notebook);
    notebook->AddPage(myInternetPanel, _("Updates"));
	
	
	// plugins panel
	wxASSERT(m_Plugins);
	for (unsigned int i = 0; i<m_Plugins->GetCount(); i++) {
		Plugin * myPlugin = m_Plugins->Item(i);
		wxASSERT(myPlugin);
		PreferencePanel * myPluginPanel = myPlugin->GetPreferencePanel(notebook);
		if (myPluginPanel == NULL) {
			wxLogError(_("Error getting preference panel for %s"),
					   myPlugin->GetPluginName());
			continue;
		}
		notebook->AddPage(myPluginPanel, myPlugin->GetPluginName(), false);
	}
	/*wxStaticText* m_staticText6;
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Some settings need a restart to take effect. "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	GetInnerSizer()->Add( m_staticText6, 0, wxLEFT | wxRIGHT, 5 );*/
	CreateButtons();
	LayoutDialog();
}



PreferenceDlg::~PreferenceDlg() {
}

