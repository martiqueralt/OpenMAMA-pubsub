#include <stdio.h>
#include <signal.h>
#include <mama/mama.h>

mamaBridge bridge = NULL;
mamaPublisher publisher = NULL;

void StopHandler(int value)
{
    signal(value, SIG_IGN);
    printf(" Do you want to stop OpenMAMA? [y/n]: ");
    char answer = getchar();
    if (answer == 'y')
    {
        mamaPublisher_destroy(publisher);
        mama_stop(bridge);
    }
    else
    {
        signal(SIGINT, StopHandler);
    }
    getchar();
}

int main(int argc, const char** argv)
{
    signal(SIGINT, StopHandler);
    printf("Publishing message with OpenMAMA.\n");

    mamaQueue publishQueue = NULL;
    mamaDispatcher publishDispatcher = NULL;
    mamaTransport transport = NULL;
    mamaMsg message = NULL;
    
    mama_status status;
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_openWithProperties(".","mama.properties")) == MAMA_STATUS_OK) &&
        ((status = mamaQueue_create(&publishQueue, bridge)) == MAMA_STATUS_OK) &&
        ((status = mamaDispatcher_create(&publishDispatcher, publishQueue)) == MAMA_STATUS_OK) &&
        ((status = mamaTransport_allocate(&transport)) == MAMA_STATUS_OK) &&
        ((status = mamaTransport_create(transport, "pub", bridge)) == MAMA_STATUS_OK) &&
        ((status = mamaPublisher_create(&publisher, transport, "topic", NULL, NULL)) == MAMA_STATUS_OK) &&
        ((status = mamaMsg_create(&message)) == MAMA_STATUS_OK))
    {
        // add some fields to the message
        mamaMsg_addI32(message, MamaFieldMsgType.mName, MamaFieldMsgType.mFid, MAMA_MSG_TYPE_INITIAL);
        mamaMsg_addI32(message, MamaFieldMsgStatus.mName, MamaFieldMsgStatus.mFid, MAMA_MSG_STATUS_OK);
        mamaMsg_addString(message, "MdMyField", 99, "string value");

        // NOTICE active thread block on the following call
        mama_start(bridge);
        // NOTICE active thread resumes here after mama_stop() call
        printf("Closing OpenMAMA.\n");
        mamaTransport_destroy(transport);
        mamaDispatcher_destroy(publishDispatcher);
        mamaQueue_destroy(publishQueue);
        mama_close();
        exit(0);
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}
