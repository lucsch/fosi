/***************************************************************************
 coltopgisapp.cpp
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/


#include "app.h"
#include "frame.h"

IMPLEMENT_APP(App);

App::App(){
    wxHandleFatalExceptions();
}


bool App::OnInit(){
	if ( !wxApp::OnInit() ){
        return false;
    }
	wxInitAllImageHandlers();

#ifdef __WXDEBUG__
    // debugging string for OSX
    // this is needed for viewing string content with Xcode !!
    wxString myTest = _T("Test debugging");
    myTest.ToUTF8().data();
#endif
    
    
    m_Frame = new Frame(g_ProgName);
    m_Frame->CenterOnScreen(wxBOTH);
	m_Frame->SetSize(50, 50, 800, 600);
	m_Frame->Show(true);
    return true;
}


void App::MacOpenFile(const wxString &fileName){
	wxFileName myFile (fileName);
	if (myFile.GetExt() != _T("c2p")) {
		wxLogError("Unknown file type");
		return;
	}
	if (m_Frame) {
		m_Frame->ProjectOpen(myFile);
	}
	wxLogMessage(_("Project found : %s"), myFile.GetFullName());
}


void App::OnFatalException(){
    lsCrashReport myCrashReport (g_ProgName);
    if(myCrashReport.PrepareReport(wxDebugReport::Context_Exception)==false){
        return;
    }
    
    wxConfigBase * myConfig =  wxConfigBase::Get(false);
    wxASSERT(myConfig);
    myConfig->SetPath("INTERNET");
    wxString myProxyInfo = myConfig->Read("PROXY_INFO", wxEmptyString);
    myConfig->SetPath("..");
    
    if (myCrashReport.SendReportWeb(_T("http://www.crealp.ch/crashreport/upload_file.php"), myProxyInfo)==false){
        wxString myDocPath = wxStandardPaths::Get().GetDocumentsDir();
        if(myCrashReport.SaveReportFile(myDocPath)==false){
            wxLogError(_("Unable to save the crashreport!"));
            return;
        }
        wxLogWarning(_("Connection problem! crashreport wasn't sent. crashreport was saved into '%s'\nplease send it manually to lucien.schreiber@crealp.vs.ch"), myDocPath);
    }
}



