#define SHUC_NO_RUN_LOG
#define SHUC_MAX_COMMAND_BUFFER_SIZE 8192
#define SHUC_ENABLE_INCREMENTAL
#define SHUILD_IMPLEMENTATION
#define SHUC_SHORT_LOG
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
        SHU_LogError(1, "Usage is <compiler> <r/d/sa/st/su/sm> [clean]");
    }

    char isDebug = 0;
	char isClean = 0;

	const char *compilerStr = argv[1];
	const char *buildOptStr = argv[2];

    if(strcmp(buildOptStr, "r") == 0)
    {
        isDebug = 0;
    }
    else if(strcmp(buildOptStr, "d") == 0)
    {
    	isDebug = DEBUG_REGULAR;
    }
    else if(strcmp(buildOptStr, "sa") == 0)
    {
		isDebug = DEBUG_SANITIZE_ADDRESS;
    }
    else if(strcmp(buildOptStr, "st") == 0)
    {
		isDebug = DEBUG_SANITIZE_THREAD;
    }
    else if(strcmp(buildOptStr, "su") == 0)
    {
		isDebug = DEBUG_SANITIZE_UNDEFINED;
    }
    else if(strcmp(buildOptStr, "sm") == 0)
    {
		isDebug = DEBUG_SANITIZE_MEMORY;
    }
    else
    {
        SHU_LogError(1, "Specify debug or release build with second parameter <r/d/sa/st/su/sm>.");
    }

	if(isDebug > DEBUG_REGULAR && strcmp(compilerStr, "clang") != 0)
	{
		SHU_LogError(1, "Sanitizers can only be used with Clang compiler");
	}

    if(argv[3] != NULL && strcmp(argv[3], "clean") == 0)
    {
		isClean = 1;
    }
    else if(argv[3] != NULL)
    {
        SHU_LogError(1, "Specify clean build with thrid parameter [clean].");
    }

    SHU_CompilerTryConfigure(compilerStr);
    SHU_UtilAutomate(argc, argv);

    SHU_CacheConfigure(isDebug ? ".shu/debug/" : ".shu/release/");

    if (isClean)
    {
        SHU_LogWarning("Performing clean build...");
        SHU_CacheClearAll();
    }

    SHU_CompilerAddFlags("-O3 -Wno-format-truncation");
    SHU_ModuleBegin("shuild", "dependencies/Code-Romeo/");
    SHU_ModuleAddSourceFile("shuild.c");
    SHU_ModuleCompile("dependencies/Code-Romeo/", SHUM_MODULE_EXECUTABLE);

    SHU_CompilerClearFlags();

    int result = 0;
#if SHUM_HOST_PLATFORM == SHUM_PLATFORM_WINDOWS
    result = SHU_UtilRun(".\\dependencies\\Code-Romeo\\shuild.exe %s %s %s", argv[1], buildOptStr, isClean ? "clean" : "");
#else
    result = SHU_UtilRun("./dependencies/Code-Romeo/shuild %s %s %s", argv[1], buildOptStr, isClean ? "clean" : "");
#endif

    if (result != 0)
    {
        SHU_LogError(2, "Error shuilding Code-Romeo : %d", result);
    }

    SHU_CompilerAddFlags(SHUM_FLAGS_STANDARD_C2X);

    if (isDebug > 0)
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_DEBUG SHUM_FLAGS_WARNING_ERROR);
        SHU_CompilerAddFlags(SHUM_FLAGS_WARNING_HIGH);
        SHU_CompilerAddFlags("-Wno-format-nonliteral -Wno-unused-function");

        if(SHU_CompilerGetIdentifier() == SHUM_COMPILER_CLANG)
        {
        	SHU_CompilerAddFlags("-Wno-gnu-zero-variadic-macro-arguments");
        }
    }
    else
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_OPTIMIZATION_HIGH);
    }

    switch(isDebug)
    {
    case DEBUG_SANITIZE_ADDRESS:
		SHU_CompilerAddFlags("-fsanitize=address");
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
