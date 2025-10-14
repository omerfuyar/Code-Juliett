#pragma once

#include "Global.h"

/// @brief Initializes the application.
/// @param argc Argument count passed by the main function.
/// @param argv Argument vector passed by the main function.
void App_Setup(int argc, char **argv);

/// @brief Loop callback for the application.
/// @param deltaTime Duration of the last frame in seconds.
void App_Loop(float deltaTime);

/// @brief Terminate callback for the application.
/// @param exitCode Exit code for the application termination.
/// @param exitMessage Exit message for the application termination.
void App_Terminate(int exitCode, char *exitMessage);
