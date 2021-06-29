
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Spectre.Tools.cpp                                        //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.4                                                 //
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

		/// <summary>
		/// A list of Base64 characters.
		/// </summary>
		static const std::string base64_chars =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		/// <summary>
		/// Check if the data "c" is encrypted.
		/// </summary>
		static inline bool is_encrypted(unsigned char c)
		{
			return (isalnum(c) || (c == '+') || (c == '/'));
		}

		/// <summary>
		/// Decode the Base64 input data.
		/// </summary>
		/// <param name="data">Encoded data.</param>
		/// <returns>Decoded data.</returns>
		std::string decrypt(std::string data)
		{
			int in_len = data.size();
			int i = 0;
			int j = 0;
			int in_ = 0;
			unsigned char char_array_4[4], char_array_3[3];
			std::string ret;

			while (in_len-- && (data[in_] != '=') && is_encrypted(data[in_])) {
				char_array_4[i++] = data[in_]; in_++;
				if (i == 4) {
					for (i = 0; i < 4; i++)
						char_array_4[i] = base64_chars.find(char_array_4[i]);

					char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
					char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
					char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

					for (i = 0; (i < 3); i++)
						ret += char_array_3[i];
					i = 0;
				}
			}

			if (i) {
				for (j = i; j < 4; j++)
					char_array_4[j] = 0;

				for (j = 0; j < 4; j++)
					char_array_4[j] = base64_chars.find(char_array_4[j]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
			}

			return ret;
		}
	}
}