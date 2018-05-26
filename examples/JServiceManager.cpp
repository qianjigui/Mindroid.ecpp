#include "JServiceManager.h"
#include "JobHandler.h"
#include "mindroid/util/Log.h"

using namespace mindroid;

static JobsManager* manager;

const char* const ServiceManager::LOG_TAG = "ServiceManager";

const static uint32_t MSG_HELLO = 1;
const static uint32_t MSG_WORLD = 2;


class Hello : public JobHandler {
public:
    const static uint32_t ID = 1;
    Hello(JobsManager* m) : JobHandler(m){
        jobId = ID;
        m->registerJobHandler(this);
    }

    void processMessage(const Message& message) {
        switch(message.what) {
            case MSG_HELLO:
                Log::i("P", "Hello");
                Message * m = new Message();
                m->what = MSG_WORLD;
                send(2, *m);
                break;
        }
    }
};

class World : public JobHandler {
public:
    const static uint32_t ID = 2;
    World(JobsManager* m) : JobHandler(m){
        jobId = ID;
        m->registerJobHandler(this);
    }

    void processMessage(const Message& message) {
        switch(message.what) {
            case MSG_WORLD:
                Log::i("P", "World");
                Message * m = new Message();
                m->what = MSG_HELLO;
                sendDelayed(1, *m, 1000);
                break;
        }
    }
};

ServiceManager::ServiceManager() {
    manager = new JobsManager(5,5);
    h = new Hello(manager);
    w = new World(manager);
}

void ServiceManager::onCreate() {
}

void ServiceManager::onDestroy() {
}


