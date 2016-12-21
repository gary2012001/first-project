/////////////////////////////////////////////////////////////////////////////
// Name:            server.cpp
// Purpose:         Socket Event Demo (server)
// Author:          Kevin Hock
// Copyright:   (c) 2005 Kevin Hock
// Licence:         wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// declarations
// ==========================================================================

// --------------------------------------------------------------------------
// headers
// --------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(__APPLE__)
#  pragma implementation "server.cpp"
#  pragma interface "server.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "windows.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#  include "wx/wx.h"
#endif

#include "wx/socket.h"
#include<wx/file.h>

// --------------------------------------------------------------------------
// resources
// --------------------------------------------------------------------------

// the application icon
#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#  include "mondrian.xpm"
#endif

// --------------------------------------------------------------------------
// classes
// --------------------------------------------------------------------------

// Define a new application type
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    MyFrame();
    ~MyFrame();

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnServerEvent(wxSocketEvent& event);
    void OnSocketEvent(wxSocketEvent& event);
    void OnServerStart(wxCommandEvent& event);


    void OnServerEvent_cmd(wxSocketEvent& event);
    void OnSocketEvent_cmd(wxSocketEvent& event);

    void OnParaSpeed(wxCommandEvent& event);
    void OnParaFixfrm(wxCommandEvent& event);

private:
    wxSocketServer *m_server;


    wxSocketServer *cmd_server;

    wxTextCtrl     *m_text;
    wxMenu         *m_menuFile;
    wxMenu         *m_menuPara;
    wxMenuBar      *m_menuBar;

    wxString filename;
    wxFile filesend;

    bool fixframe_enable = 0;
    unsigned int player_speed = 0;
    unsigned int fixframe = 0;
    unsigned int countstart = 0;
    unsigned int countend = 20000;
    unsigned int frame_count = 0;
    bool sendstatus = 0;


    LARGE_INTEGER timefreqence;
    LARGE_INTEGER timestart;
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// --------------------------------------------------------------------------
// constants
// --------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    SERVER_QUIT = 1000,
    SERVER_START,
    PARA_SPEED,
    PARA_FIXFRM,

    // id for sockets
    SERVER_ID,
    SOCKET_ID,
    SERVER_ID_CMD,
    SOCKET_ID_CMD

};

// --------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// --------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(SERVER_QUIT,  MyFrame::OnQuit)
    EVT_SOCKET(SERVER_ID,  MyFrame::OnServerEvent)
    EVT_SOCKET(SOCKET_ID,  MyFrame::OnSocketEvent)
    EVT_SOCKET(SERVER_ID_CMD,  MyFrame::OnServerEvent_cmd)
    EVT_SOCKET(SOCKET_ID_CMD,  MyFrame::OnSocketEvent_cmd)
    EVT_MENU(SERVER_START, MyFrame::OnServerStart)
    EVT_MENU(PARA_SPEED, MyFrame::OnParaSpeed)
    EVT_MENU(PARA_FIXFRM, MyFrame::OnParaFixfrm)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)


// ==========================================================================
// implementation
// ==========================================================================

// --------------------------------------------------------------------------
// the application class
// --------------------------------------------------------------------------

bool MyApp::OnInit()
{
    // Create the main application window
    MyFrame *frame = new MyFrame();

    // Show it and tell the application that it's our main window
    frame->Show(true);
    SetTopWindow(frame);

    // Success
    return true;
}

// --------------------------------------------------------------------------
// main frame
// --------------------------------------------------------------------------

// frame constructor

MyFrame::MyFrame() : wxFrame(NULL, wxID_ANY, _("wxSocket Event Demo: Server"),
                                 wxDefaultPosition, wxSize(300, 200))
{
    m_server = NULL;

    // Give the frame an icon
    SetIcon(wxICON(mondrian));

    // Make menus
    m_menuFile = new wxMenu();
    m_menuFile->Append(SERVER_START, _("&Start"), _("Start the server"));
    m_menuFile->AppendSeparator();
    m_menuFile->Append(SERVER_QUIT, _("E&xit\tAlt-X"), _("Quit server"));

    m_menuPara = new wxMenu();
    m_menuPara->Append(PARA_SPEED,_("&Speed"),_("Player Speed"));
    m_menuPara->Append(PARA_FIXFRM,_("&Fixfrm"),_("Fix play frame"));

    // Append menus to the menubar
    m_menuBar = new wxMenuBar();
    m_menuBar->Append(m_menuFile, _("&File"));
    m_menuBar->Append(m_menuPara, _("&Parameter"));
    SetMenuBar(m_menuBar);


    // Make a textctrl for logging
    m_text  = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

    m_text->AppendText(_("Welcome to the Socket Event Demo: Server\nYou need to Start the server!\n"));
}

MyFrame::~MyFrame()
{
    // No delayed deletion here, as the frame is dying anyway
    delete m_server;
}

// event handlers

void MyFrame::OnServerStart(wxCommandEvent& WXUNUSED(event))
{
    // Create the address - defaults to localhost

    wxFileDialog dialogOpen( this, wxT("Please choose an model parameter file"),
                             wxEmptyString, wxEmptyString, wxT("*.raw"), wxFD_OPEN);
    if (dialogOpen.ShowModal() == wxID_OK)
    {
        filename = dialogOpen.GetPath();
    }


    static bool server_start = 1;

    if(server_start == 1)
    {


        wxIPV4address addr;
        addr.Service(12345);

        // Create the socket, we maintain a class pointer so we can shut it down
        m_server = new wxSocketServer(addr);

        // We use Ok() here to see if the server is really listening
        if (! m_server->Ok())
        {
            return;
        }

        m_text->AppendText(_("\nSocket server listening12345.\n"));

        // Setup the event handler and subscribe to connection events
        m_server->SetEventHandler(*this, SERVER_ID);
        m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
        m_server->Notify(true);

        wxIPV4address addr1;
        addr1.Service(23456);


        cmd_server = new wxSocketServer(addr1);

        // We use Ok() here to see if the server is really listening
        if (! cmd_server->Ok())
        {
            return;
        }

        m_text->AppendText(_("\nSocket server listening23456.\n"));

        // Setup the event handler and subscribe to connection events
        cmd_server->SetEventHandler(*this, SERVER_ID_CMD);
        cmd_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
        cmd_server->Notify(true);
        server_start = 0;
    }
    // Create the socket, we maintain a class pointer so we can shut it down
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnParaSpeed(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Enter Player speed(1~50)", "Enter Player speed", "",
            wxOK | wxCANCEL | wxCENTRE );
    if ( dlg->ShowModal() == wxID_OK )
    {
        wxString str = dlg->GetValue();
        unsigned int temp = wxAtoi(str);
        if ((temp>0 )&&(temp<51))
        {
            player_speed = 1000/temp;
        }
        wxMessageBox(str, _("password"));
    }
    else
    {
        // Stop the application
    }
    dlg->Destroy();
}

void MyFrame::OnParaFixfrm(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog *dlg = new wxTextEntryDialog(this, "Enter Fix Frame(0,xxx,xxx)", "Enter Fix Frame", "",
            wxOK | wxCANCEL | wxCENTRE );
    if ( dlg->ShowModal() == wxID_OK )
    {
        wxString str = dlg->GetValue();
        char data[16];
        strcpy(data, (const char*)str.mb_str(wxConvUTF8));
        if ((data[0] == 0x30)&&(data[1]==44))
        {
            fixframe_enable = 0;
            int j=0,counttemp1=0,counttemp2=0;
            for(int i = 0; i<(int)str.length(); i++)
            {
                if((data[i] == 44))
                {
                    j++;
                    //break;
                }
                else
                {
                    if(j == 1)
                    {
                        counttemp1 = counttemp1*10 + data[i]-0x30;
                    }
                    if(j == 2)
                    {
                        counttemp2 = counttemp2*10 + data[i]-0x30;
                    }

                }
            }
            countstart = counttemp1;
            countend = counttemp2;
        }
        else
        {
            countstart = 0;
            countend = 20000;

        }
        /*
        else if(data[0] == 0x31)
        {
        fixframe_enable = 1;
        }
        if(data[1] == 44)
        {
        fixframe = atoi(data+2);
        }*/
        //wxMessageBox(data, _("data"));
    }
    else
    {
        // Stop the application
    }
    dlg->Destroy();
}

void MyFrame::OnServerEvent(wxSocketEvent& WXUNUSED(event))
{
    // Accept the new connection and get the socket pointer
    wxSocketBase* sock = m_server->Accept(false);

    // Tell the new socket how and where to process its events
    sock->SetEventHandler(*this, SOCKET_ID);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);

    m_text->AppendText(_("\nAccepted incoming connection12345.\n"));


    if(!QueryPerformanceFrequency(&timefreqence))
    {
        wxMessageBox(_("Your computer does not support high precision timers."));
    }

    QueryPerformanceCounter(&timestart);

}

void MyFrame::OnServerEvent_cmd(wxSocketEvent& WXUNUSED(event))
{
    // Accept the new connection and get the socket pointer
    wxSocketBase* sock = cmd_server->Accept(false);

    // Tell the new socket how and where to process its events
    sock->SetEventHandler(*this, SOCKET_ID_CMD);
    sock->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    sock->Notify(true);

    m_text->AppendText(_("\nAccepted incoming connection23456.\n"));


}

#define sendlength 2048000

void MyFrame::OnSocketEvent(wxSocketEvent& event)
{
    wxSocketBase *sock = event.GetSocket();

    // Process the event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_CONNECTION:
    {
        sendstatus = 0;
        /*
        if (filesend.IsOpened())
        {
            filesend.Close();
        }*/
        wxFile filesend(filename,wxFile::read);
        if (!filesend.IsOpened())
        {
            m_text->AppendText(_("File open fail\n"));
        }
        sendstatus = 1;
        break;
    }
    case wxSOCKET_INPUT:
    {
        char buf[10];
        char* sendbuf;

        // Read the data
        sock->Read(buf, sizeof(buf));

        if(buf[0] != 0x08)
        {
            //wxString numb = wxString::Format(wxT("%i"),buf[0]);
            //m_text->AppendText(wxString(_("Receive from client: ")) + numb + _("\n"));
            sock->Write(buf, 2);
        }
        else
        {
            if(sendstatus == 1)
            {
                LARGE_INTEGER timenow= {0};
                QueryPerformanceCounter(&timenow);
                double times=( ((timenow.QuadPart -
                                 timestart.QuadPart)*1000000)/(double)timefreqence.QuadPart);
                timestart = timenow;

                wxString numb = wxString::Format(wxT("Frame count:%i Send time:%f ms"),frame_count,times/1000);
                m_text->AppendText(wxString(_("")) + numb + _("\n"));

                sendbuf =(char *) malloc(2*1024*1024);
                if(frame_count > countend)
                {
                    frame_count = countstart;
                }
                filesend.Seek(sendlength);
                filesend.Read(sendbuf,sendlength);
                sock->Write(sendbuf,sendlength);
                frame_count++;
                //sock->Write(sendbuf,2);
                Sleep(player_speed);
                if(frame_count*sendlength>=filesend.Length())
                    frame_count = countstart;
                free(sendbuf);
            }
        }
        break;
    }
    case wxSOCKET_LOST:
    {
        sock->Destroy();
        break;
    }
    case wxSOCKET_OUTPUT:
    {
        break;
    }
    }
}

void MyFrame::OnSocketEvent_cmd(wxSocketEvent& event)
{
    wxSocketBase *sock = event.GetSocket();

    // Process the event
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:
    {
        char buf[10];
        sock->Read(buf, sizeof(buf));
        m_text->AppendText(wxString(_("Received CMD from client: ")) + wxString(buf, wxConvUTF8, 10) + _("\n"));
        char tempstr[16];
        wxFile recfile("./frame_count_err.txt",wxFile::write_append);
        sprintf(tempstr,"%i  \r\n",frame_count);
        recfile.Write(tempstr,sizeof(tempstr));
        recfile.Close();
        // Write it back
        sock->Write(buf, sizeof(buf));

        //m_text->AppendText(_("Wrote string back to client.\n"));
        break;

    }
    case wxSOCKET_LOST:
    {
        sock->Destroy();
        break;
    }
    case wxSOCKET_OUTPUT:
    {
        break;
    }
    case wxSOCKET_CONNECTION:
    {
        break;
    }

    }
}

