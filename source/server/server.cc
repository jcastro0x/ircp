// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include <boost/program_options.hpp>

#include <shared/logo.hh>

#include <shared/network/listener.hh>
#include <shared/network/package.hh>
#include <shared/network/socket.hh>

#include <shared/config/config.hh>
#include <shared/logging/logging.hh>

#include <shared/utilities/hook_signals.hh>


class dummy_socket : public IRCP::network::socket
{
public:
    explicit dummy_socket(boost::asio::io_context& context) : socket(context) { }
    void process_incoming_package(PACKAGE&& /*buffer*/) override
    {
    }
};

class dummy_config : public IRCP::config::config
{
public:
    explicit dummy_config() : config("ircp") { }

    using port_type_t = boost::asio::ip::port_type;
    std::pair<std::string, port_type_t> get_endpoint() const
    {
        try
        {
            return std::make_pair(get_string("bind.ip", "0.0.0.0"), static_cast<port_type_t>(get_number("bind.port", number_word{}, 8080)));
        }
        catch(IRCP::exceptions::config_exception& e)
        {
            IRCP::logging::logging::fatal("Check if ircp.conf file are correctly writen");
            throw e;
        }
    }
};

int main(const int /*argc*/, const char** /*argv*/)
{
    // Read configuration
    dummy_config conf;

    // Setup logging
    IRCP::logging::logging::setup_logging();
    IRCP::logging::logging::enable_ansi_color_terminal(conf.get_bool("console.use_color"));
    IRCP::logging::logging::enable_unicode_emoji_terminal(conf.get_bool("console.use_emoji"));

    // Print program title
    PRINT_LOGO;

    std::cout << "1) ✊\n";
    std::cout << "2) ✋\n";
    std::cout << "3) ✌️\n";

    IRCP::logging::logging::trace("✌️");
    IRCP::logging::logging::trace("😜");

    try
    {
        bool running = true;
        IRCP::utilities::hook_signals hook_signals([&]{ running = false; });

        IRCP::network::listener<dummy_socket> listener(conf.get_endpoint().first, conf.get_endpoint().second);

        while (running)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if(auto listener_exception = listener.get_current_exception())
            {
                std::rethrow_exception(listener_exception.value());
            }
        }
    }
    catch(const std::exception& e)
    {
        IRCP::logging::logging::fatal(e.what());
        return EXIT_FAILURE;
    }

    IRCP::logging::logging::info("Bye bye...");
    return EXIT_SUCCESS;
}
