
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Example.cpp                                              //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.1                                                 //
//  Description: An example main function and little show on how   //
//               to use Spectre. Plus a bonus DllMain example.     //
//                                                                 //
/////////////////////////////////////////////////////////////////////

//
// Documentation:
//     [int] Spectre::Server::Port         - The port the server will be running on. (default: 8888)
//     [int] Spectre::Server::MaxClients   - The maximum amount of clients the server allows. (default: 1)
//    [bool] Spectre::Server::IsProtected  - Protect server from out-of-network connections. (default: false)
//     [int] Spectre::Server::Start(void)  - Starts the server on the designated port you set.
//    [void] Spectre::Server::Close()      - Stops the server from running and flushes resources.
//

#include <iostream>
#include <windows.h>

#include "Spectre.h"

void ExecuteExample(std::string script)
{
    // Handle and or execute script here.
}

int main()
{
    SetConsoleTitleA("Spectre  -  Interprocess Communication Server"); // Purpose: Just for fancy console title.

    Spectre::Server::Port = 8888; // Purpose: This is the port the server will run on. (default: 8888)
    Spectre::Server::MaxClients = 1; // Purpose: This is the maximum amount of clients the server will allow. (default: 1)
    Spectre::Server::IsProtected = false;  // Purpose: Protect the server from out-of-network connections. (default: false)

    Spectre::Server::Start(ExecuteExample); // Purpose: Initalize and start the Spectre server.

    std::cin.get(); // Purpose: This will pause the application after the server ends.

    Spectre::Server::Start(ExecuteExample); // Purpose: This will reinitalize the server after it started.. technically a restart.
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            // Preform all actions before the server starts..

            // Configure the server to your prefered settings.
            Spectre::Server::Port = 8888; 
            Spectre::Server::MaxClients = 1; 
            Spectre::Server::IsProtected = false;

            // Start the server!
            Spectre::Server::Start(ExecuteExample);
            break;

        case DLL_PROCESS_DETACH:
            // Close the server, just a little cleanup if you will.
            Spectre::Server::Close();
            break;
    }

    return TRUE;
}