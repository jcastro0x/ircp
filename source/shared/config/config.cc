// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include "config.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

IRCP::config::config::config(std::string_view name)
{
    ptree_ = std::make_unique<boost::property_tree::ptree>();

    try
    {
        boost::property_tree::ini_parser::read_ini(std::format("{}.conf", name.data()), *ptree_);
    }
    catch(const boost::property_tree::ini_parser_error &e)
    {
        throw IRCP::exceptions::config_exception(std::format("({}) {}", name.data(), e.what()));
    }
}

IRCP::config::config::~config() = default;


std::string IRCP::config::config::get_string(std::string_view name, const std::string& default_val) const
{
    try
    {
        return ptree_->get<std::string>(name.data());
    }
    catch(const boost::property_tree::ptree_bad_path &e)
    {
        return default_val;
    }
    catch(const boost::property_tree::ptree_bad_data &e)
    {
        throw IRCP::exceptions::config_exception(std::format("({}) {}", name.data(), e.what()));
    }
}

bool IRCP::config::config::get_bool(std::string_view name, bool default_val) const
{
    try
    {
        return ptree_->get<bool>(name.data());
    }
    catch(const boost::property_tree::ptree_bad_path &e)
    {
        return default_val;
    }
    catch(const boost::property_tree::ptree_bad_data &e)
    {
        throw IRCP::exceptions::config_exception(std::format("({}) {}", name.data(), e.what()));
    }
}

template<typename number_t>
static number_t get_number_template(boost::property_tree::ptree* ptree, std::string_view name, number_t default_val)
{
    try
    {
        return ptree->get<number_t>(name.data());
    }
    catch(const boost::property_tree::ptree_bad_path &e)
    {
        return default_val;
    }
    catch(const boost::property_tree::ptree_bad_data &e)
    {
        throw IRCP::exceptions::config_exception(std::format("({}) {}", name.data(), e.what()));
    }
}

size_t IRCP::config::config::get_number(std::string_view name, IRCP::config::config::number_word, size_t default_val) const
{
    return get_number_template<size_t>(ptree_.get(), name, default_val);
}

int32_t IRCP::config::config::get_number(std::string_view name, IRCP::config::config::number_int, int32_t default_val) const
{
    return get_number_template<int32_t>(ptree_.get(), name, default_val);
}
