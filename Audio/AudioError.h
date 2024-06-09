/// 
/// \file AudioError.h
/// 
/// \brief The error handling for FMOD.
/// 

#pragma once

#include <fmod_common.h>

namespace AudioSystem
{
    /// \brief A internal tool function, only use to check error for FMOD api.
    ///
    /// If any error value is returned, throw the exception.
    auto FMOD_ERRCHECK_fn(FMOD_RESULT result, const char* file, int line) -> void;

    /// \brief A wrapper function is used to call FMOD_ERRCHECK_fn.
    #define FMOD_ERRCHECK(result) FMOD_ERRCHECK_fn(result, __FILE__, __LINE__)
}