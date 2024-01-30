#include "active_object.h"

#include <pthread.h>
#include <unistd.h>

class ActiveObject 
{
public:
	func_t func;
    Queue queue;
    pthread_t thread = 0;
    bool running = true;

	ActiveObject() = delete;
	explicit ActiveObject(func_t func)
        : func(func), queue() { }
	virtual ~ActiveObject() { }
	ActiveObject(const ActiveObject& other) = delete;
	ActiveObject(ActiveObject&& other) noexcept = delete;
	ActiveObject& operator=(Queue& other) = delete;
    ActiveObject& operator=(ActiveObject&& other)  noexcept = delete;
};

static void* threadLoop(void* activeObjectPtr);

void* createActiveObject(func_t func)
{
    auto activeObject = new ActiveObject(func);

    if (pthread_create(&activeObject->thread, nullptr, &threadLoop, (void*)activeObject) != 0) {
        ERROR("Failed to create the thread");
    }

    return (void*)activeObject;
}

static void* threadLoop(void* activeObjectPtr) {
    auto activeObject = (ActiveObject*)activeObjectPtr;

    while (activeObject->running) {
        void* task = activeObject->queue.dequeue();
        if (activeObject->running) {
            activeObject->func(task);
        }
    }

    return nullptr;
}

Queue* getQueue(void* activeObject)
{
    return &((ActiveObject*)activeObject)->queue;
}

void killActiveObject(void* activeObjectPtr)
{
    auto activeObject = (ActiveObject*)activeObjectPtr;

    activeObject->running = false;
    pthread_cancel(activeObject->thread);

    delete activeObject;
}

void stopActiveObject(void* activeObjectPtr)
{
    auto activeObject = (ActiveObject*)activeObjectPtr;

    if (activeObject->running) {
        activeObject->running = false;
    }}

void joinActiveObject(void* activeObjectPtr)
{
    auto activeObject = (ActiveObject*)activeObjectPtr;

    if (!activeObject->running) {
        return;
    }

    if (pthread_join(activeObject->thread, nullptr) != 0) {
        ERROR("Failed to join the thread");
    }
}
