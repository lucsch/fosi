/***************************************************************************
 app.cpp
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/


#include "app.h"
#include "frame.h"
#include "lscrashreport.h"

IMPLEMENT_APP(App);

App::App(){
    //wxHandleFatalExceptions();
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
    
    if (myCrashReport.SendReportWeb(_T("http://www.terranum.ch/crashreport/upload_file.php"), myProxyInfo)==false){
        wxString myDocPath = wxStandardPaths::Get().GetDocumentsDir();
        if(myCrashReport.SaveReportFile(myDocPath)==false){
            wxLogError(_("Unable to save the crashreport!"));
            return;
        }
        wxLogWarning(_("Connection problem! crashreport wasn't sent. crashreport was saved into '%s'\nplease send it manually to lucien.schreiber@gmail.com"), myDocPath);
    }
}



