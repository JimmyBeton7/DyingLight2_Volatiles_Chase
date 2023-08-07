/***************************************************************
 * Name:      DL2_Volatiles_MechanicMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2023-07-28
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef DL2_VOLATILES_MECHANICMAIN_H
#define DL2_VOLATILES_MECHANICMAIN_H

//(*Headers(DL2_Volatiles_MechanicFrame)
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/timer.h>
//*)

class DL2_Volatiles_MechanicFrame: public wxFrame
{
    public:

        DL2_Volatiles_MechanicFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~DL2_Volatiles_MechanicFrame();

    private:

        //(*Handlers(DL2_Volatiles_MechanicFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnPanel1Paint(wxPaintEvent& event);
        void OnTimerPlayerTrigger(wxTimerEvent& event);
        void OnButton1Click(wxCommandEvent& event);
        void OnPanel1LeftDown(wxMouseEvent& event);
        void OnTimerVolatile1Trigger(wxTimerEvent& event);
        void OnTimerVolatile2Trigger(wxTimerEvent& event);
        void OnTimerVolatile3Trigger(wxTimerEvent& event);
        //*)

        //(*Identifiers(DL2_Volatiles_MechanicFrame)
        static const long ID_BUTTON1;
        static const long ID_PANEL1;
        static const long ID_TIMER1;
        static const long ID_TIMER2;
        static const long ID_TIMER3;
        static const long ID_TIMER4;
        //*)

        //(*Declarations(DL2_Volatiles_MechanicFrame)
        wxButton* Button1;
        wxPanel* Panel1;
        wxTimer TimerPlayer;
        wxTimer TimerVolatile1;
        wxTimer TimerVolatile2;
        wxTimer TimerVolatile3;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // DL2_VOLATILES_MECHANICMAIN_H
