#include "RJGlobal.h"
#include "app/App.h"

/*
    This file is just a connector between user callbacks and the framework.
*/

int main(int argc, char **argv)
{
    RJ_SetSetupCallback(App_Setup);
    RJ_SetLoopCallback(App_Loop);
    RJ_SetTerminateCallback(App_Terminate);

    RJ_Run(argc, argv);
}
