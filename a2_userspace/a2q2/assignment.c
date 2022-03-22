#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "assignment.h"

void
consume_enter(struct resource *resource)
{
    // FILL ME IN

    pthread_mutex_lock(&resource->mutex);

    while(resource->num_consumers >= resource->num_producers * resource->ratio) {
       pthread_cond_wait(&resource->cond, &resource->mutex);

    }
    resource->num_consumers ++;
    pthread_cond_signal(&resource->cond);
}

void
consume_exit(struct resource *resource)
{
    assert(resource->num_consumers > 0);

    resource->num_consumers --;
    pthread_cond_signal(&resource->cond);

    pthread_mutex_unlock(&resource->mutex);
}

void
produce_enter(struct resource *resource)
{
    // FILL ME IN
    pthread_mutex_lock(&resource->mutex);

    assert(resource->num_producers >= 0);
    resource->num_producers += 1;
    pthread_cond_signal(&resource->cond);
}

void
produce_exit(struct resource *resource)
{
    // FILL ME IN
    while(resource->num_consumers >= resource->num_producers * resource->ratio) {
       pthread_cond_wait(&resource->cond, &resource->mutex);
    }
    resource->num_producers -= 1;
    pthread_cond_signal(&resource->cond);

    pthread_mutex_unlock(&resource->mutex);

}
