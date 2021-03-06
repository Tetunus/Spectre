
/////////////////////////////////////////////////////////////////////
//                                                                 //
//  File: Spectre.Tools.h                                          //
//  Author: Tetunus (Josh)                                         //
//  Version: 0.0.4                                                 //
//  Description: Additional tools that Spectre uses for various    //
//               functions and security features.                  //
//                                                                 //
/////////////////////////////////////////////////////////////////////

#pragma once

namespace spectre
{
    namespace util
    {
        char* getlocalip();
        char* getpublicip();

        std::string decrypt(std::string data);
    }
}