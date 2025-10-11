#pragma once

#include "Global.h"

/// @brief
/// @param argc
/// @param argv
void App_Setup(int argc, char **argv);

/// @brief
/// @param deltaTime
void App_Loop(float deltaTime);

/// @brief
/// @param exitCode
/// @param exitMessage
void App_Terminate(int exitCode, char *exitMessage);
