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

    mamaQueue publishQueue = NULL;
    mamaDispatcher publishDispatcher = NULL;
    
    mama_status status = mama_loadBridge(&bridge, "solace");
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_open()) == MAMA_STATUS_OK) &&
        ((status = mamaQueue_create(&publishQueue, bridge)) == MAMA_STATUS_OK) &&
        ((status = mamaDispatcher_create(&publishDispatcher, publishQueue)) == MAMA_STATUS_OK))
    {
        // NOTICE active thread block on the following call
        mama_start(bridge);
        // NOTICE active thread resumes here after mama_stop() call
        printf("Closing OpenMAMA.\n");
        mamaDispatcher_destroy(publishDispatcher);
        mamaQueue_destroy(publishQueue);
        mama_close();
        exit(0);
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
