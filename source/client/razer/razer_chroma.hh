// This file is part of the IRCP Project. See LICENSE file for Copyright information

#pragma once

#include <shared/pch.hh>

#ifdef _WIN32

namespace IRCP::exceptions
{
    class razer_chroma_exception final : public std::runtime_error
    {
        public: explicit razer_chroma_exception(std::string_view message)
        : std::runtime_error(std::format("[razer_chroma_exception] {}", message)) {}
    };
}

namespace IRCP::client
{
    /**
     * Wrapper to use Razer Chroma SDK.
     *
     * @warning Currently only works with keyboard
     * @see https://github.com/RazerOfficial/CChromaEditor
     */
    class razer_chroma final
    {
    public:
        razer_chroma();
        ~razer_chroma();

        /**
         * Highlight `keys` keys to highlight_rgb and all other left to others_rgb.
         *
         * @note Keys are found at ChromaSDK::Keyboard::RZKEY
         * @note Calculate `highlight_rgb` and `others_rgb` with ChromaSDK::ChromaAnimationAPI::GetRGB
         */

        using rgb_t = std::tuple<int,int,int>;
        void highlight_keys(const std::vector<int>& keys, rgb_t highlight_rgb, rgb_t others_rgb);

        /**
         * Clear state and restore razer chroma effects.
         */
        void clear();

    private:
        inline static const char* blank_chromaLink = "razer_chroma/Blank_ChromaLink.chroma";
        inline static const char* blank_headset    = "razer_chroma/Blank_Headset.chroma";
        inline static const char* blank_keyboard   = "razer_chroma/Blank_Keyboard.chroma";
        inline static const char* blank_keypad     = "razer_chroma/Blank_Keypad.chroma";
        inline static const char* blank_mouse      = "razer_chroma/Blank_Mouse.chroma";
        inline static const char* blank_mousepad   = "razer_chroma/Blank_Mousepad.chroma";
    };
}

#endif
