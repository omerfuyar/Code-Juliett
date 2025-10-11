#include "Global.h"
#include "user/App.h"

/*
    This file is just a connector between user callbacks and the framework.
*/

int main(int argc, char **argv)
{
    Global_SetSetupCallback(App_Setup);
    Global_SetLoopCallback(App_Loop);
    Global_SetTerminateCallback(App_Terminate);

    Global_Run(argc, argv);
}
