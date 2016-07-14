#include <iostream>
#include <fstream>
#include <stdio.h>

#include "mama/mama.h"
#include "mama/mamacpp.h"

using namespace Wombat;
using std::cerr;
using std::cout;
using std::endl;

class Sample : public MamaPublisherCallback, public MamaStartCallback
{
public:
    Sample() {
		msg = NULL;
		source = "PB-NAMR-DEV";
		symbol="TEST_001.PUB";
		transportName = "vmr";
		bridgeName = "solace";
		count = 2;
		interval = 2;
		msgCount = 1;
	}

    virtual ~Sample() { }

	MamaMsg* msg;
	const char* source;
	const char* symbol;
	const char* transportName;
	const char* bridgeName;
	int count;
	int interval;
	int msgCount;

	void run()
	{
    	try
    	{
    		mamaBridge gBridge = Mama::loadBridge(bridgeName);
	
   			Mama::open(".", "mama.properties");
	
       		MamaQueue* gDefaultQueue = Mama::getDefaultEventQueue (gBridge);
	
			MamaTransport* transport = new MamaTransport();
       		transport->create (transportName, gBridge);
	
			MamaPublisher* pub = new MamaPublisher();
       		pub->createWithCallbacks(transport, gDefaultQueue, this, NULL, symbol, source);
	
   			Mama::startBackground(gBridge, this);
	
			for (int i = 0; i < count; i++) {
				publishMessage(pub);
				sleep(interval);
			}
	
			mama_log(MAMA_LOG_LEVEL_NORMAL, "stop bridge");
        	Mama::stop(gBridge);
	
			mama_log(MAMA_LOG_LEVEL_NORMAL, "delete publisher");
            pub->destroy();
            // delete pub;
		    sleep(2);
	
			mama_log(MAMA_LOG_LEVEL_NORMAL, "delete transport");
			delete transport;
			sleep(2);
	
			mama_log(MAMA_LOG_LEVEL_NORMAL, "Mama::close");
   			Mama::close();
			sleep(2);
   		}
   		catch (MamaStatus &status)
   		{
       		cerr << "Caught MAMA exception: " << status.toString () << endl;
       		exit (1);
   		}
	}

	void publishMessage (MamaPublisher* pub)
	{
    	try
    	{
			if (msg != NULL) {
				msg->clear();;
			} else {
				msg = new MamaMsg();
				msg->create();
			}
	
        	msg->addU8("", 1, 1);
        	msg->addU8("", 2, 0);
        	msg->addString("", 121, symbol);
	
			printf("Sending msg %d\n", msgCount++);
        	pub->send(msg);
    	}
    	catch (MamaStatus &status)
    	{
        	cerr << "Error publishing message: "
             	<< status.toString () << endl;
        	exit (1);
    	}
	}

	virtual void onCreate(MamaPublisher* pub, void* closure)
	{
		printf("onCreate\n");
	}

	virtual void onDestroy(MamaPublisher* pub, void* closure)
	{
		printf("onDestroy\n");
		delete pub;
	}

	virtual void onError(MamaPublisher* pub, const MamaStatus& status, const char* msg, void* closure)
	{
		printf("onError\n");
	}

	virtual void onStartComplete(MamaStatus status) 
	{
	}
};

int main (int argc, const char **argv)
{
    setbuf (stdout, NULL);
    setbuf (stderr, NULL);

	Sample* m = new Sample();
	m->run();

	delete m;

	return 0;
}

