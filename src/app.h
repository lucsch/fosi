/***************************************************************************
 coltopgisapp.h
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#ifndef _COLTOPGISAPP_H
#define _COLTOPGISAPP_H

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/config.h>
#include <wx/stdpaths.h>

class Frame;
class App : public wxApp {
	Frame * m_Frame;
  public:
    App();
	virtual bool OnInit();
	virtual void MacOpenFile(const wxString &fileName);
    virtual void OnFatalException();
};
DECLARE_APP(App)

#endif
