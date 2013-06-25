/***************************************************************************
 frame.h
 -------------------
 copyright            : (C) 2013 Lucien Schreiber
 email                : lucien.schreiber at gmail dot com
 ***************************************************************************/
#ifndef _FRAME_H_
#define _FRAME_H_

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <wx/infobar.h>
#include <wx/dnd.h>

#include "vroomgis.h"
#include "update.h"

class TOC;
class vrLayerManager;
class vrViewerLayerManager;
class vrViewerDisplay;
class ProjectManager;

// Identifier
const int FRAME_WINDOW = wxID_HIGHEST + 1;
const int MENU_FRAME_SELECT = wxID_HIGHEST + 2;
const int MENU_FRAME_PAN = wxID_HIGHEST + 3;
const int MENU_WINDOW_LOG = wxID_HIGHEST + 4;
const int MENU_WINDOW_DATA_MANAGER = wxID_HIGHEST + 5;
const int WINDOW_DATA_MANAGER = wxID_HIGHEST + 6;
const int MENU_DATA_ADD = wxID_HIGHEST + 7;
const int MENU_DATA_REMOVE = wxID_HIGHEST + 8;
const int MENU_DATA_MEMORY_ADD = wxID_HIGHEST + 9;
const int MENU_EDITION_START = wxID_HIGHEST + 10;
const int MENU_EDITION_STOP = wxID_HIGHEST + 11;
const int MENU_CHECK_UPDATE = wxID_HIGHEST + 23;
const int MENU_WEBSITE = wxID_HIGHEST + 24;
const int MENU_FRAME_CLEAR_SELECTION = wxID_HIGHEST + 26;

const wxString g_ProgName = "Fosi";

// DND support
class Frame;
class ctDropFiles : public wxFileDropTarget {
public:
	ctDropFiles(Frame * parent);
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString & filenames);
private:
	Frame * m_Frame;
};


class Frame : public wxFrame {
public:
    Frame(const wxString & title);
    virtual ~Frame();
	
	bool ProjectOpen(const wxFileName & file);
	bool AddLayers (const wxArrayString & names);
    bool AddLayer(const wxFileName & file, long position = -1, vrRender * render = NULL,
				  vrLabel * label = NULL, bool visible = true, bool readwrite = false);
	inline vrViewerLayerManager * GetViewerLayerManager() const;
	inline vrLayerManager * GetLayerManager() const;
    
private:
	// vroomgis engine members
    vrViewerTOCTree * m_vrTOC;
    vrLayerManager * m_vrLayerManager;
    vrViewerLayerManager * m_vrViewerLayerManager;
    vrViewerDisplay * m_vrDisplay;
	
	wxLogWindow * m_LogWnd;
	wxMenu * m_RecentMenu;
	wxKeyboardState m_KeyBoardState;
    WebUpdateInformationBar * m_InfoBar;
	
	void _CreateControls();
    void _CreateMenus();
    void _CreateToolbar();
	void _CreateAccelerators();
    wxString _GetMemoryLayerNameFromUser(const wxString & name);
    
	void _SetSelectedFeature();
    bool _RemoveLayer(long position);
	void _PreferenceChanged(bool refresh = true);
	void OnDisplaySecurityMessage(wxThreadEvent & event);
    
    void OnAbout(wxCommandEvent & event);
    void OnCheckUpdates(wxCommandEvent & event);
	void OnPreferences(wxCommandEvent & event);
    void OnUserManual(wxCommandEvent & event);
    void OnWebSite (wxCommandEvent & event);
    void OnQuit(wxCommandEvent & event);
	void OnLogWindow(wxCommandEvent & event);
	void OnLayerAdd(wxCommandEvent & event);
    void OnLayerRemove(wxCommandEvent & event);
    void OnLayerMemoryAdd(wxCommandEvent & event);
    
    void OnEditionStart (wxCommandEvent & event);
    void OnEditionStop (wxCommandEvent & event);

	void OnUpdateIdle(wxIdleEvent & event);
	void OnClose(wxCloseEvent & event);
	void OnKeyDown(wxKeyEvent & event);
    void OnKeyUp(wxKeyEvent & event);
	
	void OnToolZoomToFit(wxCommandEvent & event);
    void OnToolSelect(wxCommandEvent & event);
    void OnToolSelectAction(wxCommandEvent & event);
    void OnToolClearSelection(wxCommandEvent & event);
    void OnToolZoom(wxCommandEvent & event);
    void OnToolZoomAction(wxCommandEvent & event);
    void OnToolPan(wxCommandEvent & event);
    void OnToolPanAction(wxCommandEvent & event);
    
    // Update UI functions
    void OnUpdateUIToolClearSelection(wxUpdateUIEvent & event);
    void OnUpdateUIRemoveLayer (wxUpdateUIEvent & event);
	
	DECLARE_EVENT_TABLE();
};



inline  vrLayerManager * Frame::GetLayerManager() const {
	return m_vrLayerManager;
}

inline  vrViewerLayerManager * Frame::GetViewerLayerManager() const {
	return m_vrViewerLayerManager;
}


#endif
