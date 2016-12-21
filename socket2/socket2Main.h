/***************************************************************
 * Name:      socket2Main.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2016-11-10
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef SOCKET2MAIN_H
#define SOCKET2MAIN_H

//(*Headers(socket2Frame)
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class socket2Frame: public wxFrame
{
    public:

        socket2Frame(wxWindow* parent,wxWindowID id = -1);
        virtual ~socket2Frame();

    private:

        //(*Handlers(socket2Frame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(socket2Frame)
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(socket2Frame)
        wxStatusBar* StatusBar1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // SOCKET2MAIN_H
