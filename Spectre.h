
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Spectre.h                                                //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.1                                                 //
//  Description: The main components and core code for the         //
//               Spectre inter-process communication components.   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#pragma once

namespace Spectre
{
    namespace Server
    {
        extern int Port; // Server port number.
        extern int MaxClients; // Maximum amount of clients.
        extern bool IsProtected; // Protect server from out-of-network connections

        void Close();
        int Start(void action(std::string script));
    }
}