#include "AudioError.h"
#include <fmod_errors.h>
#include <format>
#include <iostream>

auto AudioSystem::FMOD_ERRCHECK_fn(FMOD_RESULT result, const char* file, int line) -> void
{
    if (result != FMOD_OK)
    {
        auto errMsg{ std::format(
            "{}({}): FMOD error {} - {}",
            file, line, static_cast<int>(result), FMOD_ErrorString(result)
        ) };

        throw std::exception(errMsg.c_str());
    }
}