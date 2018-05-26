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
    int dd;
    Hello(JobsManager* m) : JobHandler(m){
        jobId = ID;
        m->registerJobHandler(this);
        dd = 10;
    }

    void* getObj() {
        void*i = new int[2];
        (static_cast<int*>(i))[0] = dd++;
        AutoPtrData* d = new AutoPtrData(i);
        return static_cast<void*>(d);
    }

    void onCreate() {
        Message* m;
        uint32_t i;
        for(i=0;i<1;i++){
            m = new Message();
            m->what = MSG_HELLO;
            Log::i("D", "new Message=%p", m);
            send(1, *m);
        }
    }

    void processMessage(const Message& message) {
        Log::i("D", "old Message=%p", &message);
        switch(message.what) {
            case MSG_HELLO:
                Log::i("P", "Hello");
                Message * m = new Message();
                Log::i("D", "new Message=%p", m);
                m->what = MSG_WORLD;
                m->obj = getObj();
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
                if(message.obj!=NULL){
                    AutoPtrData* d = static_cast<AutoPtrData*>(message.obj);
                    Log::i("P", "No.=%d", static_cast<int*>(d->obj)[0]);
                }
                Message * m = new Message();
                Log::i("D", "new Message=%p", m);
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
    h->onCreate();
    w->onCreate();
}

void ServiceManager::onDestroy() {
    h->onDestroy();
    w->onDestroy();
}


