/***************************************************************************
 update.h
 Deals with update process by cheking internet for new versions
 -------------------
 copyright            : (C) 2009 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#ifndef _WEBUPDATE_H_
#define _WEBUPDATE_H_

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

// Include wxWidgets' headers
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/infobar.h>
#include <curl/curl.h>// for web acces
#include <wx/sstream.h>

class Frame;

const long WEBUPDATE_CONNECTION_TIMEOUT = 2000;
const wxString WEBUPDATE_SERVER_DOWNLOAD = "http://www.crealp.ch/index.php?option=com_content&task=view&id=382&Itemid=387";
const wxString WEBUPDATE_SERVER_UPDATE = "http://www.crealp.ch/down/coltopgis/latestversion.txt";


const int INFOBAR_DOWNLOAD_BUTTON = wxID_HIGHEST + 22;
class WebUpdateInformationBar : public wxInfoBar {
private:
    Frame * m_Parent;
    
    void _ClearBar();
    
    
    void OnCommandDownload(wxCommandEvent & event);
    
    void OnInternetFailed(wxThreadEvent & event);
    void OnNewVersion(wxThreadEvent & event);
    void OnNoNewVersion(wxThreadEvent & event);
    
    DECLARE_EVENT_TABLE();
public:
    WebUpdateInformationBar(Frame * parent);
    virtual ~WebUpdateInformationBar();
    
};



// CALLBACK FOR CURL
size_t wxcurl_str_write(void* ptr, size_t size, size_t nmemb, void* stream);

const int THREAD_MESSAGE_NEW_VERSION = wxID_HIGHEST + 19;
const int THREAD_MESSAGE_NONEW_VERSION = wxID_HIGHEST + 20;
const int THREAD_MESSAGE_NO_INTERNET = wxID_HIGHEST + 21;

class WebUpdateThread : public wxThread {
private:
    CURL * m_CurlHandle;
    wxWindow * m_Parent;
    wxStringOutputStream  m_Buffer;
    
    bool m_msgNewVersion;
    bool m_msgNoNewVersion;
    bool m_msgNoInternet;
    
    long m_ActualVersion;
    
protected:
    virtual ExitCode Entry();
    
public:
    WebUpdateThread(wxWindow * parent, const wxString & proxy = wxEmptyString);
    virtual ~WebUpdateThread();
    
    bool CheckNewVersion (long actualversion, bool msgNewVersion = true,
                          bool msgNoNewVersion = true, bool msgNoInternet = true);
};























#endif
