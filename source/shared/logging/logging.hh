// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

namespace IRCP::logging
{
    /**
     * The class that provides all logging related methods.
     */
    class logging
    {
        // Represents the different levels of logging severity.
        enum class severity_level : int
        {
            trace,
            debug,
            info,
            warning,
            error,
            fatal
        };

    public:
        /** Sets up the logger. */
        static void setup_logging();

        /** Outputs a sample log message */
        static void print_test();

        /** Toggles ANSI color for the terminal on/off. */
        static void enable_ansi_color_terminal(bool enabled);

        /** Toggles Unicode emoji for the terminal on/off. */
        static void enable_unicode_emoji_terminal(bool enabled);

        /** Determines if ANSI color terminal is switched on */
        static bool is_ansi_color_terminal_enabled();

        /** Determines if Unicode emoji terminal is switched on */
        static bool is_unicode_emoji_terminal_enabled();


    public:
        template<typename... Args>
        static void trace(Args... args)
        {
            on_print(static_cast<int>(logging::severity_level::trace), (std::format("{}", args) + ...));
        }

        template<typename... Args>
        static void debug(Args... args)
        {
            on_print(static_cast<int>(logging::severity_level::debug), (std::format("{}", args) + ...));
        }

        template<typename... Args>
        static void info(Args... args)
        {
            on_print(static_cast<int>(logging::severity_level::info), (std::format("{}", args) + ...));
        }

        template<typename... Args>
        static void warning(Args... args)
        {
            on_print(static_cast<int>(logging::severity_level::warning), (std::format("{}", args) + ...));
        }

        template<typename... Args>
        static void error(Args... args)
        {
            on_print(static_cast<int>(logging::severity_level::error), (std::format("{}", args) + ...));
        }

        template<typename... Args>
        static void fatal(Args... args)
        {
            on_print(static_cast<int>(logging::severity_level::fatal), (std::format("{}", args) + ...));
        }

    private:
        static void on_print(int severity_level, std::string_view message);

    private:
        inline static bool b_enable_ansi_color_terminal {};
        inline static bool b_enable_unicode_emoji_terminal {};
    };
}
