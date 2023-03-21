/***************************************************************************
 app.h
 -------------------
 copyright            : (C) 2013 Lucien Schreiber 
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _APP_H_
#define _APP_H_

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
    // virtual void OnFatalException();
};
DECLARE_APP(App)

#endif
