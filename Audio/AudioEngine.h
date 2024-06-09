///
/// \file AudioEngine.h
/// 
/// \brief The AudioEngine class.
///

#pragma once

#include <fmod.hpp>

namespace AudioSystem
{
    /// \brief AudioEngine class
    ///
    /// AudioEngine only use the simplest FMOD Core. Just for learning FMOD at present.\n
    /// It should only have single instance in entire project.\n
    /// The class is used to manage audio system resources and create sounds.\n
    class AudioEngine
    {
        friend class Audio;

    public:

        /// \brief Constructor of AudioEngine.
        ///
        /// It may be throw exception when call FMOD::System_Create and FMOD::System::setCallback.\n
        /// The callback of system (which is FMOD::System) is only handle the return error for FMOD api.\n
        /// The constructor also initialize the system by default arguments as follow:\n
        /// \code
        ///     // Default initialize configuration.
        ///     m_pSystem->init(512, FMOD_INIT_NORMAL, 0);
        /// \endcode
        /// The detail information please jump to https://www.fmod.com/docs/2.03/api/core-api-system.html#system_init
        /// 
        /// Note:\n
        ///     The init also maybe throw exception because setCallback which use to handle error return value of FMOD api.\n
        ///     But currently the exception handling is already performed inside the callback.\n
        ///     So not need to special handle, only need to check the error message in callback when encounter some FMOD api call error.
        AudioEngine();

        /// \brief Destructor of AudioEngine.
        ///
        /// Only close system then release system.
        ~AudioEngine();

        AudioEngine(const AudioEngine&) = delete;
        AudioEngine(AudioEngine&&)      = delete;

        auto operator=(const AudioEngine&) -> AudioEngine& = delete;
        auto operator=(AudioEngine&&)      -> AudioEngine& = delete;

        /// \brief Update the FMOD::System.
        ///
        /// This should be called once in per frame.
        auto update() const -> void;

    private:
        FMOD::System* m_pSystem{};
    };
}