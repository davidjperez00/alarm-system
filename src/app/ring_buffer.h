#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Platform-specific mutex definitions
#ifdef LINUX
// TODO: THIS WILL NEED ADJUSTED AND TESTED ON DIFFERENT PLATFORMS
// Linux/macOS/Unix - use pthreads
#include <pthread.h>
typedef pthread_mutex_t mutex_t;
#define MUTEX_INIT(m) (pthread_mutex_init(&(m), NULL) == 0)
#define MUTEX_LOCK(m) (pthread_mutex_lock(&(m)) == 0)
#define MUTEX_UNLOCK(m) (pthread_mutex_unlock(&(m)) == 0)
#define MUTEX_DESTROY(m) (pthread_mutex_destroy(&(m)) == 0)
#else
// ESP32 - use FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
typedef SemaphoreHandle_t mutex_t;
#define MUTEX_INIT(m) ((m = xSemaphoreCreateMutex()) != NULL)
#define MUTEX_LOCK(m) ((m != NULL) && (xSemaphoreTake(m, portMAX_DELAY) == pdTRUE))
#define MUTEX_UNLOCK(m) ((m != NULL) && (xSemaphoreGive(m) == pdTRUE))
#define MUTEX_DESTROY(m) ((m != NULL) && (vSemaphoreDelete(m), true))
#endif

// Ring buffer structure (opaque to users)
// Users can use this struct but won't be able to access elements of it.
typedef struct ring_buffer_t ring_buffer_t;

/**
 * @brief Initialize a ring buffer
 * @param rb Pointer to ring buffer structure
 * @return true if successful, false on error
 */
bool ring_buffer_init(ring_buffer_t *rb);

/**
 * @brief Write samples to ring buffer
 * @param rb Pointer to ring buffer
 * @param data Pointer to data to write
 * @param count Number of samples to write
 * @return Number of samples actually written
 */
size_t ring_buffer_write(ring_buffer_t *rb, const int16_t *data, size_t count);

/**
 * @brief Read samples from ring buffer
 * @param rb Pointer to ring buffer
 * @param data Pointer to buffer to read into
 * @param count Number of samples to read
 * @return Number of samples actually read
 */
size_t ring_buffer_read(ring_buffer_t *rb, int16_t *data, size_t count);

/**
 * @brief Get number of samples available to read
 * @param rb Pointer to ring buffer
 * @return Number of samples available
 */
size_t ring_buffer_available(ring_buffer_t *rb);

/**
 * @brief Get amount of free space in ring buffer
 * @param rb Pointer to ring buffer
 * @return Number of samples that can be written
 */
size_t ring_buffer_space(ring_buffer_t *rb);

/**
 * @brief Check if ring buffer is empty
 * @param rb Pointer to ring buffer
 * @return true if empty, false otherwise
 */
bool ring_buffer_is_empty(ring_buffer_t *rb);

/**
 * @brief Check if ring buffer is full
 * @param rb Pointer to ring buffer
 * @return true if full, false otherwise
 */
bool ring_buffer_is_full(ring_buffer_t *rb);

/**
 * @brief Reset ring buffer to empty state
 * @param rb Pointer to ring buffer
 */
void ring_buffer_reset(ring_buffer_t *rb);

/**
 * @brief Cleanup ring buffer resources
 * @param rb Pointer to ring buffer
 */
void ring_buffer_deinit(ring_buffer_t *rb);

#endif // RING_BUFFER_H