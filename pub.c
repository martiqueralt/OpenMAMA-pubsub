#include <stdio.h>
#include <signal.h>
#include <mama/mama.h>


void StopHandler(int value)
{
    signal(value, SIG_IGN);
    printf(" Do you want to stop OpenMAMA? [y/n]: ");
    char answer = getchar();
    if (answer == 'y')
    {
        // mama_stop(bridge);
    }
    else
    {
        signal(SIGINT, StopHandler);
    }
    getchar();
}

int main(int argc, const char** argv)
{
    printf("Publishing message with OpenMAMA.\n");
    
    mama_status status;
    mamaBridge bridge = NULL;
    // load Solace bridge and initiate MAMA
    if (((status = mama_loadBridge(&bridge, "solace")) == MAMA_STATUS_OK) &&
        ((status = mama_openWithProperties(".","mama.properties")) == MAMA_STATUS_OK))
    {
        // create transport and publisher
        mamaTransport transport = NULL;
        mamaPublisher publisher = NULL;
        if (((status = mamaTransport_allocate(&transport)) == MAMA_STATUS_OK) &&
            ((status = mamaTransport_create(transport, "pub", bridge)) == MAMA_STATUS_OK) &&
            ((status = mamaPublisher_create(&publisher, transport, "tutorial.topic", NULL, NULL)) == MAMA_STATUS_OK))
        {
            // create message and add some fields to it
            mamaMsg message = NULL;
            if (((status = mamaMsg_create(&message)) == MAMA_STATUS_OK) &&
                ((status = mamaMsg_addI32(message, MamaFieldMsgType.mName, MamaFieldMsgType.mFid, 
                                          MAMA_MSG_TYPE_INITIAL)) == MAMA_STATUS_OK) &&
                ((status = mamaMsg_addI32(message, MamaFieldMsgStatus.mName, MamaFieldMsgStatus.mFid,
                                          MAMA_MSG_STATUS_OK)) == MAMA_STATUS_OK) &&
                ((status = mamaMsg_addString(message, "MdMyField", 99, 
                                             "string value")) == MAMA_STATUS_OK))
            {
                // publish the message
                if ((status = mamaPublisher_send (publisher, message)) == MAMA_STATUS_OK)
                {
                    printf("Message published, closing OpenMAMA.\n");
                    mamaPublisher_destroy(publisher);
                    mamaTransport_destroy(transport);
                    mama_close();
                    // normal exit
                    exit(0);
                }
            }
        }
    }
    printf("OpenMAMA error: %s\n", mamaStatus_stringForStatus(status));
    exit(status);
}

