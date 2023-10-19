// This file is part of the IRCP Project. See LICENSE file for Copyright information

#include <shared/pch.hh>


namespace IRCP::utilities
{
    /**
     * RAII version to hook and unhook program's signals
     **/
    class hook_signals final
    {
    public:
        explicit hook_signals(std::function<void()> callback)
        {
            hook_signals::callback_ = std::move(callback);

            signal(SIGINT, on_signal);
            signal(SIGTERM, on_signal);

            #ifdef _WIN32
            signal(SIGBREAK, on_signal);
            #endif

            IRCP::logging::logging::trace("hook signals");
        }
        ~hook_signals()
        {
            signal(SIGINT, nullptr);
            signal(SIGTERM, nullptr);

            #ifdef _WIN32
            signal(SIGBREAK, nullptr);
            #endif

            hook_signals::callback_ = {};
            IRCP::logging::logging::trace("unhook signals");
        }

    private:
        static void on_signal(int s)
        {
            switch (s)
            {
                case SIGINT:
                case SIGTERM:
                    hook_signals::callback_();
                    break;

                #ifdef _WIN32
                case SIGBREAK:
                    hook_signals::callback_();
                    break;
                #endif
            }

            signal(s, on_signal);
        }

        inline static std::function<void()> callback_;
    };
};


