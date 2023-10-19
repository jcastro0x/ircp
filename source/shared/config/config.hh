// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include <shared/pch.hh>
#include <boost/property_tree/ptree_fwd.hpp>

namespace IRCP::config
{
    /**
     * Read *.conf files from filesystem and store a property tree pointer to the data.
     * Through boost::property_tree, this class store in memory the values. Use RAII idiom.
     */
    class config
    {
    public:
        explicit config(std::string_view name);
        ~config();

    public:
        std::string get_string(std::string_view name, const std::string& default_val = "") const;
        bool get_bool(std::string_view name, bool default_val = false) const;

        class number_word {};
        class number_int  {};

        size_t get_number(std::string_view name, number_word, size_t default_val = 0) const;
        int32_t get_number(std::string_view name, number_int, int32_t default_val = 0) const;

    private:
        std::unique_ptr<boost::property_tree::ptree> ptree_;
    };
}
