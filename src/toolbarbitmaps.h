#ifndef _FOSI_TOOLBARBITMAPS_H
#define _FOSI_TOOLBARBITMAPS_H

#include "wx/wxprec.h"

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

class Bitmaps {
public:
    static wxString SvgText[];
    static wxString SvgLogo;
    enum ID {
        SELECT = 0,
        ZOOM_FIT,
        ZOOM,
        PAN,
        EDITION_START,
        EDITION_DRAW,
        EDITION_MODIFY
    };

    static wxBitmap GetBitmap(Bitmaps::ID id, const wxString &colour = "black", const wxSize &size = wxSize(16, 16));

    static wxBitmap GetLogo(const wxSize &size = wxSize(32, 32));
};

#endif
