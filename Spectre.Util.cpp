
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Spectre.Tools.cpp                                        //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.3                                                 //
//  Description: Additional tools that Spectre uses for various    //
//               functions and security features.                  //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#include <string>
#include <iostream>
#include <Windows.h>
#include <wininet.h>

#pragma comment(lib,"wininet.lib")

#include "Spectre.Util.h"

namespace spectre
{
	namespace util
	{
		/// <summary>
		/// Get the networks local IPv4 address via Winsock.
		/// </summary>
		/// <returns>Provate IPv4 Address</returns>
		char* getlocalip()
		{
			WSADATA wsa_data;

			if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
			{
				printf("[Spectre -> Error] Failed to initalize, error code : %d\n", WSAGetLastError());
				exit(EXIT_FAILURE);
			}

			char* localip;
			char hostname[255];
			struct hostent* host_entry;

			gethostname(hostname, 255);

			host_entry = gethostbyname(hostname);
			localip = inet_ntoa(*(struct in_addr*)*host_entry->h_addr_list);

			WSACleanup();

			printf("[Spectre -> Debug] Server protection enabled, all connections limited to %s\n", localip);

			return localip;
		}

		/// <summary>
		/// Get the networks public IPv4 address via ipify.
		/// </summary>
		/// <returns>Public IPv4 Address</returns>
		char* getpublicip()
		{
			HINTERNET connection = InternetOpen(L"Spectre", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

			if (!connection)
			{
				printf("[Spectre -> Debug] Failed to create a connection to the provided server.\n");

				return (char*)"127.0.0.1";
			}

			HINTERNET openaddr = InternetOpenUrl(connection, L"http://api.ipify.org/", NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_KEEP_CONNECTION, 0);

			if (!openaddr)
			{
				printf("[Spectre -> Debug] Failed to open the provided URL, error code : %d\n", GetLastError());

				InternetCloseHandle(connection);

				return (char*)"127.0.0.1";
			}

			char publicip[1024];
			DWORD buffer = 0;

			while (InternetReadFile(openaddr, publicip, 1024, &buffer) && buffer)
			{
				publicip[buffer] = '\0';

				printf("[Spectre -> Debug] Server protection enabled, all connections limited to %s\n", publicip);
			}

			InternetCloseHandle(openaddr);
			InternetCloseHandle(connect);

			return publicip;
		}
	}
}