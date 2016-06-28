#include <stdio.h>
#include <signal.h>
#include <mama/mama.h>

mamaBridge bridge = NULL;

void StopHandler(int value)
{
    signal(value, SIG_IGN);
    printf(" Do you want to stop OpenMAMA? [y/n]: ");
    char answer = getchar();
    if (answer == 'y')
        mama_stop(bridge);
    else
        signal(SIGINT, StopHandler);
    getchar();
}

int main(int argc, const char** argv)
{
    signal(SIGINT, StopHandler);
    printf("Publishing message with OpenMAMA.\n");

    mama_status status = mama_loadBridge(&bridge, "solace");
    if ((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK)
    {
        if ((status = mama_open()) == MAMA_STATUS_OK)
        {
            mama_start(bridge);

            // here

            printf("Closing OpenMAMA.\n");
            mama_close();
            exit(0);
        }
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
