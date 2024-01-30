#pragma once

#include "Queue.h"

typedef void (*func_t)(void* element);

/**
 * Create a new active object and run it.
 * @param func the function that used on every element
 * @return ret active object
 */
void* createActiveObject(func_t func);

/**
 * Kill the active object
 * @param activeObject this
 */
void killActiveObject(void* activeObject);

/**
 * Return the Queue of the active object
 * @param activeObject this
 * @return the queue
 */
Queue* getQueue(void* activeObject);

/**
 * Stop the active (not killing)
 * @param activeObject this
 */
void stopActiveObject(void* activeObject);

/**
 * Join to the thread of the active object
 * @param activeObject this
 */
void joinActiveObject(void* activeObject);
