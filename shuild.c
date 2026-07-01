#define SHUC_NO_RUN_LOG
#define SHUC_MAX_COMMAND_BUFFER_SIZE 8192
#define SHUC_ENABLE_INCREMENTAL
#define SHU_IMPLEMENTATION
#include "dependencies/Code-Romeo/dependencies/shu/shu.h"
#include "dependencies/shuild/shuild.h"

#define DEBUG_REGULAR 1
#define DEBUG_SANITIZE_ADDRESS 2
#define DEBUG_SANITIZE_THREAD 3
#define DEBUG_SANITIZE_UNDEFINED 4
#define DEBUG_SANITIZE_MEMORY 5

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        SHU_LogError(1, "Usage is <compiler> <r/d/dsa/dst/dsu/dsm> [dynamic] [clean]");
    }

    char isDebug = 0;
    char isClean = 0;
    char isDynamic = 0;

    const char *compilerStr = argv[1];
    const char *buildOptStr = argv[2];
    const char *buildName = NULL;

    if (strcmp(buildOptStr, "r") == 0)
    {
        isDebug = 0;
        buildName = "release";
    }
    else if (strcmp(buildOptStr, "d") == 0)
    {
        isDebug = DEBUG_REGULAR;
        buildName = "debug";
    }
    else if (strcmp(buildOptStr, "dsa") == 0)
    {
        isDebug = DEBUG_SANITIZE_ADDRESS;
        buildName = "debug_sanitize_address";
    }
    else if (strcmp(buildOptStr, "dst") == 0)
    {
        isDebug = DEBUG_SANITIZE_THREAD;
        buildName = "debug_sanitize_thread";
    }
    else if (strcmp(buildOptStr, "dsu") == 0)
    {
        isDebug = DEBUG_SANITIZE_UNDEFINED;
        buildName = "debug_sanitize_undefined";
    }
    else if (strcmp(buildOptStr, "dsm") == 0)
    {
        isDebug = DEBUG_SANITIZE_MEMORY;
        buildName = "debug_sanitize_memory";
    }
    else
    {
        SHU_LogError(1, "Unknown argument '%s', Specify debug or release build with second parameter <r/d/dsa/dst/dsu/dsm>.", buildOptStr);
    }

    if (isDebug > DEBUG_REGULAR && strcmp(compilerStr, "clang") != 0)
    {
        SHU_LogError(1, "Sanitizers can only be used with Clang compiler.");
    }

    for (int i = 3; i < argc; i++)
    {
        const char *const optionalArg = argv[i];

        if (isClean == 0 && strcmp(optionalArg, "clean") == 0)
        {
            isClean = 1;
        }
        else if (isDynamic == 0 && strcmp(optionalArg, "dynamic") == 0)
        {
            isDynamic = 1;
        }
        else
        {
            SHU_LogError(1, "Unknown argument '%s', try [dynamic] [clean].", optionalArg);
        }
    }

    SHU_CompilerTryConfigure(compilerStr);
    SHU_UtilAutomate(argc, argv);

    char strBuffer[SHUC_MAX_STRING_SIZE] = {0};
    snprintf(strBuffer, SHUC_MAX_STRING_SIZE, ".shu/%s/%s/", isDynamic ? "dynamic" : "static", buildName);
    SHU_CacheConfigure(strBuffer);

    if (isClean)
    {
        SHU_LogWarning("Performing clean build...");
        SHU_CacheClearAll();
    }

    SHU_CompilerAddFlags(SHUILD_BUILD);
    SHU_ModuleBegin("shuild", "dependencies/Code-Romeo/");
    SHU_ModuleAddSourceFile("shuild.c");
    SHU_ModuleCompile("dependencies/Code-Romeo/", SHUModuleType_Executable);

    SHU_CompilerClearFlags();

    int result = 0;
#if SHUM_HOST_PLATFORM == SHUM_PLATFORM_WINDOWS
    result = SHU_UtilRun(".\\dependencies\\Code-Romeo\\shuild.exe %s %s %s %s", argv[1], buildOptStr, isDynamic ? "dynamic" : "", isClean ? "clean" : "");
#else
    result = SHU_UtilRun("./dependencies/Code-Romeo/shuild %s %s %s %s", argv[1], buildOptStr, isDynamic ? "dynamic" : "", isClean ? "clean" : "");
#endif

    if (result != 0)
    {
        SHU_LogError(2, "Error shuilding Code-Romeo : %d", result);
    }

    SHU_CompilerAddFlags(SHUM_FLAGS_STANDARD_C2X);

    if (isDebug > 0)
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_DEBUG SHUM_FLAGS_OPTIMIZATION_DEBUG SHUM_FLAGS_WARNING_ERROR);
        SHU_CompilerAddFlags(SHUM_FLAGS_WARNING_HIGH);
        SHU_CompilerAddFlags("-Wno-format-nonliteral -Wno-unused-function -Og");

        if (SHU_CompilerGetIdentifier() == SHUM_COMPILER_CLANG)
        {
            SHU_CompilerAddFlags("-Wno-gnu-zero-variadic-macro-arguments -fno-omit-frame-pointer");
        }
    }
    else
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_OPTIMIZATION_HIGH);
    }

    switch (isDebug)
    {
    case DEBUG_SANITIZE_ADDRESS:
        SHU_CompilerAddFlags("-fsanitize=address,leak");
        break;

    case DEBUG_SANITIZE_THREAD:
        SHU_CompilerAddFlags("-fsanitize=thread");
        break;

    case DEBUG_SANITIZE_UNDEFINED:
        SHU_CompilerAddFlags("-fsanitize=undefined");
        break;

    case DEBUG_SANITIZE_MEMORY:
        SHU_CompilerAddFlags("-fsanitize=memory");
        break;
    }

    SHU_ModuleBegin("Code-Juliett", NULL);

    SHU_ModuleAddIncludeDirectory("include/");
    SHU_ModuleAddIncludeDirectory("dependencies/Code-Romeo/include/");

    SHU_ModuleAddSourceFile("src/");

    snprintf(strBuffer, SHUC_MAX_STRING_SIZE, "dependencies/Code-Romeo/build/%s/%s/", isDynamic ? "dynamic" : "static", buildName);
    SHU_ModuleAddLibraryDirectory(strBuffer);

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

    snprintf(strBuffer, SHUC_MAX_STRING_SIZE, "build/%s/%s/", isDynamic ? "dynamic" : "static", buildName);
    SHU_ModuleCompile(strBuffer, SHUModuleType_Executable);

    snprintf(strBuffer, SHUC_MAX_STRING_SIZE, "build/%s/%s/resources/", isDynamic ? "dynamic" : "static", buildName);
    SHU_UtilCopyFile("resources/", strBuffer);

    return 0;
}
