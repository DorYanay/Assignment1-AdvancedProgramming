#include "Queue.h"

Queue::Queue() {
    pthread_mutex_init(&m_lock, nullptr);
    pthread_cond_init(&m_lockCond, nullptr);
}

Queue::~Queue() {
    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_lockCond);
}

Queue::Queue(Queue &&other) noexcept {
    this->m_data = other.m_data;
    this->m_lock = other.m_lock;
    this->m_lockCond = other.m_lockCond;
}

Queue& Queue::operator=(Queue &&other) noexcept {
    this->m_data = other.m_data;
    this->m_lock = other.m_lock;
    this->m_lockCond = other.m_lockCond;
    return *this;
}

void Queue::enqueue(void *element) {
    pthread_mutex_lock(&m_lock);

    m_data.insert(m_data.begin(), element);
    pthread_cond_signal(&m_lockCond);

    pthread_mutex_unlock(&m_lock);
}

void *Queue::dequeue() {
    pthread_mutex_lock(&m_lock);

    while (m_data.empty()) {
        pthread_cond_wait(&m_lockCond, &m_lock);
    }

    void* element = m_data.front();
    m_data.pop_back();

    pthread_mutex_unlock(&m_lock);
    return element;
}
