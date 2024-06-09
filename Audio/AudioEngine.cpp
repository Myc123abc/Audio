#include "AudioEngine.h"
#include "AudioError.h"
#include <bit>
#include <fmod_errors.h>
#include <format>
#include <iostream>
#include <string_view>

using namespace AudioSystem;

AudioEngine::AudioEngine()
{
    FMOD_ERRCHECK(FMOD::System_Create(&m_pSystem));

    // Default initialize configuration.
    FMOD_ERRCHECK(m_pSystem->init(512, FMOD_INIT_NORMAL, nullptr));
}

AudioEngine::~AudioEngine()
{
    FMOD_ERRCHECK(m_pSystem->close());
    FMOD_ERRCHECK(m_pSystem->release());
}

auto AudioEngine::update() const -> void
{
    FMOD_ERRCHECK(m_pSystem->update());
}