// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include "shared/pch.hh"

namespace IRCP::exceptions
{
    class listener_unhandled final : public std::runtime_error
    {
        public: explicit listener_unhandled(std::string_view message, int32_t value)
        : std::runtime_error(std::format("({}) listener error_code unhandled.\n{}", value, message)) {}
    };

    class socket_unhandled final : public std::runtime_error
    {
        public: explicit socket_unhandled(std::string_view message, int32_t value)
        : std::runtime_error(std::format("({}) socket error_code unhandled.\n{}", value, message)) {}
    };

    class config_exception final : public std::runtime_error
    {
        public: explicit config_exception(std::string_view message)
        : std::runtime_error(std::format("[config_exception] {}", message)) {}
    };
}