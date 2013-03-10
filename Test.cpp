#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "mindroid/os/Looper.h"
#include "mindroid/os/Handler.h"
#include "mindroid/os/Thread.h"
#include "mindroid/os/Message.h"
#include "mindroid/os/Closure.h"
#include "mindroid/os/Clock.h"

using namespace mindroid;

class Handler3;
class Handler4;
Looper* sLooper1 = NULL;
Looper* sLooper2 = NULL;
Handler* sHandler1 = NULL;
Handler* sHandler2 = NULL;
Handler3* sHandler3 = NULL;
Handler4* sHandler4 = NULL;

class Params
{
public:
	static const uint32_t ARRAY_SIZE = 32;

	Params() {
		for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
			values[i] = i;
		}
	}

	uint32_t values[ARRAY_SIZE];
};

class Handler2 : public Handler
{
public:
	virtual void handleMessage(Message& message) {
		printf("Handler2::handleMessage 0x%x with ID %d by Looper 0x%x\n",
				&message, message.what, Looper::myLooper());
	}
};

class Handler3 : public Handler
{
public:
	Handler3() {
		obtainMessage(mMessage, 3);
	}

	virtual void handleMessage(Message& message) {
		printf("Handler3::handleMessage 0x%x with ID %d by Looper 0x%x\n",
				&message, message.what, Looper::myLooper());
	}

	void test() {
		sendMessage(mMessage);
	}

private:
	Message mMessage;
};

class Handler4 : public Handler
{
public:
	Handler4(Looper& looper) : Handler(looper), mMessage(*this, 4) {}

	virtual void handleMessage(Message& message) {
		printf("Handler4::handleMessage 0x%x with ID %d by Looper 0x%x\n",
				&message, message.what, Looper::myLooper());
		sendMessageDelayed(message, 4000);
		printf("Time: %lld\n", Clock::monotonicTime());
	}

	void test() {
		bool rc = sendMessageDelayed(mMessage, 1000);
//		printf("Handler4: %s\n", rc ? "ok" : "error");
	}

private:
	Message mMessage;
};

class Thread1 : public Thread
{
public:
	virtual void run() {
		Looper::prepare();
		sLooper1 = Looper::myLooper();
		sHandler3 = new Handler3();
		printf("Looper 1 uses thread %d\n", (int32_t) pthread_self());
		Looper::loop();
	}
};

class Thread2 : public Thread
{
public:
	virtual void run() {
		Looper::prepare();
		sLooper2 = Looper::myLooper();
		sHandler2 = new Handler2();
		printf("Looper 2 uses thread %d\n", (int32_t) pthread_self());
		Looper::loop();
	}
};

class Runnable1 : public Runnable
{
public:
	virtual void run() {
		printf("Runnable1::run by Looper 0x%x\n", Looper::myLooper());
		assert(false);
	}
};

class Test
{
public:
	void test(int32_t value) {
		printf("Closure1::test with value %d by Looper 0x%x\n",
				value, Looper::myLooper());
	}
};

Thread1 sThread1;
Thread2 sThread2;
Test sTest;

int main() {
	sThread1.start();
	sThread2.start();

	Thread::sleep(200);

	sHandler4 = new Handler4(*sLooper2);
	Closure1<Test, int32_t> closure1;
	Runnable1 runnable1;

	//TODO: Repost in handleMessage callback

	sHandler4->test();
	int i = 0;
	while (true) {
		bool abc = obtainClosure<Test, int32_t>(closure1, sTest, &Test::test, i++);
//		if (!abc) {
//			printf("Error\n");
//		}
		sHandler2->postDelayed(runnable1, 100);
		sHandler2->removeCallbacks(runnable1);
		sHandler2->postDelayed(closure1, 500);


		sHandler3->test();

		Thread::sleep(1000);
	}

	sLooper1->quit();
	sThread1.join();
	sLooper2->quit();
	sThread2.join();

	return 0;
}