#ifndef LOGS_H
#define LOGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize the logging system with a circular buffer
     */
    void logs_init();

    /**
     * @brief Get a pointer to the raw log buffer
     * @return Pointer to the buffer
     */
    const char *logs_get_buffer();

    /**
     * @brief Get the number of bytes available to read
     * @return Number of bytes available
     */
    int logs_get_available_bytes();

    /**
     * @brief Read a specified number of bytes from the log buffer
     * @param dst Destination buffer to copy logs into
     * @param max_bytes Maximum number of bytes to read
     * @return Number of bytes actually read
     */
    int logs_read(char *dst, int max_bytes);

    /**
     * @brief Reset the read index to the beginning of the buffer
     */
    void logs_reset_read_index();

    /**
     * @brief Get the current write index
     * @return Current write index
     */
    int logs_get_write_index();

    /**
     * @brief Get the current read index
     * @return Current read index
     */
    int logs_get_read_index();

    /**
     * @brief Check if the buffer is full
     * @return true if the buffer has been filled at least once
     */
    bool logs_is_buffer_full();

#ifdef __cplusplus
}
#endif

#endif // LOGS_H
