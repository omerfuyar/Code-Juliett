#include "RJGlobal.h"
#include "app/App.h"

/*
    This file is just a connector between user callbacks and the framework.
*/

int main(int argc, char **argv)
{
    RJGlobal_SetSetupCallback(App_Setup);
    RJGlobal_SetLoopCallback(App_Loop);
    RJGlobal_SetTerminateCallback(App_Terminate);

    RJGlobal_Run(argc, argv);
}
