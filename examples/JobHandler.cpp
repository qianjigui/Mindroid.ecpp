#include "JobHandler.h"

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
        AutoPtrData* data = static_cast<AutoPtrData*> (data);
        if(0==data->dec()){
            //TODO delete data
        }
    }
    delete &m;
}

JobHandler* JobsManager::getJobHandler(uint32_t id) {
    if(maxJobs>0){
        return handlerMap[id];
    }
    return NULL;
}


JobHandler* JobsManager::bindJobHandler(uint32_t id, Message& message){
    JobHandler* h = getJobHandler(id);
    if(NULL!=h){
        void* obj = message.obj;
        Message::obtain(message, *h, message.what, message.arg1, message.arg2);
        message.obj=obj;
    }
    return h;
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

void JobHandler::broadcast(Message&) {}

void JobHandler::send(uint32_t id, Message& message){
    JobHandler* h=manager->bindJobHandler(id, message);
    if(NULL!=h){
        h->sendMessage(message);
    } else {
        destroyMessage(message);
    }
}
bool JobHandler::sendDelayed(uint32_t id, Message& message, uint32_t delay){
    JobHandler* h=manager->bindJobHandler(id, message);
    if(NULL!=h){
        return h->sendMessageDelayed(message, delay);
    } else {
        destroyMessage(message);
    }
    return false;
}

bool JobHandler::sendAtTime(uint32_t id, Message& message, uint64_t uptimeMillis){
    JobHandler* h=manager->bindJobHandler(id, message);
    if(NULL!=h){
        return h->sendMessageAtTime(message, uptimeMillis);
    } else {
        destroyMessage(message);
    }
    return false;
}

}

