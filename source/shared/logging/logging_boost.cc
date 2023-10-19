// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include "logging.hh"

#include <boost/log/expressions.hpp>
#include <boost/log/support/exception.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions/formatters.hpp>
#include <boost/log/support/date_time.hpp>

#include <boost/log/utility/setup/common_attributes.hpp>


#define RESET           "\033[0m"
#define BLACK           "\033[30m"
#define RED             "\033[31m"
#define GREEN           "\033[32m"
#define YELLOW          "\033[33m"
#define BLUE            "\033[34m"
#define MAGENTA         "\033[35m"
#define CYAN            "\033[36m"
#define WHITE           "\033[37m"
#define BRIGHT_BLACK    "\033[90m"
#define BRIGHT_RED      "\033[91m"
#define BRIGHT_GREEN    "\033[92m"
#define BRIGHT_YELLOW   "\033[93m"
#define BRIGHT_BLUE     "\033[94m"
#define BRIGHT_MAGENTA  "\033[95m"
#define BRIGHT_CYAN     "\033[96m"
#define BRIGHT_WHITE    "\033[97m"

void IRCP::logging::logging::setup_logging()
{
    namespace logging   = boost::log;
    namespace src       = boost::log::sources;
    namespace expr      = boost::log::expressions;
    namespace attrs     = boost::log::attributes;
    namespace keywords  = boost::log::keywords;

    static boost::shared_ptr<boost::log::sinks::sink> sink;
    if(sink)
    {
        logging::core::get()->remove_sink(sink);
    }

    sink = logging::add_console_log(std::clog,
    keywords::format =
    (
        expr::stream

        << RESET
        << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f") << " "
        << "["  << expr::attr<attrs::current_thread_id::value_type>("ThreadID") << "] "

        //~=============================================================================================================
        // Severity
        //~=============================================================================================================
        << expr::if_(logging::trivial::severity < logging::trivial::info)
        [
            expr::stream << "["  << logging::trivial::severity << "] "
        ]
        << expr::if_(logging::trivial::severity == logging::trivial::info)
        [
            expr::stream << (b_enable_ansi_color_terminal ? GREEN : RESET) << "["  << logging::trivial::severity << "] "
        ]
        << expr::if_(logging::trivial::severity == logging::trivial::warning)
        [
            expr::stream << (b_enable_ansi_color_terminal ? YELLOW : RESET) << "["  << logging::trivial::severity << "] "
        ]
        << expr::if_(logging::trivial::severity >= logging::trivial::error)
        [
            expr::stream << (b_enable_ansi_color_terminal ? RED : RESET) << "["  << logging::trivial::severity << "] "
        ]
        << RESET

        #if 0
        << expr::if_(logging::trivial::severity == logging::trivial::info)      [ expr::stream << (b_enable_ansi_color_terminal ? GREEN : RESET)  ]
        << expr::if_(logging::trivial::severity == logging::trivial::warning)   [ expr::stream << (b_enable_ansi_color_terminal ? YELLOW : RESET) ]
        << expr::if_(logging::trivial::severity >= logging::trivial::error)     [ expr::stream << (b_enable_ansi_color_terminal ? RED : RESET)    ]
        #endif

        << std::setw(7) << std::right
        << logging::expressions::smessage

        << RESET
    ),
    keywords::auto_flush = true);

    logging::add_common_attributes();
}

void IRCP::logging::logging::print_test()
{
    std::cout << "printing logging boost test:\n";
    {
        trace("trace", " ", "cool", " ", 12, " ", false, " ", 12.23);
        debug("debug");
        info("info");
        warning("warning");
        error("error");
        fatal("fatal");
    }
    std::cout << "end of printing...\n";
}

void IRCP::logging::logging::enable_ansi_color_terminal(const bool enabled)
{
    b_enable_ansi_color_terminal = enabled;
    setup_logging();

    if(b_enable_ansi_color_terminal)
    {
        trace("logging enable color");
    }
    else
    {
        trace("logging disable color");
    }
}

void IRCP::logging::logging::enable_unicode_emoji_terminal(const bool enabled)
{
    b_enable_unicode_emoji_terminal = enabled;
    //...

    if(b_enable_ansi_color_terminal)
    {
        trace("logging enable unicode emojis");
    }
    else
    {
        trace("logging disable unicode emojis");
    }
}

bool IRCP::logging::logging::is_ansi_color_terminal_enabled()
{
    return b_enable_ansi_color_terminal;
}

bool IRCP::logging::logging::is_unicode_emoji_terminal_enabled()
{
    return b_enable_unicode_emoji_terminal;
}


void IRCP::logging::logging::on_print(const int severity_level, std::string_view message)
{
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get()
    , (::boost::log::keywords::severity = static_cast<boost::log::trivial::severity_level>(severity_level)))
        << message;
}


#undef RESET
#undef BLACK
#undef RED
#undef GREEN
#undef YELLOW
#undef BLUE
#undef MAGENTA
#undef CYAN
#undef WHITE
#undef BRIGHT_BLACK
#undef BRIGHT_RED
#undef BRIGHT_GREEN
#undef BRIGHT_YELLOW
#undef BRIGHT_BLUE
#undef BRIGHT_MAGENTA
#undef BRIGHT_CYAN
#undef BRIGHT_WHITE
