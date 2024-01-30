#pragma once

#include <vector>

#include "build.h"

/**
 * A blocking Queue that threads saved
 */
class Queue
{
private:
    std::vector<void*> m_data;
    pthread_mutex_t m_lock{};
    pthread_cond_t m_lockCond{};

public:
	Queue();
	virtual ~Queue();
	Queue(const Queue& other) = delete;
	Queue(Queue&& other) noexcept;

	Queue& operator=(Queue& other) = delete;
    Queue& operator=(Queue&& other) noexcept;

	void enqueue(void* element);
	void* dequeue();
};
