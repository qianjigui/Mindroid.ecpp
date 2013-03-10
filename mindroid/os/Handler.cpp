/*
 * Copyright (C) 2011 Daniel Himmelein
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <assert.h>
#include "mindroid/os/Handler.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/MessageQueue.h"
#include "mindroid/os/Looper.h"
#include "mindroid/os/Clock.h"

namespace mindroid {

Handler::Handler() {
	Looper* looper = Looper::myLooper();
	assert(looper != NULL);
	mMessageQueue = &looper->myMessageQueue();
}

Handler::Handler(Looper& looper) {
	mMessageQueue = &looper.myMessageQueue();
}

Handler::~Handler() {
}

void Handler::dispatchMessage(Message& message) {
	if (message.getCallback() != NULL) {
		handleCallback(message);
	} else {
		handleMessage(message);
	}
}

void Handler::handleMessage(Message& message) {
}

bool Handler::sendMessage(Message& message) {
	return sendMessageAtTime(message, Clock::monotonicTime());
}

bool Handler::sendMessageDelayed(Message& message, uint32_t delay) {
	return sendMessageAtTime(message, Clock::monotonicTime() + delay * 1000000LL);
}

bool Handler::sendMessageAtTime(Message& message, uint64_t execTimestamp) {
	if (message.getHandler() == this) {
		return mMessageQueue->enqueueMessage(message, execTimestamp);
	} else {
		return false;
	}
}

bool Handler::post(Runnable& runnable) {
	if (obtainMessage(runnable.mWrapperMessage, runnable)) {
		return sendMessage(runnable.mWrapperMessage);
	} else {
		return false;
	}
}

bool Handler::postDelayed(Runnable& runnable, uint32_t delay) {
	if (obtainMessage(runnable.mWrapperMessage, runnable)) {
		return sendMessageDelayed(runnable.mWrapperMessage, delay);
	} else {
		return false;
	}
}

bool Handler::postAtTime(Runnable& runnable, uint64_t execTimestamp) {
	if (obtainMessage(runnable.mWrapperMessage, runnable)) {
		return sendMessageAtTime(runnable.mWrapperMessage, execTimestamp);
	} else {
		return false;
	}
}

bool Handler::removeMessages(int32_t what) {
	return mMessageQueue->removeMessages(this, what);
}

bool Handler::removeCallbacks(Runnable& runnable) {
	return mMessageQueue->removeCallbacks(this, &runnable);
}

bool Handler::removeCallbacksAndMessages() {
	return mMessageQueue->removeCallbacksAndMessages(this);
}

} /* namespace mindroid */