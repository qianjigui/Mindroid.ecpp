#include "JobHandler.h"
#include "mindroid/util/Log.h"

namespace mindroid {

JobsManager::JobsManager(uint32_t j, uint32_t t){
    maxJobs = j;
    maxMessageTypes = t;
    handlerMap = new JobHandler*[j];
    uint32_t i,k;
    for(i=0;i<j;i++){
        handlerMap[i] = NULL;
    }
    messageTypeMap = new JobHandler**[t];
    for(i=0;i<t;i++){
        messageTypeMap[i] = new JobHandler*[j];
        for(k=0;k<j;k++){
            messageTypeMap[i][k] = NULL;
        }
    }
}

void JobsManager::registerJobHandler(JobHandler* h) {
    handlerMap[h->jobId] = h;
}

void JobsManager::registerBroadcast(int32_t what, JobHandler* handler) {
    JobHandler** h = messageTypeMap[what];
    uint32_t i;
    for(i=0;i<maxJobs;i++) {
        if(NULL==h[i]) {
            h[i] = handler;
            break;
        }
    }
}

void JobsManager::broadcast(Message & msg) {
    if(msg.what<0){
        //TODO assert
        return;
    }
    JobHandler** h = messageTypeMap[msg.what];
    uint32_t i;
    for(i=0;i<maxJobs;i++){
        if(NULL!=h[i]) {
            Message* m = new Message(*h[i], msg);
            h[i]->sendMessage(*m);
            incAutoPtr(m->obj);
        }
    }
    destroyMessage(msg);
}

void JobsManager::incAutoPtr(void * data) {
    if(NULL!=data){
        AutoPtrData* data = static_cast<AutoPtrData*> (data);
        data->inc();
    }
}

void JobsManager::destroyMessage(const Message& m) {
    void* data = m.obj;
    if(NULL!=data){
        Log::i("DD", "obj=%p", data);
        AutoPtrData* d = static_cast<AutoPtrData*> (data);
        if(0==d->dec()){
            free(d->obj);
        }
    }
    //Log::d("D", "delete %x", &&m);
    delete &m;
}

JobHandler* JobsManager::getJobHandler(uint32_t id) {
    if(maxJobs>0){
        return handlerMap[id];
    }
    return NULL;
}


Message* JobsManager::bindJobHandler(uint32_t id, Message& message){
    JobHandler* h = getJobHandler(id);
    if(NULL!=h){
        Log::i("D", "bind message=%p", &message);
        void* obj = message.obj;
        message = *h->obtainMessage(message, message.what, message.arg1, message.arg2);
        message.obj=obj;
        Log::i("D", "bind messagereturn=%p", &message);
        return &message;
    } else {
        destroyMessage(message);
        return NULL;
    }
}

JobHandler::JobHandler(JobsManager* m) {
    manager = m;
}

void JobHandler::onCreate(){}
void JobHandler::onDestroy(){}

Message* mindroid::JobHandler::newMessage() {
    return new Message();
}

void JobHandler::destroyMessage(const Message& m) {
    manager -> destroyMessage(m);
}

void JobHandler::handleMessage(const Message& message){
    processMessage(message);
    destroyMessage(message);
}

void JobHandler::broadcast(Message& msg) {
    manager->broadcast(msg);
}

bool JobHandler::send(uint32_t id, Message& message){
    Message* m=manager->bindJobHandler(id, message);
    if(NULL!=m){
        return m->getTarget()->sendMessage(*m);
    } else {
        destroyMessage(message);
    }
    return false;
}
bool JobHandler::sendDelayed(uint32_t id, Message& message, uint32_t delay){
    Message* m=manager->bindJobHandler(id, message);
    if(NULL!=m){
        return m->getTarget()-> sendMessageDelayed(*m, delay);
    } else {
        destroyMessage(message);
    }
    return false;
}

bool JobHandler::sendAtTime(uint32_t id, Message& message, uint64_t uptimeMillis){
    Message* m=manager->bindJobHandler(id, message);
    if(NULL!=m){
        return m->getTarget()->sendMessageAtTime(*m, uptimeMillis);
    } else {
        destroyMessage(message);
    }
    return false;
}

}

