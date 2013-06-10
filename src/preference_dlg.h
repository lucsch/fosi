/***************************************************************************
 preferencedlg.h
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/

#ifndef _PREFERENCEDLG_H_
#define _PREFERENCEDLG_H_

// wxWidgets include
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/propdlg.h>

class PreferenceDlg : public wxPropertySheetDialog {
public:
    PreferenceDlg(wxWindow * parent);
    virtual ~PreferenceDlg();
    DECLARE_EVENT_TABLE();
    
private:
    void OnSave(wxCommandEvent & event);
    
};
#endif
