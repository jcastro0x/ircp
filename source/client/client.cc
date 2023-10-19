// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include <boost/program_options.hpp>

#include <razer/razer_chroma.hh>
#include <razer/RzChromaSDKTypes.h>

#include <shared/logo.hh>
#include <shared/pch.hh>
#include <shared/utilities/hook_signals.hh>


int main(const int /*argc*/, const char** /*argv*/)
{
    using namespace IRCP::logging;

    logging::setup_logging();
    logging::enable_ansi_color_terminal(true);
    logging::enable_unicode_emoji_terminal(true);

    // Print program title
    PRINT_LOGO;

    logging::info("client launched");
    try
    {
        bool running = true;
        IRCP::utilities::hook_signals hook_signals([&]{ running = false; });

        #ifdef _WIN32
        IRCP::client::razer_chroma razer_chroma;
        razer_chroma.highlight_keys(
        {
            ChromaSDK::Keyboard::RZKEY::RZKEY_1,
            ChromaSDK::Keyboard::RZKEY::RZKEY_2,
            ChromaSDK::Keyboard::RZKEY::RZKEY_3,
            ChromaSDK::Keyboard::RZKEY::RZKEY_4,
        }, {255, 255, 0} ,  {64, 64, 64});
        #endif

        while (running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch(const std::exception& e)
    {
        logging::fatal(e.what());
        return EXIT_FAILURE;
    }

    logging::info("Bye bye...");
    return EXIT_SUCCESS;
}
