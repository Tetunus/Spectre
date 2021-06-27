
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Example.cpp                                              //
//  Author: Tetunus (Josh)                                         //
//  Description: An example main function and little show on how   //
//               to use Spectre. Plus a bonus DllMain example.     //
//                                                                 //
/////////////////////////////////////////////////////////////////////

//
//  Credits:
//     static   - Moral support.
//     xgladius - Helped find and fix some memory leaks.
//     Koaxy    - Helped test alpha builds and found buffer issues.
//

//
//  Documentation:
//     [int] configuration.port          - The port the server will be running on. (default: 8888)
//     [int] configuration.max_clients   - The maximum amount of clients the server allows. (default: 1)
//    [bool] configuration.is_protected  - Protect server from out-of-network connections. (default: false)
//     [int] server.start(void)          - Starts the server on the designated port you set.
//    [void] server.close()              - Stops the server from running and flushes resources.
//

#include <iostream>
#include <windows.h>

#include "Spectre.h"

void data_handler(std::string data)
{
    // Handle all incoming data here..
}

int main()
{
    SetConsoleTitleA("Spectre  -  Interprocess Communication Server"); // Purpose: Just for fancy console title.

    spectre::server server;

    server.configuration.port = 8888; // Purpose: This is the port the server will run on. (default: 8888)
    server.configuration.max_clients = 1; // Purpose: This is the maximum amount of clients the server will allow. (default: 1)
    server.configuration.is_protected = false; // Purpose: Protect the server from out-of-network connections. (default: false)

    server.start(data_handler); // Purpose: Initalize and start the Spectre server.

    std::cin.get(); // Purpose: This will pause the application after the server ends.

    server.start(data_handler); // Purpose: This will reinitalize the server after it started.. technically a restart.
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    spectre::server server;

    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            // Preform all actions before the server starts..

            // Configure the server to your prefered settings.
            server.configuration.port = 8888;
            server.configuration.max_clients = 1;
            server.configuration.is_protected = false;

            // Start the server!
            server.start(data_handler);
            break;

        case DLL_PROCESS_DETACH:
            // Close the server, just a little cleanup if you will.
            server.close();
            break;
    }

    return TRUE;
}
