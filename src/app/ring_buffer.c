#include "ring_buffer.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

bool ring_buffer_init(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_init - NULL pointer\n");
        return false;
    }

    rb->write_pos = 0;
    rb->read_pos = 0;
    rb->available = 0;
    rb->initialized = false;

    if (!MUTEX_INIT(rb->mutex))
    {
        printf("ERROR: ring_buffer_init - Failed to create mutex\n");
        return false;
    }

    rb->initialized = true;
    return true;
}

size_t ring_buffer_write(ring_buffer_t *rb, const int16_t *data, size_t count)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_write - NULL ring buffer pointer\n");
        return 0;
    }

    if (data == NULL)
    {
        printf("ERROR: ring_buffer_write - NULL data pointer\n");
        return 0;
    }

    if (count == 0)
    {
        return 0;
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_write - Ring buffer not initialized\n");
        return 0;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_write - Failed to acquire mutex\n");
        return 0;
    }

    size_t space_available = RING_BUFFER_SIZE - rb->available;
    size_t to_write = (count < space_available) ? count : space_available;

    for (size_t i = 0; i < to_write; i++)
    {
        rb->data[rb->write_pos] = data[i];
        rb->write_pos = (rb->write_pos + 1) % RING_BUFFER_SIZE;
    }

    rb->available += to_write;

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_write - Failed to release mutex\n");
    }

    return to_write;
}

size_t ring_buffer_read(ring_buffer_t *rb, int16_t *data, size_t count)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_read - NULL ring buffer pointer\n");
        return 0;
    }

    if (data == NULL)
    {
        printf("ERROR: ring_buffer_read - NULL data pointer\n");
        return 0;
    }

    if (count == 0)
    {
        return 0;
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_read - Ring buffer not initialized\n");
        return 0;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_read - Failed to acquire mutex\n");
        return 0;
    }

    size_t to_read = (count < rb->available) ? count : rb->available;

    for (size_t i = 0; i < to_read; i++)
    {
        data[i] = rb->data[rb->read_pos];
        rb->read_pos = (rb->read_pos + 1) % RING_BUFFER_SIZE;
    }

    rb->available -= to_read;

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_read - Failed to release mutex\n");
    }

    return to_read;
}

size_t ring_buffer_available(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_available - NULL pointer\n");
        return 0;
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_available - Ring buffer not initialized\n");
        return 0;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_available - Failed to acquire mutex\n");
        return 0;
    }

    size_t avail = rb->available;

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_available - Failed to release mutex\n");
    }

    return avail;
}

size_t ring_buffer_space(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_space - NULL pointer\n");
        return 0;
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_space - Ring buffer not initialized\n");
        return 0;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_space - Failed to acquire mutex\n");
        return 0;
    }

    size_t space = RING_BUFFER_SIZE - rb->available;

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_space - Failed to release mutex\n");
    }

    return space;
}

bool ring_buffer_is_empty(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_is_empty - NULL pointer\n");
        return true; // Treat NULL as empty
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_is_empty - Ring buffer not initialized\n");
        return true;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_is_empty - Failed to acquire mutex\n");
        return true;
    }

    bool empty = (rb->available == 0);

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_is_empty - Failed to release mutex\n");
    }

    return empty;
}

bool ring_buffer_is_full(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_is_full - NULL pointer\n");
        return false; // Treat NULL as not full
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_is_full - Ring buffer not initialized\n");
        return false;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_is_full - Failed to acquire mutex\n");
        return false;
    }

    bool full = (rb->available == RING_BUFFER_SIZE);

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_is_full - Failed to release mutex\n");
    }

    return full;
}

void ring_buffer_reset(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_reset - NULL pointer\n");
        return;
    }

    if (!rb->initialized)
    {
        printf("ERROR: ring_buffer_reset - Ring buffer not initialized\n");
        return;
    }

    if (!MUTEX_LOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_reset - Failed to acquire mutex\n");
        return;
    }

    rb->write_pos = 0;
    rb->read_pos = 0;
    rb->available = 0;

    if (!MUTEX_UNLOCK(rb->mutex))
    {
        printf("ERROR: ring_buffer_reset - Failed to release mutex\n");
    }
}

void ring_buffer_deinit(ring_buffer_t *rb)
{
    if (rb == NULL)
    {
        printf("ERROR: ring_buffer_deinit - NULL pointer\n");
        return;
    }

    if (!rb->initialized)
    {
        printf("WARNING: ring_buffer_deinit - Ring buffer not initialized\n");
        return;
    }

    if (!MUTEX_DESTROY(rb->mutex))
    {
        printf("ERROR: ring_buffer_deinit - Failed to destroy mutex\n");
    }

    rb->initialized = false;
}