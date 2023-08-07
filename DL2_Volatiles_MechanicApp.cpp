/***************************************************************
 * Name:      DL2_Volatiles_MechanicApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2023-07-28
 * Copyright:  ()
 * License:
 **************************************************************/

#include "DL2_Volatiles_MechanicApp.h"

//(*AppHeaders
#include "DL2_Volatiles_MechanicMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(DL2_Volatiles_MechanicApp);

bool DL2_Volatiles_MechanicApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	DL2_Volatiles_MechanicFrame* Frame = new DL2_Volatiles_MechanicFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
