#include "Audio.h"
#include "AudioError.h"
#include <Windows.h>
#include <unordered_set>

using namespace AudioSystem;

static const std::unordered_set<std::string_view> Supported_Audio_Types
{
    "wav", "mp3", "flac", "ogg", "aac"
};

bool AudioSystem::isSupportedType(const std::filesystem::path& path)
{
    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path))
    {
        auto extensionName{ path.extension().string().substr(1) };
        std::transform(std::begin(extensionName), std::end(extensionName), std::begin(extensionName), tolower);

        if (Supported_Audio_Types.find(extensionName) != Supported_Audio_Types.end())
        {
            return true;
        }
    }

    return false;
}

std::string AudioSystem::wstrToStr(const std::wstring& wstr)
{
    int size{ WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL) };
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size, NULL, NULL);
    return str;
}

namespace AudioSystem
{
    FMOD_RESULT F_CALL FMOD_ChannelControl_Callback(
        FMOD_CHANNELCONTROL* channelcontrol,
        FMOD_CHANNELCONTROL_TYPE controltype,
        FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
        void* commanddata1,
        void* commanddata2)
    {
        if (controltype == FMOD_CHANNELCONTROL_CALLBACK_END)
        {
            Audio* pAudio{};
            FMOD_ERRCHECK(std::bit_cast<FMOD::Channel*>(channelcontrol)->getUserData(std::bit_cast<void**>(&pAudio)));
            if (pAudio != nullptr)
            {
                pAudio->m_end = true;
            }
        }
        return FMOD_OK;
    }
}

Audio::~Audio()
{
    std::lock_guard lock{ m_mutex };
    FMOD_ERRCHECK(m_pSound->release());
}

auto Audio::load(const std::wstring& file) -> void
{
    std::lock_guard lock{ m_mutex };
    if (!m_end)
    {
        FMOD_ERRCHECK(m_pSound->release());
    }

    //static FMOD_MODE mode{ 
    //    FMOD_CREATESTREAM | FMOD_LOOP_NORMAL | FMOD_OPENONLY | 
    //    FMOD_ACCURATETIME | FMOD_UNIQUE      | FMOD_LOWMEM 
    //};
    FMOD_MODE mode{ FMOD_CREATESTREAM | FMOD_LOWMEM | FMOD_ACCURATETIME | FMOD_UNIQUE };
    FMOD_ERRCHECK(m_engine.m_pSystem->createSound(wstrToStr(file).c_str(), mode, nullptr, &m_pSound));
    FMOD_ERRCHECK(m_engine.m_pSystem->playSound(m_pSound, nullptr, true, &m_pChannel));
    FMOD_ERRCHECK(m_pChannel->setUserData(this));
    FMOD_ERRCHECK(m_pChannel->setCallback(FMOD_ChannelControl_Callback));

    m_fileName = file;
    {
        unsigned length{};
        FMOD_ERRCHECK(m_pSound->getLength(&length, FMOD_TIMEUNIT_MS));
        m_duration = static_cast<double>(length) / 1000;
    }
    m_end    = false;
    m_paused = true;
    FMOD_ERRCHECK(m_pChannel->setVolume(static_cast<float>(m_volume) / 100));
    if (m_loop)
    {
        FMOD_ERRCHECK(m_pChannel->setMode(FMOD_LOOP_NORMAL));
    }
}

auto Audio::play()  -> void
{
    if (!m_end)
    {
        m_paused = false;
        std::lock_guard lock{ m_mutex };
        FMOD_ERRCHECK(m_pChannel->setPaused(false));
    }
}

auto Audio::pause() -> void
{
    m_paused = true;
    if (!m_end)
    {
        std::lock_guard lock{ m_mutex };
        FMOD_ERRCHECK(m_pChannel->setPaused(true));
    }
}

auto Audio::setVolume(int volume) -> void
{
    m_volume = std::clamp(volume, 0, 100);
    if (!m_end)
    {
        std::lock_guard lock{ m_mutex };
        FMOD_ERRCHECK(m_pChannel->setVolume(static_cast<float>(m_volume) / 100));
    }
}

auto Audio::setTime(double time) -> void
{
    if (!m_end)
    {
        std::lock_guard lock{ m_mutex };
        time = std::clamp(time, 0.0, m_duration);
        if (time < m_duration)
        {
            FMOD_ERRCHECK(m_pChannel->setPosition(static_cast<unsigned>(time * 1000), FMOD_TIMEUNIT_MS));
        }
        else
        {
            unsigned endPos{};
            m_pSound->getLength(&endPos, FMOD_TIMEUNIT_PCM);
            FMOD_ERRCHECK(m_pChannel->setPosition(endPos - 1, FMOD_TIMEUNIT_PCM));
            return;
        }
    }
}

auto Audio::setLoop(bool loop) -> void
{
    m_loop = loop;
    if (!m_end)
    {
        std::lock_guard lock{ m_mutex };
        FMOD_MODE mode{};
        if (m_loop)
        {
            mode = FMOD_LOOP_NORMAL;
        }
        else
        {
            mode = FMOD_LOOP_OFF;
        }
        FMOD_ERRCHECK(m_pChannel->setMode(mode));
    }
}

auto Audio::getTime() -> double
{
    if (!m_end)
    {
        std::lock_guard lock{ m_mutex };
        unsigned length{};
        FMOD_ERRCHECK(m_pChannel->getPosition(&length, FMOD_TIMEUNIT_MS));
        double time{ static_cast<double>(length) / 1000 };
        if (time <= m_duration)
        {
            return time;
        }
    }
    return m_duration;
}