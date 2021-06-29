
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Spectre.cpp                                              //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.3                                                 //
//  Description: The main components and core code for the         //
//               Spectre inter-process communication components.   //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <io.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")

#include "Spectre.h"
#include "Spectre.Util.h"

namespace spectre
{
    WSADATA wsa_data; // WinSock data bullshit.
    SOCKET client_socket[30]; // The client socket.
    SOCKET master_socket; // The master socket.
    SOCKET fake_socket; // The fake socket.
    SOCKET new_socket; // The new socket.
    std::string script; // The stored value that the buffer gets added to.
    struct sockaddr_in address; // Address socket data.
    struct sockaddr_in sserver; // Server socket data.
    int activity; // Socket activity.
    int addrlen; // Address length.
    int valread; // Value reading data.
    int maxbuffer = 65536; // Maximum buffer length. (do not change!)
    int i; // The illiteration constipation proclamation
    bool closed = false; // Causes the server to stop when true.
    char* buffer; // It is the fucking buffer, buffer go BRRRRRR.
    char* message = (char*)"Spectre 0.0.1 \r\n"; // Coming soon, for now version.
    char* limiter; // Out-of-network protection IP limiter.
    fd_set readfds; // Socket FD set.

    /// <summary>
    /// Stop the Spectre server and cleanup used resources.
    /// </summary>
    void server::close()
    {
        printf("[Spectre -> Debug] Stopping server..\n");

        closed = true;
    }

    /// <summary>
    /// Start the Spectre server and allow clients to join and send data.
    /// </summary>
    /// <param name="action">Action to handle the returned script/data.</param>
    /// <returns>Retrieved data from client.</returns>
    int server::start(void action(std::string script))
    {
        // Allocation constipation proclamation... deez nuts
        buffer = (char*)malloc((static_cast<unsigned long long>(maxbuffer) + 1) * sizeof(char));

        // Clean the client sockets with the MaxClients in mind
        for (i = 0; i < configuration.max_clients; i++)
        {
            client_socket[i] = 0;
        }

        // We check if the server was stopped to reset some values
        if (closed == true)
        {
            char* buffer = NULL;
            char* message = (char*)"Spectre 0.0.1 \r\n";

            closed = false;
        }

        printf("[Spectre -> Debug] Initialising.. ");

        // Valid port check
        if (configuration.port < 255 || configuration.port > 65535)
        {
            printf(" Failed.\n");
            printf("[Spectre -> Error] It seems your Port number was below 255 or above 65535, please change this and try again.\n");
            std::cin.get();
            exit(EXIT_FAILURE);
        }

        // Valid max clients check
        if (configuration.max_clients < 1 || configuration.max_clients > 30)
        {
            printf(" Failed.\n");
            printf("[Spectre -> Error] It seems your MaxClients was below 1 or above 30, please change this and try again.\n");
            std::cin.get();
            exit(EXIT_FAILURE);
        }

        // Start WSA
        if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
        {
            printf(" Failed.\n");
            printf("[Spectre -> Error] Failed to initalize, error code : %d", WSAGetLastError());
            std::cin.get();
            exit(EXIT_FAILURE);
        }

        printf("Done.\n"); // Successfully initalized WSA/wsa_data

        // Create the master socket
        if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            printf("[Spectre -> Error] Could not create socket, error code : %d\n", WSAGetLastError());
            std::cin.get();
            exit(EXIT_FAILURE);
        }

        printf("[Spectre -> Debug] Socket created, binding..\n");

        // Prep the sockaddr_in structure
        sserver.sin_family = AF_INET;
        sserver.sin_addr.S_un.S_addr = INADDR_ANY;
        sserver.sin_port = htons(configuration.port);

        // Bind to the server
        if (bind(master_socket, (struct sockaddr*)&sserver, sizeof(sserver)) == SOCKET_ERROR)
        {
            printf("[Spectre -> Error] Failed to bind to specified port, error code : %d\n", WSAGetLastError());
            std::cin.get();
            exit(EXIT_FAILURE);
        }

        printf("[Spectre -> Debug] Successfully binded to port %i.\n", configuration.port);

        // Listen to incoming conns
        listen(master_socket, 3);

        if (configuration.is_protected)
        {
            //char* publicip = Tools::GetPublicIP();

            //if (publicip == (char*)"127.0.0.1")
            //{
            //  printf("[Spectre -> Debug] Server protection failed at the public level..\n");
            //  exit(EXIT_FAILURE);
            //}

            limiter = spectre::util::getlocalip(); // Local network protection, we can use public but it isn't needed lol.
        }

        // Accept incoming connections
        printf("[Spectre -> Debug] Waiting for incoming connections..\n");

        addrlen = sizeof(struct sockaddr_in);

        while (!closed)
        {
            // Clear the socket fd set
            FD_ZERO(&readfds);

            // Add master socket to fd set
            FD_SET(master_socket, &readfds);

            // Add child sockets to fd set
            for (i = 0; i < configuration.max_clients; i++)
            {
                fake_socket = client_socket[i];

                if (fake_socket > 0)
                {
                    FD_SET(fake_socket, &readfds);
                }
            }

            // Wait for an activity on any of the sockets, timeout is NULL, so wait indefinitely
            activity = select(0, &readfds, NULL, NULL, NULL);

            if (activity == SOCKET_ERROR)
            {
                printf("[Spectre -> Error] 'select' call failed, error code : %d\n", WSAGetLastError());
                std::cin.get();
                exit(EXIT_FAILURE);
            }

            // If something happened on the master socket, then its an incoming connection
            if (FD_ISSET(master_socket, &readfds))
            {
                if ((new_socket = accept(master_socket, (struct sockaddr*)&address, (int*)&addrlen)) < 0)
                {
                    printf("[Spectre -> Error] 'accept' call failed, exiting..\n");
                    std::cin.get();
                    exit(EXIT_FAILURE);
                }

                if (inet_ntoa(address.sin_addr) != limiter && configuration.is_protected == true)
                {
                    printf("[Spectre -> Debug] Blocked connection, Socket FD: %i, IP: %s, Port: %i\n", static_cast<int>(new_socket), inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                }
                else
                {
                    printf("[Spectre -> Debug] New connection, Socket FD: %i, IP: %s, Port: %i\n", static_cast<int>(new_socket), inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Add new socket to array of sockets
                    for (i = 0; i < configuration.max_clients; i++)
                    {
                        if (client_socket[i] == 0)
                        {
                            client_socket[i] = new_socket;
                            printf("[Spectre -> Debug] Adding to list of sockets at index %d..\n\n", i);
                            break;
                        }
                    }
                }
            }

            // Else its some IO operation on some other socket
            for (i = 0; i < configuration.max_clients; i++)
            {
                fake_socket = client_socket[i];

                // If client presend in read sockets             
                if (FD_ISSET(fake_socket, &readfds))
                {
                    // Get details of the client
                    getpeername(fake_socket, (struct sockaddr*)&address, (int*)&addrlen);

                    valread = recv(fake_socket, buffer, maxbuffer, 0);

                    setsockopt(fake_socket, SOL_SOCKET, SO_SNDBUF, (char*)&maxbuffer, sizeof(char*));

                    // Check if valread got a socket error, if so disconnect, if not move on to process data
                    if (valread == SOCKET_ERROR)
                    {
                        int error_code = WSAGetLastError();

                        if (error_code == WSAECONNRESET)
                        {
                            printf("[Spectre -> Debug] %s:%d - Disconnected.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                            closesocket(fake_socket); // Disconnect the users
                            client_socket[i] = 0; // Remove the stored socket
                        }
                        else
                        {
                            printf("[Spectre -> Debug] %s:%d - Disconnected.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                            closesocket(fake_socket); // Disconnect the users
                            client_socket[i] = 0; // Remove the stored socket
                        }
                    }
                    else
                    {
                        if (valread == 0)
                        {
                            printf("[Spectre -> Debug] %s:%d - Disconnected.\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                            closesocket(fake_socket); // Disconnect the users
                            client_socket[i] = 0; // Remove the stored socket
                        }
                        else
                        {
                            buffer[valread] = '\0'; // Add empty char, try removing this i dare you <3

                            if (inet_ntoa(address.sin_addr) != limiter && configuration.is_protected == true)
                            {
                                printf("[Spectre -> Debug] %s:%d - Data Blocked - %zu\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), strlen(buffer));
                            }
                            else
                            {
                                if (configuration.is_encrypted)
                                {
                                    printf("[Spectre -> Debug] %s:%d - Data Recieved - %zu (Encrypted)\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), strlen(buffer));
                                }
                                else
                                {
                                    printf("[Spectre -> Debug] %s:%d - Data Recieved - %zu (Unencrypted)\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port), strlen(buffer));
                                }

                                // The reason we have to check if the buffer is maxing out is because
                                // if we do not add it to "script" it will end up executing in chunks
                                // and we do not want that, we want the full script so we add the incoming
                                // buffer to the stored value until it goes below 65000, then we can move
                                // on to handling the action. <3

                                if (strlen(buffer) > 65000) // Reached buffer max, add to script
                                {
                                    if (configuration.is_encrypted)
                                    {
                                        script = script + util::decrypt(std::string(buffer));
                                    }
                                    else
                                    {
                                        script = script + std::string(buffer);

                                        printf("[Spectre -> Debug] Data reached max buffer, adding to current buffer..\n");
                                    }
                                }
                                else // Buffer is finished, move on
                                {
                                    if (configuration.is_encrypted)
                                    {
                                        script = script + util::decrypt(std::string(buffer));
                                    }
                                    else
                                    {
                                        script = script + std::string(buffer); // Add final buffer to stored script value
                                    }

                                    if (strlen(buffer) == 12 && strstr(buffer, "SPECTRE_STOP")) // If requested value if to close the server, handle it, if not move on
                                    {
                                        close(); // Close the server
                                    }
                                    else
                                    {
                                        if (configuration.is_encrypted)
                                        {
                                            printf("[Spectre -> Debug] All data from client recieved, final length was %zu.. (Decrypted)\n", script.length());
                                        }
                                        else
                                        {
                                            printf("[Spectre -> Debug] All data from client recieved, final length was %zu.. (Unencrypted)\n", script.length());
                                        }

                                        action(script); // Perform requested action with the full script value as the parameter

                                        script = ""; // Reset stored script value

                                        printf("\n"); // Cry about it
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        printf("[Spectre -> Debug] Cleaning socket data..\n");

        closesocket(fake_socket); // Close socket
        WSACleanup(); // Clean up WSA

        printf("[Spectre -> Debug] Socket data cleaned.\n");

        printf("[Spectre -> Debug] Server stopped.\n");

        return 0;
    }
}