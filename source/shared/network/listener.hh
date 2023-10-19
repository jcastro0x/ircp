// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include <shared/pch.hh>

#include <shared/utilities/constructor_policies.hh>
#include <shared/network/socket.hh>
#include <shared/utilities/exceptions.hh>


namespace IRCP::network
{
    template<typename T>
    concept is_socket = std::is_base_of<IRCP::network::socket, T>::value;

    template<is_socket socket_t>
    class listener final : private immovable
    {
        using port_type_t = boost::asio::ip::port_type;

    public:
        explicit listener(std::string_view address, port_type_t port);
        ~listener();

        [[nodiscard]] std::optional<std::exception_ptr> get_current_exception() const noexcept
        {
            std::scoped_lock lock(unhandled_exception_mutex);

            if(exception_ptr) return exception_ptr;
            return {};
        }

    private:
        void begin_accept();
        void on_accepted(socket_t* IRCP_socket);

    private:
        boost::asio::io_context context_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::unordered_set<std::unique_ptr<socket_t>> sockets_;

        mutable std::mutex unhandled_exception_mutex;
        std::thread thread_;
        std::exception_ptr exception_ptr;
    };
}


template<IRCP::network::is_socket socket_t>
IRCP::network::listener<socket_t>::listener(std::string_view address, port_type_t port)
    : context_()
    , acceptor_(context_, {boost::asio::ip::make_address(address.data()), port})
{
    const auto endpoint = acceptor_.local_endpoint();
    IRCP::logging::logging::info(std::format("listening at {}:{}", endpoint.address().to_string(), endpoint.port()));

    begin_accept();
    thread_ = std::thread([this]() { context_.run(); });
}

template<IRCP::network::is_socket socket_t>
IRCP::network::listener<socket_t>::~listener()
{
    IRCP::logging::logging::trace("IRCP::network::listener<socket_t>::~listener()");
    boost::asio::post(context_, [&](){
        acceptor_.close();
    });

    context_.stop();
    thread_.join(); //todo: handle exception if not joinable
}

template<IRCP::network::is_socket socket_t>
void IRCP::network::listener<socket_t>::begin_accept()
{
    auto[socket, success] = sockets_.emplace(std::make_unique<socket_t>(context_));
    assert(success);

    socket_t* IRCP_socket = (*socket).get();
    auto& asio_socket = static_cast<boost::asio::ip::tcp::socket&>(*IRCP_socket);

    using error_code = const boost::system::error_code;
    acceptor_.async_accept(asio_socket, [&, IRCP_socket](const error_code& ec)
    {
        if(ec)
        {
            using enum boost::asio::error::basic_errors;
            switch (ec.value())
            {
                case operation_aborted:
                    IRCP::logging::logging::trace("listener::begin_accept operation_aborted");
                    break;

                case connection_reset:
                    IRCP::logging::logging::trace("listener::begin_accept connection_reset");
                    break;

                default:
                    try { throw IRCP::exceptions::listener_unhandled(ec.message(), ec.value()); }
                    catch (...)
                    {
                        IRCP::logging::logging::fatal("cache listener_unhandled_exception");
                        std::scoped_lock lock(unhandled_exception_mutex);
                        exception_ptr = std::current_exception();
                    }
            }

            return;
        }

        on_accepted(IRCP_socket);
    });
}

template<IRCP::network::is_socket socket_t>
void IRCP::network::listener<socket_t>::on_accepted(socket_t* IRCP_socket)
{
    assert(IRCP_socket);

    auto& asio_socket = static_cast<boost::asio::ip::tcp::socket&>(*IRCP_socket);
    const auto endpoint = asio_socket.remote_endpoint();
    IRCP::logging::logging::trace(std::format("new client from {}:{}", endpoint.address().to_string(), endpoint.port()));

    IRCP_socket->open();
    begin_accept();
}
