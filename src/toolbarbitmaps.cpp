#include "toolbarbitmaps.h"

wxString
        Bitmaps::
        SvgText
[] =
        {
                // SELECT
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-pointer" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M7.904 17.563a1.2 1.2 0 0 0 2.228 .308l2.09 -3.093l4.907 4.907a1.067 1.067 0 0 0 1.509 0l1.047 -1.047a1.067 1.067 0 0 0 0 -1.509l-4.907 -4.907l3.113 -2.09a1.2 1.2 0 0 0 -.309 -2.228l-13.582 -3.904l3.904 13.563z" /> </svg>)",
                // ZOOM_FIT
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-arrows-maximize" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M16 4l4 0l0 4" /> <path d="M14 10l6 -6" /> <path d="M8 20l-4 0l0 -4" /> <path d="M4 20l6 -6" /> <path d="M16 20l4 0l0 -4" /> <path d="M14 14l6 6" /> <path d="M8 4l-4 0l0 4" /> <path d="M4 4l6 6" /> </svg>)",
                // ZOOM
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-zoom-in-area" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M15 13v4" /> <path d="M13 15h4" /> <path d="M15 15m-5 0a5 5 0 1 0 10 0a5 5 0 1 0 -10 0" /> <path d="M22 22l-3 -3" /> <path d="M6 18h-1a2 2 0 0 1 -2 -2v-1" /> <path d="M3 11v-1" /> <path d="M3 6v-1a2 2 0 0 1 2 -2h1" /> <path d="M10 3h1" /> <path d="M15 3h1a2 2 0 0 1 2 2v1" /> </svg>)",
                // PAN
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-hand-stop" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M8 13v-7.5a1.5 1.5 0 0 1 3 0v6.5" /> <path d="M11 5.5v-2a1.5 1.5 0 1 1 3 0v8.5" /> <path d="M14 5.5a1.5 1.5 0 0 1 3 0v6.5" /> <path d="M17 7.5a1.5 1.5 0 0 1 3 0v8.5a6 6 0 0 1 -6 6h-2h.208a6 6 0 0 1 -5.012 -2.7a69.74 69.74 0 0 1 -.196 -.3c-.312 -.479 -1.407 -2.388 -3.286 -5.728a1.5 1.5 0 0 1 .536 -2.022a1.867 1.867 0 0 1 2.28 .28l1.47 1.47" /> </svg>)",
                // EDITION_START
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-edit" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M7 7h-1a2 2 0 0 0 -2 2v9a2 2 0 0 0 2 2h9a2 2 0 0 0 2 -2v-1" /> <path d="M20.385 6.585a2.1 2.1 0 0 0 -2.97 -2.97l-8.415 8.385v3h3l8.385 -8.415z" /> <path d="M16 5l3 3" /> </svg>)",
                // EDITION_DRAW
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-pencil" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M4 20h4l10.5 -10.5a1.5 1.5 0 0 0 -4 -4l-10.5 10.5v4" /> <path d="M13.5 6.5l4 4" /> </svg>)",
                // EDITION_MODIFY
                R"(<svg xmlns="http://www.w3.org/2000/svg" class="icon icon-tabler icon-tabler-photo-edit" width="24" height="24" viewBox="0 0 24 24" stroke-width="2" stroke="%s" fill="none" stroke-linecap="round" stroke-linejoin="round"> <path stroke="none" d="M0 0h24v24H0z" fill="none"/> <path d="M15 8h.01" /> <path d="M11 20h-4a3 3 0 0 1 -3 -3v-10a3 3 0 0 1 3 -3h10a3 3 0 0 1 3 3v4" /> <path d="M4 15l4 -4c.928 -.893 2.072 -.893 3 0l3 3" /> <path d="M14 14l1 -1c.31 -.298 .644 -.497 .987 -.596" /> <path d="M18.42 15.61a2.1 2.1 0 0 1 2.97 2.97l-3.39 3.42h-3v-3l3.42 -3.39z" /> </svg>)"
        };

wxString Bitmaps::SvgLogo = R"lit(<svg version="1.1" viewBox="0 0 1024 1024" xmlns="http://www.w3.org/2000/svg"> <path d="m269.75 31.954h484.5c131.74 0 237.8 106.06 237.8 237.8v484.5c0 131.74-106.06 237.8-237.8 237.8h-484.5c-131.74 0-237.8-106.06-237.8-237.8v-484.5c0-131.74 106.06-237.8 237.8-237.8z" color="#000000" fill="#84c254" stroke-width="49.908" style="-inkscape-stroke:none"/> <g transform="translate(0,-14)" stroke-linecap="square" stroke-linejoin="round" stroke-width="30"> <path d="m192.11 772.18 139.37 0.7408 93.146 103.07-158.67 0.55047z" color="#000000" stroke="#000" stroke-width="30"/> <path d="m512.85 772.18-88.224 103.81 346.98-1.1435 62.005-102.67z" color="#000000" fill="#fff" stroke="#000"/> </g> <g transform="matrix(.76405 0 0 .6462 672.07 489.79)" stroke-width="32.021"> <g transform="matrix(1.3333 0 0 -1.3333 -398.39 -224.27)"> <path d="m0 0 53.721-151.22-94.323-246.38-139.33 0.813z"/> </g> <g transform="matrix(1.3333 0 0 -1.3333 -398.39 -224.27)"> <path d="m0 0 53.721-151.22-94.323-246.38-139.33 0.813z" fill="none" stroke="#000" stroke-linejoin="round" stroke-width="32.021"/> </g> <g transform="matrix(1.3333 0 0 -1.3333 -326.63 -22.648)"> <path d="m0 0 154.17 402.74 253.57-648.3h-320.79z" fill="#fff"/> </g> <g transform="matrix(1.3333 0 0 -1.3333 -326.63 -22.648)"> <path d="M 0,0 154.173,402.741 407.744,-245.563 H 86.95 L 0,0" fill="none" stroke="#000" stroke-linejoin="round" stroke-width="32.021"/> </g> <g transform="matrix(1.3333 0 0 -1.3333 2.1407 -244.5)"> <path d="m0 0-159.63-411.95" stroke="#000" stroke-width="32.021"/> </g> <g transform="matrix(1.3333 0 0 -1.3333 -104.22 30.076)"> <path d="m0 0 80.534-206.02" stroke="#000" stroke-width="32.021"/> </g> </g> </svg>)lit";

wxBitmap Bitmaps::GetBitmap(Bitmaps::ID id, const wxString &colour, const wxSize &size) {
    wxString my_bmp = wxString::Format(Bitmaps::SvgText[id], colour);
    return wxBitmapBundle::FromSVG(my_bmp, size).GetBitmap(size);
}

wxBitmap Bitmaps::GetLogo(const wxSize &size) {
    return wxBitmapBundle::FromSVG(Bitmaps::SvgLogo, size).GetBitmap(size);
}
