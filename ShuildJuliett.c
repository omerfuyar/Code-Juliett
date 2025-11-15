#define SHUM_MAX_COMMAND_BUFFER_SIZE 8192
#define SHUM_NO_MODULE_LOG
#define SHUILD_IMPLEMENTATION
#include "shuild.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        return 1;
    }

    char isDebug = -1;

    if (strcmp(argv[2], "d") == 0)
    {
        isDebug = 1;
    }
    else if (strcmp(argv[2], "r") == 0)
    {
        isDebug = 0;
    }
    else
    {
        return 2;
    }

    SHU_CompilerTryConfigure(argv[1]);

    if (argc > 3)
    {
        SHU_CompilerSetFlags("-O3");
        SHU_ModuleBegin("shuildRomeo");
        SHU_ModuleAddSourcefile("dependencies/Romeo/ShuildRomeo.c");
        SHU_ModuleCompile("dependencies/Romeo/", SHUM_MODULE_EXECUTABLE);
        SHU_Run(".\\dependencies\\Romeo\\shuildRomeo.exe %s %s", argv[1], argv[2]);
    }

    char *compilerFlags = NULL;

    if (isDebug)
    {
        if (strcmp(argv[1], "clang-cl") == 0 || strcmp(argv[1], "cl") == 0)
        {
            compilerFlags = "/Zi /Od /W4 /permissive- /GS /WX /wd4324";
        }
        else if (strcmp(argv[1], "clang") == 0 || strcmp(argv[1], "gcc") == 0)
        {
            compilerFlags = "-g -O0 -Wall -Werror -Wextra -Wshadow -Wpedantic -Wconversion -Wnull-dereference -Wunused-result -Wno-strict-prototypes -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-value -fstack-protector-strong ";
        }
        else
        {
            return 3;
        }
    }
    else
    {
        if (strcmp(argv[1], "clang-cl") == 0 || strcmp(argv[1], "cl") == 0)
        {
            compilerFlags = "/O2 /DNDEBUG";
        }
        else if (strcmp(argv[1], "clang") == 0 || strcmp(argv[1], "gcc") == 0)
        {
            compilerFlags = "-O3 -DNDEBUG";
        }
        else
        {
            return 3;
        }
    }

    SHU_CompilerSetFlags(compilerFlags);

    SHU_ModuleBegin("Code-Juliett");

    SHU_ModuleAddIncludeDirectory("include/");
    SHU_ModuleAddIncludeDirectory("dependencies/Romeo/include/");

    SHU_ModuleAddSourceDirectory("src/");

    SHU_ModuleAddLibraryDirectory(isDebug ? "dependencies/Romeo/build/debug/" : "dependencies/Romeo/build/release/");

    SHU_ModuleLinkLibrary("Code-Romeo");
    SHU_ModuleLinkLibrary("cglm");
    SHU_ModuleLinkLibrary("glfw");

#if SHUM_HOST_PLATFORM == SHUM_PLATFORM_WINDOWS
    SHU_ModuleLinkLibrary("opengl32");
    SHU_ModuleLinkLibrary("gdi32");
    SHU_ModuleLinkLibrary("user32");
    SHU_ModuleLinkLibrary("shell32");
#elif SHUM_HOST_PLATFORM == SHUM_PLATFORM_LINUX
    SHU_ModuleLinkLibrary("dl");
#elif SHUM_HOST_PLATFORM == SHUM_PLATFORM_MACOS
    SHU_ModuleLinkLibrary("Cocoa");
    SHU_ModuleLinkLibrary("OpenGL");
    SHU_ModuleLinkLibrary("IOKit");
    SHU_ModuleLinkLibrary("CoreVideo");
#endif

    SHU_ModuleCompile(isDebug ? "build/debug/" : "build/release/", SHUM_MODULE_EXECUTABLE);

    SHU_CopyFile("resources/", isDebug ? "build/debug/resources/" : "build/release/resources/");

    return 0;
}