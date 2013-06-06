/***************************************************************************
 preferencedlg.h
 -------------------
 copyright            : (C) 2010 CREALP Lucien Schreiber 
 email                : lucien.schreiber at crealp dot vs dot ch
 ***************************************************************************/

#ifndef _PREFERENCEDLG_H_
#define _PREFERENCEDLG_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/propdlg.h>
#include "plugin.h"


class PreferenceDlg : public wxPropertySheetDialog {
  private:
    ArrayPlugin * m_Plugins;

    void OnSave(wxCommandEvent & event);

  public:
    PreferenceDlg(wxWindow * parent, ArrayPlugin * plugins);
    virtual ~PreferenceDlg();
	DECLARE_EVENT_TABLE();

};
#endif
