// This file is part of the IRCP Project. See LICENSE file for Copyright information

        /********************************************************************
         *                                                                  *
         * This unit file alters the default behavior of ChromaLogger       *
         * to utilize features from IRCP such as IRCP::logging.             *
         * It's a non-intrusive modification, so the Razer Chroma SDK can   *
         * continue to be updated and use this modified logger.             *
         *                                                                  *
         ********************************************************************/

#include "ChromaLogger.h"
#include <stdarg.h>

#include <shared/pch.hh>
#include <boost/locale.hpp>

using namespace ChromaSDK;
using namespace IRCP::logging;

// @todo Is there a better way to initialize logging than this?
static void setup_logging()
{
    static bool _bStarted = false;
    if(!_bStarted)
    {
        _bStarted = true;
        logging::setup_logging();
    }
}

[[deprecated("This function is deprecated and should not be used.")]]
void ChromaLogger::printf(const char* /*format*/, ...) { }

[[deprecated("This function is deprecated and should not be used.")]]
void ChromaLogger::wprintf(const wchar_t* /*format*/, ...) { }

void ChromaLogger::fprintf(FILE* /*stream*/, const char* format, ...)
{
#if _DEBUG
    setup_logging();

    va_list args;
    va_start(args, format);

    std::string fmt(format);
    std::string buffer(256, L'\0');
    std::vsprintf(&buffer[0], fmt.c_str(), args);

    va_end(args);

    // Remove the '/r''/n' or '/n' tail
    size_t len = ::strlen(&buffer[0]);
    if(len >= 2 && buffer[len-2] == '\r' && buffer[len-1] == '\n')
    {
        buffer[len-2] = '\0';
    }
    else if(len >= 2 && buffer[len-1] == '\n')
    {
        buffer[len-1] = '\0';
    }

    logging::trace(buffer);
#endif
}



void ChromaLogger::fwprintf(FILE* /*stream*/, const wchar_t* format, ...)
{
#if _DEBUG
    setup_logging();

    va_list args;
    va_start(args, format);

    std::wstring fmt(format);
    std::wstring buffer(256, L'\0');

    std::vswprintf(&buffer[0], buffer.size(), fmt.c_str(), args);
    va_end(args);

    // Remove the '/r''/n' or '/n' tail
    size_t len = ::wcslen(&buffer[0]);
    if(len >= 2 && buffer[len-2] == '\r' && buffer[len-1] == '\n')
    {
        buffer[len-1] = L'\0';
        buffer[len-2] = L'\0';
    }
    else if(len >= 2 && buffer[len-1] == '\n')
    {
        buffer[len-1] = L'\0';
    }

    try
    {
        const auto string = boost::locale::conv::utf_to_utf<char>(buffer);
        logging::trace(string);
    }
    catch(boost::locale::conv::conversion_error&)
    {
        logging::error("ChromaLogger fail to parse wchar_t to char formatted message. Using vfwprintf:");
        ::vfwprintf(stdout, format, args);
    }

#endif
}
