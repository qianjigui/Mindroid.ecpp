#ifndef JOBHANDLER_H_
#define JOBHANDLER_H_

#include <stdint.h>
#include "mindroid/app/Service.h"
#include "mindroid/lang/Closure.h"
#include "mindroid/lang/Object.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"

#define MAX_JOB_HANDLERS (1024);

namespace mindroid {

class JobHandler;
class JobsManager {
public:
    JobsManager(uint32_t maxJobs, uint32_t maxMessageTypes);
    JobHandler* getJobHandler(uint32_t id);
    JobHandler* bindJobHandler(uint32_t id, Message&);
    void registerJobHandler(JobHandler*);
    void registerBroadcast(int32_t what, JobHandler* handler);
    void broadcast(Message&);
    void destroyMessage(const Message&);
    void incAutoPtr(void * data);
private:
    uint32_t maxJobs, maxMessageTypes;
    JobHandler** handlerMap;
    JobHandler*** messageTypeMap;
};

class AutoPtrData {
public:
    AutoPtrData(void* o){
        obj = o;
        prtCount = 0;
    }
    void* obj;
    inline uint32_t inc() {return ++prtCount;}
    inline uint32_t dec() {return --prtCount;};
    inline uint32_t getPrtCount(){return prtCount;};
private:
    uint32_t prtCount;
};

/*
 *
 */
class JobHandler : public Service, public Handler {
public:
    JobHandler(JobsManager* m);
    virtual void onCreate();
    virtual void onDestroy();
    Message* newMessage();
    void destroyMessage(const Message&);
    void handleMessage(const Message& message);
    void broadcast(Message& message);
    void send(uint32_t id, Message& message);
    bool sendDelayed(uint32_t id, Message& message, uint32_t delay);
    bool sendAtTime(uint32_t id, Message& message, uint64_t uptimeMillis);
    uint32_t jobId;
    virtual void processMessage(const Message& message) = 0;

private:

    JobsManager* manager;
    void bindHandler(JobHandler* , Message&);

    NO_COPY_CONSTRUCTOR_AND_ASSIGNMENT_OPERATOR(JobHandler)
};
}



#endif /* HELLOWORLD_H_ */
