// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include "razer_chroma.hh"

#include <razer/ChromaSDKPluginTypes.h>
#include <razer/ChromaAnimationAPI.h>
#include <razer/RzChromaSDKTypes.h>

#include <objbase.h>
#include <tchar.h>

IRCP::client::razer_chroma::razer_chroma()
{
    using namespace ChromaSDK;

    if (ChromaAnimationAPI::InitAPI() != RZRESULT_SUCCESS)
    {
        throw IRCP::exceptions::razer_chroma_exception("fail to initialize razer chroma api");
    }

    ChromaSDK::APPINFOTYPE appInfo = {};
    _tcscpy_s(appInfo.Title,            256,  TEXT("ircp client"));
    _tcscpy_s(appInfo.Description,      1024, TEXT("ircp client"));
    _tcscpy_s(appInfo.Author.Name,      256,  TEXT("jcastro0x"));
    _tcscpy_s(appInfo.Author.Contact,   256,  TEXT("www.jcastro0x.com"));
    appInfo.SupportedDevice                   = (0x01 | 0x02);      // support: Keyboards and Mice
    appInfo.Category                          = 0x01;               // 0x01 | Utility


    switch (ChromaAnimationAPI::InitSDK(&appInfo))
    {
        case RZRESULT_SUCCESS:
            break;

        case RZRESULT_DLL_NOT_FOUND:
            ChromaAnimationAPI::UninitAPI();
            throw IRCP::exceptions::razer_chroma_exception("dll not found");

        case RZRESULT_DLL_INVALID_SIGNATURE:
            ChromaAnimationAPI::UninitAPI();
            throw IRCP::exceptions::razer_chroma_exception("dll has an invalid signature");

        default:
            ChromaAnimationAPI::UninitAPI();
            throw IRCP::exceptions::razer_chroma_exception("failed to initialize razer chroma");
    }

    IRCP::logging::logging::trace("razer_chroma initialized");
}

IRCP::client::razer_chroma::~razer_chroma()
{
    using namespace ChromaSDK;

    if (ChromaAnimationAPI::GetIsInitializedAPI())
    {
        if (ChromaAnimationAPI::IsInitialized())
        {
            ChromaAnimationAPI::StopAll();
            ChromaAnimationAPI::CloseAll();
            ChromaAnimationAPI::Uninit();
            ChromaAnimationAPI::UninitAPI();

            IRCP::logging::logging::trace("razer_chroma uninitialized");
        }
    }
}

void IRCP::client::razer_chroma::highlight_keys(const std::vector<int>& keys, rgb_t highlight_rgb, rgb_t others_rgb)
{
    using namespace ChromaSDK;

    clear();

    ChromaAnimationAPI::MakeBlankFramesRGBName(
        blank_keyboard,
        120,
        0.1f,
        std::get<0>(others_rgb),    // r
        std::get<1>(others_rgb),    // g
        std::get<2>(others_rgb)     // b
    );

    ChromaAnimationAPI::SetKeysColorAllFramesRGBName(
        blank_keyboard,
        keys.data(),
        static_cast<int>(keys.size()),
        std::get<0>(highlight_rgb), // r
        std::get<1>(highlight_rgb), // g
        std::get<2>(highlight_rgb)  // b
    );

    ChromaAnimationAPI::PlayAnimationName(blank_keyboard, true);
}

void IRCP::client::razer_chroma::clear()
{
    using namespace ChromaSDK;
    ChromaAnimationAPI::CloseAnimationName(blank_keyboard);
}
