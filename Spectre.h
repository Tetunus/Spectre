
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Spectre.h                                                //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.3                                                 //
//  Description: The main components and core code for the         //
//               Spectre inter-process communication components.   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#pragma once

namespace spectre
{
    struct config
    {
        int port = 8888; // Default value.
        int max_clients = 1; // Default value.
        bool is_protected = false; // Default value.
    };

    class server
    {
    public:
        struct config configuration; // Server configuration.

        void close();
        int start(void action(std::string script));
    };
}