#define SHUC_NO_RUN_LOG
#define SHUC_MAX_COMMAND_BUFFER_SIZE 8192
#define SHUILD_IMPLEMENTATION
#include "shuild.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        goto usageError;
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
    SHU_Automate(argc, argv);

    if (argc > 3)
    {
        SHU_CompilerSetFlags("-O3");
        SHU_ModuleBegin("ShuildRomeo");
        SHU_ModuleAddSourcefile("dependencies/Romeo/ShuildRomeo.c");
        SHU_ModuleCompile("dependencies/Romeo/", SHUM_MODULE_EXECUTABLE);
        SHU_Run(".\\dependencies\\Romeo\\ShuildRomeo.exe %s %s", argv[1], argv[2]);
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
            compilerFlags = "-O2 -DNDEBUG";
        }
        else
        {
            return 3;
        }
    }

    SHU_CompilerSetFlags(compilerFlags);

    // Build test executables
    const char *testFiles[] = {
        "test_vector.c",
        "test_string.c",
        "test_hashmap.c",
        "test_listarray.c",
        "test_timer.c"
    };
    
    int numTests = sizeof(testFiles) / sizeof(testFiles[0]);
    
    for (int i = 0; i < numTests; i++)
    {
        char moduleName[128];
        snprintf(moduleName, sizeof(moduleName), "%.*s", (int)(strlen(testFiles[i]) - 2), testFiles[i]);
        
        SHU_ModuleBegin(moduleName);
        
        SHU_ModuleAddIncludeDirectory("tests/");
        SHU_ModuleAddIncludeDirectory("dependencies/Romeo/include/");
        
        char sourceFile[256];
        snprintf(sourceFile, sizeof(sourceFile), "tests/%s", testFiles[i]);
        SHU_ModuleAddSourcefile(sourceFile);
        
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
        SHU_ModuleLinkLibrary("m");
#elif SHUM_HOST_PLATFORM == SHUM_PLATFORM_MACOS
        SHU_ModuleLinkLibrary("Cocoa");
        SHU_ModuleLinkLibrary("OpenGL");
        SHU_ModuleLinkLibrary("IOKit");
        SHU_ModuleLinkLibrary("CoreVideo");
#endif
        
        SHU_ModuleCompile(isDebug ? "build/tests/debug/" : "build/tests/release/", SHUM_MODULE_EXECUTABLE);
    }

    return 0;

usageError:
    SHU_LogInfo("Usage is <compiler> <d/r> [all]");
    return 1;
}
