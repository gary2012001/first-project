/***************************************************************
 * Name:      socket2App.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2016-11-10
 * Copyright:  ()
 * License:
 **************************************************************/

#include "socket2App.h"

//(*AppHeaders
#include "socket2Main.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(socket2App);

bool socket2App::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	socket2Frame* Frame = new socket2Frame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
