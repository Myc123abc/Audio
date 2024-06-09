///
/// \file Audio.h
/// 
/// \brief The base functional of Audio.
/// 

#pragma once

#include "AudioEngine.h"
#include <fmod.hpp>
#include <filesystem>
#include <mutex>
#include <atomic>

namespace AudioSystem
{
    /// \brief The Supported types of audio.
    ///
    /// Supported types:\n
    /// wav mp3 flac ogg acc
    /// 
    /// \param[in] path Path of file.
    /// 
    /// \return True is supported.
    auto isSupportedType(const std::filesystem::path& path) -> bool;

    /// \brief Convert wstring to string.
    ///
    /// \param[in] wstr
    /// 
    /// \return str
    auto wstrToStr(const std::wstring& wstr) -> std::string;

    /// \brief Audio class
    ///
    /// Note: Use some functions like isLoop which call the FMOD::Channel.\n
    /// So you must ensure whether the Audio objace is playing, which is not end.\n
    /// Using Audio::isEnd to judge.\n
    /// So the audio is not thread safe.
    class Audio
    {
        friend auto F_CALL FMOD_ChannelControl_Callback(
            FMOD_CHANNELCONTROL* channelcontrol,
            FMOD_CHANNELCONTROL_TYPE controltype,
            FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
            void* commanddata1,
            void* commanddata2
        ) -> FMOD_RESULT;

    public:

        /// \brief Constructor of Audio.
        ///
        /// Only bind the specific AudioEngine to Audio.
        Audio(const AudioEngine& engine) : m_engine(engine) {}

        /// \brief Destructor of Audio.
        ///
        /// Release FMOD::Sound.
        ~Audio();

        Audio(const Audio&) = delete;
        Audio(Audio&&)      = delete;

        auto operator=(const Audio&) -> Audio& = delete;
        auto operator=(Audio&&)      -> Audio& = delete;

        /// \brief Load the audio file.
        ///
        /// \param[in] file The absolute path of audio file.
        auto load(const std::wstring& file) -> void;

        /// \brief Is the audio playing end.
        ///
        /// \return End is true.
        auto isEnd() const noexcept -> bool { return m_end; }

        /// \brief Play the audio.
        auto play()  -> void;

        /// \brief Pause the audio.
        auto pause() -> void;

        /// \brief Set volume of the audio.
        ///
        /// \param[in] volume Max 100 Min 0.\n
        /// If volume value is not between Max and Min.\n
        /// It will be converted to the closest one.
        auto setVolume(int volume) -> void;

        /// \brief Set time of the audio.
        ///
        /// \param[in] time In second.\n
        /// If time is not beween duration and 0.\n
        /// It will be converted to the closest one.
        auto setTime(double time) -> void;

        /// \brief Set loop for audio.\n
        /// The loop is unlimited.
        /// \param[in] loop true is set, false reverse.
        auto setLoop(bool loop) -> void;

        /// \brief Get currently playing time.
        ///
        /// \return Current time.
        auto getTime() -> double;

        /// \brief Get absolute path of audio.
        ///
        /// \return Path of audio.
        auto getFileName() const noexcept -> const wchar_t* { return m_fileName.c_str(); }

        /// \brief Get duration of audio.
        ///
        /// \return Duration of audio.
        auto getDuration() const noexcept -> double { return m_duration; }

        /// \brief Get volume of audio.
        ///
        /// \return Volume of audio.
        auto getVolume() const noexcept -> int { return m_volume; }

        /// \brief Is audio paused.
        ///
        /// \return Paused is true.
        auto isPaused() const noexcept -> bool { return m_paused; }

        /// \brief Is audio loop.
        ///
        /// \return Loop is true.
        auto isLoop() const noexcept -> bool { return m_loop; }

    private:
        const AudioEngine&   m_engine  {};
        FMOD::Sound*         m_pSound  {};
        FMOD::Channel*       m_pChannel{};

        std::wstring         m_fileName{};
        double               m_duration{};
        std::atomic<bool>    m_end{ true };
        int                  m_volume{ 100 };
        bool                 m_paused{ true };
        bool                 m_loop{ false };

        std::mutex           m_mutex;
    };
}