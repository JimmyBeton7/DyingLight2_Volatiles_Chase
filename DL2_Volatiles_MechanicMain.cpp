/***************************************************************
 * Name:      DL2_Volatiles_MechanicMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2023-07-28
 * Copyright:  ()
 * License:
 **************************************************************/

#include "DL2_Volatiles_MechanicMain.h"
#include <wx/msgdlg.h>
#include <wx/dcclient.h>
#include <wx/frame.h>
#include <wx/wx.h>
#include <time.h>
#include <wx/utils.h>
#include <random>
#include <wx/graphics.h>
#include <cmath>

//(*InternalHeaders(DL2_Volatiles_MechanicFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(DL2_Volatiles_MechanicFrame)
const long DL2_Volatiles_MechanicFrame::ID_BUTTON1 = wxNewId();
const long DL2_Volatiles_MechanicFrame::ID_PANEL1 = wxNewId();
const long DL2_Volatiles_MechanicFrame::ID_TIMER1 = wxNewId();
const long DL2_Volatiles_MechanicFrame::ID_TIMER2 = wxNewId();
const long DL2_Volatiles_MechanicFrame::ID_TIMER3 = wxNewId();
const long DL2_Volatiles_MechanicFrame::ID_TIMER4 = wxNewId();
//*)

BEGIN_EVENT_TABLE(DL2_Volatiles_MechanicFrame,wxFrame)
    //(*EventTable(DL2_Volatiles_MechanicFrame)
    //*)
END_EVENT_TABLE()

DL2_Volatiles_MechanicFrame::DL2_Volatiles_MechanicFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(DL2_Volatiles_MechanicFrame)
    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    SetClientSize(wxSize(1200,600));
    Panel1 = new wxPanel(this, ID_PANEL1, wxPoint(24,0), wxSize(1200,600), wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Button1 = new wxButton(Panel1, ID_BUTTON1, _("Start"), wxPoint(1072,8), wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    TimerPlayer.SetOwner(this, ID_TIMER1);
    TimerPlayer.Start(100, false);
    TimerVolatile1.SetOwner(this, ID_TIMER2);
    TimerVolatile2.SetOwner(this, ID_TIMER3);
    TimerVolatile3.SetOwner(this, ID_TIMER4);

    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnButton1Click);
    Panel1->Connect(wxEVT_PAINT,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnPanel1Paint,0,this);
    Panel1->Connect(wxEVT_LEFT_DOWN,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnPanel1LeftDown,0,this);
    Connect(ID_TIMER1,wxEVT_TIMER,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnTimerPlayerTrigger);
    Connect(ID_TIMER2,wxEVT_TIMER,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnTimerVolatile1Trigger);
    Connect(ID_TIMER3,wxEVT_TIMER,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnTimerVolatile2Trigger);
    Connect(ID_TIMER4,wxEVT_TIMER,(wxObjectEventFunction)&DL2_Volatiles_MechanicFrame::OnTimerVolatile3Trigger);
    //*)
}

DL2_Volatiles_MechanicFrame::~DL2_Volatiles_MechanicFrame()
{
    //(*Destroy(DL2_Volatiles_MechanicFrame)
    //*)
}

enum class VolatileState
{
    IDLE,
    CHASING_PLAYER,
    MOVING_TO_RANDOM_POINT
};

int ScreenX;
int ScreenY;
wxPoint PlayerPosition(600,300);
int a,b;
wxPoint ClickPoint;
bool MoveToClick = false;
const int StepSize = 10;
bool IsChasingPlayer = false;

//-----------------------------------------------------
wxPoint Volatile1Position(30,30);
bool MoveToPointV1 = false;
wxPoint MoveToV1;
wxPoint MoveToRandomV1;
wxPoint MoveToPlayerV1;
const int StepSizeV1 = 2;
double viewAngle = M_PI / 4.0;
double triangleRotation = 0.0;
double V1Angle;
VolatileState volatile1State = VolatileState::IDLE;
double radius = 50;
double distanceToPlayer1;
//-----------------------------------------------------
wxPoint Volatile2Position(1100,30);
wxPoint MoveToV2;
const int StepSizeV2 = 2;
double viewAngleV2 = M_PI / 4.0;
double triangleRotationV2 = 0.0;
double V2Angle;
VolatileState volatile2State = VolatileState::IDLE;
double distanceToPlayer2;
//-----------------------------------------------------
wxPoint Volatile3Position(600,550);
wxPoint MoveToV3;
const int StepSizeV3 = 2;
double viewAngleV3 = M_PI / 4.0;
double triangleRotationV3 = 0.0;
double V3Angle;
VolatileState volatile3State = VolatileState::IDLE;
double distanceToPlayer3;
//-----------------------------------------------------

double CalculateDistance(const wxPoint& p1, const wxPoint& p2)
{
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    return std::sqrt(dx * dx + dy * dy);
}

double CalculateVolatileAngle(wxPoint& vol_pos, wxPoint& movingto)
{
    double dx = movingto.x - vol_pos.x;
    double dy = movingto.y - vol_pos.y;
    double angle = atan2(dy, dx);
    return angle;
}

bool IsPointInTriangle(const wxPoint& pt, const wxPoint& v1, const wxPoint& v2, const wxPoint& v3)
{
    auto sign = [](const wxPoint& p1, const wxPoint& p2, const wxPoint& p3) -> double
    {
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    bool has_neg = false;
    bool has_pos = false;

    double d1 = sign(pt, v1, v2);
    if (d1 < 0) has_neg = true;
    else if (d1 > 0) has_pos = true;

    double d2 = sign(pt, v2, v3);
    if (d2 < 0) has_neg = true;
    else if (d2 > 0) has_pos = true;

    double d3 = sign(pt, v3, v1);
    if (d3 < 0) has_neg = true;
    else if (d3 > 0) has_pos = true;

    return !(has_neg && has_pos);
}

void DrawTriangle(wxDC& dc, const wxPoint& center, int radius, double angle)
{
    wxPoint points[3];
    points[0] = center;
    points[1] = wxPoint(center.x + radius * cos(angle - viewAngle / 2.0),
                        center.y + radius * sin(angle - viewAngle / 2.0));
    points[2] = wxPoint(center.x + radius * cos(angle + viewAngle / 2.0),
                        center.y + radius * sin(angle + viewAngle / 2.0));
    dc.DrawPolygon(3, points);
}


void DL2_Volatiles_MechanicFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void DL2_Volatiles_MechanicFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void DL2_Volatiles_MechanicFrame::OnPanel1Paint(wxPaintEvent& event)
{
    ScreenX = Panel1->GetSize().GetWidth();
    ScreenY = Panel1->GetSize().GetHeight();
    //PlayerPosition = wxPoint(ScreenX / 2, ScreenY / 2);
    wxPaintDC dc(Panel1);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.DrawCircle(PlayerPosition, 5);

    //-----------------------------------------------
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawCircle(Volatile1Position,5);
    if(volatile1State == VolatileState::MOVING_TO_RANDOM_POINT || volatile1State == VolatileState::IDLE)
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
    }
    else
    {
        dc.SetBrush(wxBrush(wxColour(255, 0, 0, 128)));
    }
    //dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxColour(255, 0, 0, 64));
    DrawTriangle(dc, Volatile1Position, 200, atan2(MoveToV1.y - Volatile1Position.y, MoveToV1.x - Volatile1Position.x));

    //-----------------------------------------------
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawCircle(Volatile2Position,5);
    if(volatile2State == VolatileState::MOVING_TO_RANDOM_POINT || volatile2State == VolatileState::IDLE)
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
    }
    else
    {
        dc.SetBrush(wxBrush(wxColour(255, 0, 0, 128)));
    }
    //dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxColour(255, 0, 0, 64));
    DrawTriangle(dc, Volatile2Position, 200, atan2(MoveToV2.y - Volatile2Position.y, MoveToV2.x - Volatile2Position.x));

    //-----------------------------------------------
    dc.SetBrush(*wxRED_BRUSH);
    dc.DrawCircle(Volatile3Position,5);
    if(volatile3State == VolatileState::MOVING_TO_RANDOM_POINT || volatile3State == VolatileState::IDLE)
    {
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
    }
    else
    {
        dc.SetBrush(wxBrush(wxColour(255, 0, 0, 128)));
    }
    //dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(wxColour(255, 0, 0, 64));
    DrawTriangle(dc, Volatile3Position, 200, atan2(MoveToV3.y - Volatile3Position.y, MoveToV3.x - Volatile3Position.x));
    //-----------------------------------------------

    srand(time(NULL));
    SetDoubleBuffered(true);
}

void DL2_Volatiles_MechanicFrame::OnTimerPlayerTrigger(wxTimerEvent& event)
{
    if (MoveToClick)
    {
        int dx = ClickPoint.x - PlayerPosition.x;
        int dy = ClickPoint.y - PlayerPosition.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSize)
        {
            PlayerPosition = ClickPoint;
            MoveToClick = false;
            TimerPlayer.Stop();
        }
        else
        {
            double ratio = StepSize / distance;
            PlayerPosition.x += static_cast<int>(dx * ratio);
            PlayerPosition.y += static_cast<int>(dy * ratio);
        }

        Panel1->Refresh();
    }
}

void DL2_Volatiles_MechanicFrame::OnButton1Click(wxCommandEvent& event)
{
    MoveToV1 = wxPoint(rand() % 300, rand() % 300);
    TimerVolatile1.Start(10);
    volatile1State = VolatileState::MOVING_TO_RANDOM_POINT;

    MoveToV2 = wxPoint(rand() % 300 + 900, rand() % 300);
    TimerVolatile2.Start(10);
    volatile2State = VolatileState::MOVING_TO_RANDOM_POINT;

    MoveToV3 = wxPoint(rand() % 400 + 400, rand() % 300 + 300);
    TimerVolatile3.Start(10);
    volatile3State = VolatileState::MOVING_TO_RANDOM_POINT;
}

void DL2_Volatiles_MechanicFrame::OnPanel1LeftDown(wxMouseEvent& event)
{
    ClickPoint = event.GetPosition();
    MoveToClick = true;
    TimerPlayer.Start(10);
}

void DL2_Volatiles_MechanicFrame::OnTimerVolatile1Trigger(wxTimerEvent& event)
{
    V1Angle = CalculateVolatileAngle(Volatile1Position, MoveToV1);
    wxPoint trianglePoints[3];
    trianglePoints[0] = Volatile1Position;
    trianglePoints[1] = wxPoint(Volatile1Position.x + radius * cos(V1Angle - viewAngle / 2.0),
                                Volatile1Position.y + radius * sin(V1Angle - viewAngle / 2.0));
    trianglePoints[2] = wxPoint(Volatile1Position.x + radius * cos(V1Angle + viewAngle / 2.0),
                                Volatile1Position.y + radius * sin(V1Angle + viewAngle / 2.0));

    bool playerIsInView = IsPointInTriangle(PlayerPosition, trianglePoints[0], trianglePoints[1], trianglePoints[2]);
    distanceToPlayer1 = CalculateDistance(Volatile1Position, PlayerPosition);

    if (playerIsInView || (volatile1State == VolatileState::CHASING_PLAYER && distanceToPlayer1 < 500))
    {
        MoveToV1 = PlayerPosition;
        volatile1State = VolatileState::CHASING_PLAYER;
        MoveToV2 = PlayerPosition;
        volatile2State = VolatileState::CHASING_PLAYER;
        MoveToV3 = PlayerPosition;
        volatile3State = VolatileState::CHASING_PLAYER;
    }
    else if (volatile1State == VolatileState::CHASING_PLAYER && distanceToPlayer1 > 500 && distanceToPlayer2 > 500 && distanceToPlayer3 > 500)
    {
        volatile1State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV1 = wxPoint(rand() % 300, rand() % 300);
        volatile2State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV2 = wxPoint(rand() % 300 + 900, rand() % 300);
        volatile3State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV3 = wxPoint(rand() % 400 + 400, rand() % 300 + 300);
    }

    if (volatile1State == VolatileState::MOVING_TO_RANDOM_POINT)
    {
        int dx = MoveToV1.x - Volatile1Position.x;
        int dy = MoveToV1.y - Volatile1Position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSizeV1)
        {
            Volatile1Position = MoveToV1;
            TimerVolatile1.Stop();
            MoveToV1 = wxPoint(rand() % 300, rand() % 300);
            TimerVolatile1.Start(10);
        }
        else
        {
            double ratio = StepSizeV1 / distance;
            Volatile1Position.x += static_cast<int>(dx * ratio);
            Volatile1Position.y += static_cast<int>(dy * ratio);
        }
    }
    else if (volatile1State == VolatileState::CHASING_PLAYER)
    {
        int dx = MoveToV1.x - Volatile1Position.x;
        int dy = MoveToV1.y - Volatile1Position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSizeV1)
        {
            Volatile1Position = MoveToV1;
            TimerVolatile1.Stop();

            MoveToV1 = wxPoint(Volatile1Position.x, Volatile1Position.y);
            TimerVolatile1.Start(10);
        }
        else
        {
            double ratio = StepSizeV1 / distance;
            Volatile1Position.x += static_cast<int>(dx * ratio);
            Volatile1Position.y += static_cast<int>(dy * ratio);
        }
    }

    Refresh();
}

void DL2_Volatiles_MechanicFrame::OnTimerVolatile2Trigger(wxTimerEvent& event)
{
    V2Angle = CalculateVolatileAngle(Volatile2Position, MoveToV2);
    wxPoint trianglePoints[3];
    trianglePoints[0] = Volatile2Position;
    trianglePoints[1] = wxPoint(Volatile2Position.x + radius * cos(V2Angle - viewAngleV2 / 2.0),
                                Volatile2Position.y + radius * sin(V2Angle - viewAngleV2 / 2.0));
    trianglePoints[2] = wxPoint(Volatile2Position.x + radius * cos(V2Angle + viewAngleV2 / 2.0),
                                Volatile2Position.y + radius * sin(V2Angle + viewAngleV2 / 2.0));

    bool playerIsInView = IsPointInTriangle(PlayerPosition, trianglePoints[0], trianglePoints[1], trianglePoints[2]);
    distanceToPlayer2 = CalculateDistance(Volatile2Position, PlayerPosition);

    if (playerIsInView || (volatile2State == VolatileState::CHASING_PLAYER && distanceToPlayer2 < 500))
    {
        MoveToV2 = PlayerPosition;
        volatile2State = VolatileState::CHASING_PLAYER;
        MoveToV1 = PlayerPosition;
        volatile1State = VolatileState::CHASING_PLAYER;
        MoveToV3 = PlayerPosition;
        volatile3State = VolatileState::CHASING_PLAYER;
    }
    else if (volatile2State == VolatileState::CHASING_PLAYER && distanceToPlayer2 > 500 && distanceToPlayer1 > 500 && distanceToPlayer3 > 500)
    {
        volatile2State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV2 = wxPoint(rand() % 300 + 900, rand() % 300);
        volatile1State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV1 = wxPoint(rand() % 300, rand() % 300);
        volatile3State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV3 = wxPoint(rand() % 400 + 400, rand() % 300 + 300);
    }

    if (volatile2State == VolatileState::MOVING_TO_RANDOM_POINT)
    {
        int dx = MoveToV2.x - Volatile2Position.x;
        int dy = MoveToV2.y - Volatile2Position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSizeV2)
        {
            Volatile2Position = MoveToV2;
            TimerVolatile2.Stop();
            MoveToV2 = wxPoint(rand() % 300 + 900, rand() % 300);
            TimerVolatile2.Start(10);
        }
        else
        {
            double ratio = StepSizeV2 / distance;
            Volatile2Position.x += static_cast<int>(dx * ratio);
            Volatile2Position.y += static_cast<int>(dy * ratio);
        }
    }
    else if (volatile2State == VolatileState::CHASING_PLAYER)
    {
        int dx = MoveToV2.x - Volatile2Position.x;
        int dy = MoveToV2.y - Volatile2Position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSizeV2)
        {
            Volatile2Position = MoveToV2;
            TimerVolatile2.Stop();

            MoveToV2 = wxPoint(Volatile2Position.x, Volatile2Position.y);
            TimerVolatile2.Start(10);
        }
        else
        {
            double ratio = StepSizeV2 / distance;
            Volatile2Position.x += static_cast<int>(dx * ratio);
            Volatile2Position.y += static_cast<int>(dy * ratio);
        }
    }

    Refresh();
}

void DL2_Volatiles_MechanicFrame::OnTimerVolatile3Trigger(wxTimerEvent& event)
{
    V3Angle = CalculateVolatileAngle(Volatile3Position, MoveToV3);
    wxPoint trianglePoints[3];
    trianglePoints[0] = Volatile3Position;
    trianglePoints[1] = wxPoint(Volatile3Position.x + radius * cos(V3Angle - viewAngleV3 / 2.0),
                                Volatile3Position.y + radius * sin(V3Angle - viewAngleV3 / 2.0));
    trianglePoints[2] = wxPoint(Volatile3Position.x + radius * cos(V3Angle + viewAngleV3 / 2.0),
                                Volatile3Position.y + radius * sin(V3Angle + viewAngleV3 / 2.0));

    bool playerIsInView = IsPointInTriangle(PlayerPosition, trianglePoints[0], trianglePoints[1], trianglePoints[2]);
    distanceToPlayer3 = CalculateDistance(Volatile3Position, PlayerPosition);

    if (playerIsInView || (volatile3State == VolatileState::CHASING_PLAYER && distanceToPlayer3 < 500))
    {
        MoveToV3 = PlayerPosition;
        volatile3State = VolatileState::CHASING_PLAYER;
        MoveToV1 = PlayerPosition;
        volatile1State = VolatileState::CHASING_PLAYER;
        MoveToV2 = PlayerPosition;
        volatile2State = VolatileState::CHASING_PLAYER;
    }
    else if (volatile3State == VolatileState::CHASING_PLAYER && distanceToPlayer2 > 500 && distanceToPlayer1 > 500 && distanceToPlayer3 > 500)
    {
        volatile3State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV3 = wxPoint(rand() % 400 + 400, rand() % 300 + 300);
        volatile2State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV2 = wxPoint(rand() % 300 + 900, rand() % 300);
        volatile1State = VolatileState::MOVING_TO_RANDOM_POINT;
        MoveToV1 = wxPoint(rand() % 300, rand() % 300);
    }

    if (volatile3State == VolatileState::MOVING_TO_RANDOM_POINT)
    {
        int dx = MoveToV3.x - Volatile3Position.x;
        int dy = MoveToV3.y - Volatile3Position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSizeV3)
        {
            Volatile3Position = MoveToV3;
            TimerVolatile3.Stop();
            MoveToV3 = wxPoint(rand() % 400 + 400, rand() % 300 + 300);
            TimerVolatile3.Start(16);
        }
        else
        {
            double ratio = StepSizeV3 / distance;
            Volatile3Position.x += static_cast<int>(dx * ratio);
            Volatile3Position.y += static_cast<int>(dy * ratio);
        }
    }
    else if (volatile3State == VolatileState::CHASING_PLAYER)
    {
        int dx = MoveToV3.x - Volatile3Position.x;
        int dy = MoveToV3.y - Volatile3Position.y;
        double distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= StepSizeV3)
        {
            Volatile3Position = MoveToV3;
            TimerVolatile3.Stop();

            MoveToV3 = wxPoint(Volatile3Position.x, Volatile3Position.y);
            TimerVolatile3.Start(16);
        }
        else
        {
            double ratio = StepSizeV3 / distance;
            Volatile3Position.x += static_cast<int>(dx * ratio);
            Volatile3Position.y += static_cast<int>(dy * ratio);
        }
    }

    Refresh();
}
