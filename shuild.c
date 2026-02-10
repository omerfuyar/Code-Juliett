#define SHUC_NO_RUN_LOG
#define SHUC_MAX_COMMAND_BUFFER_SIZE 8192
#define SHUC_ENABLE_INCREMENTAL
#define SHUILD_IMPLEMENTATION
#include "dependencies/shuild/shuild.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        SHU_LogError(1, "Usage is <compiler> <d/r> [clean]");
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
    SHU_UtilAutomate(argc, argv);

    SHU_CacheConfigure(isDebug ? ".shu/debug/" : ".shu/release/");

    if (argc > 3)
    {
        SHU_LogWarning("Performing clean build...");
        SHU_CacheClearAll();
    }

    SHU_CompilerAddFlags(SHUM_FLAGS_OPTIMIZATION_HIGH);
    SHU_ModuleBegin("shuild", "dependencies/Code-Romeo/");
    SHU_ModuleAddSourceFile("shuild.c");
    SHU_ModuleCompile("dependencies/Code-Romeo/", SHUM_MODULE_EXECUTABLE);

    SHU_CompilerClearFlags();

    int result = 0;
#if SHUM_HOST_PLATFORM == SHUM_PLATFORM_WINDOWS
    result = SHU_UtilRun(".\\dependencies\\Code-Romeo\\shuild.exe %s %s %s %s", argv[1], argv[2], argc > 3 ? argv[3] : "");
#else
    result = SHU_UtilRun("./dependencies/Code-Romeo/shuild %s %s %s", argv[1], argv[2], argc > 3 ? argv[3] : "");
#endif

    if (result != 0)
    {
        SHU_LogError(2, "Error shuilding Code-Romeo : %d", result);
    }

    SHU_CompilerAddFlags(SHUM_FLAGS_STANDARD_C23);

    if (isDebug)
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_DEBUG SHUM_FLAGS_WARNING_ERROR);
        SHU_CompilerAddFlags(SHUM_FLAGS_WARNING_HIGH);
        SHU_CompilerAddFlags("-Wno-unused-function -Wno-gnu-zero-variadic-macro-arguments -Wno-format-nonliteral -Wno-language-extension-token");
    }
    else
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_OPTIMIZATION_HIGH);
    }

    SHU_ModuleBegin("Code-Juliett", "");

    SHU_ModuleAddIncludeDirectory("include/");
    SHU_ModuleAddIncludeDirectory("dependencies/Code-Romeo/include/");

    SHU_ModuleAddSourceFile("src/");

    SHU_ModuleAddLibraryDirectory(isDebug ? "dependencies/Code-Romeo/build/debug/" : "dependencies/Code-Romeo/build/release/");

    SHU_ModuleLinkLibrary("Code-Romeo");
    SHU_ModuleLinkLibrary("cglm");
    SHU_ModuleLinkLibrary("glfw");
    SHU_ModuleLinkLibrary("glad");
    SHU_ModuleLinkLibrary("stb");
    SHU_ModuleLinkLibrary("miniaudio");

#if SHUM_HOST_PLATFORM == SHUM_PLATFORM_WINDOWS
    SHU_ModuleLinkLibrary("opengl32");
    SHU_ModuleLinkLibrary("gdi32");
    SHU_ModuleLinkLibrary("user32");
    SHU_ModuleLinkLibrary("shell32");
#elif SHUM_HOST_PLATFORM == SHUM_PLATFORM_LINUX
    SHU_ModuleLinkLibrary("dl");
    SHU_ModuleLinkLibrary("m");
    SHU_ModuleLinkLibrary("X11");
    SHU_ModuleLinkLibrary("pthread");
#elif SHUM_HOST_PLATFORM == SHUM_PLATFORM_MACOS
    SHU_ModuleLinkLibrary("Cocoa");
    SHU_ModuleLinkLibrary("OpenGL");
    SHU_ModuleLinkLibrary("IOKit");
    SHU_ModuleLinkLibrary("CoreVideo");
#endif

    SHU_ModuleCompile(isDebug ? "build/debug/" : "build/release/", SHUM_MODULE_EXECUTABLE);

    SHU_UtilCopyFile("resources/", isDebug ? "build/debug/resources/" : "build/release/resources/");

    return 0;
}