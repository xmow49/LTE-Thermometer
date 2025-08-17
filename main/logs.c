#include <stdio.h>
#include <string.h>

#include "logs.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#define LOGS_BUFFER_SIZE (64 * 1024)

static char *logs_buffer = NULL;
static int logs_write_index = 0;
static int logs_read_index = 0;
static bool logs_buffer_full = false;

static int logs_vprintf(const char *fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);

    // Calculate remaining space
    int remaining_space = LOGS_BUFFER_SIZE - logs_write_index;

    // If there's not enough space for a typical message, wrap around
    if (remaining_space < 256)
    {
        logs_write_index = 0;
        logs_buffer_full = true;
    }

    int written = vsnprintf(logs_buffer + logs_write_index, LOGS_BUFFER_SIZE - logs_write_index, fmt, args_copy);
    if (written > 0)
    {
        logs_write_index += written;

        // If the message filled the buffer, wrap around
        if (logs_write_index >= LOGS_BUFFER_SIZE)
        {
            logs_write_index = 0;
            logs_buffer_full = true;
        }

        // If buffer is full and write index overtakes read index, move read index
        if (logs_buffer_full && logs_write_index >= logs_read_index)
        {
            // Find the next newline to maintain complete log lines
            char *next_newline = strchr(logs_buffer + logs_read_index, '\n');
            if (next_newline)
            {
                logs_read_index = (next_newline - logs_buffer) + 1;
            }
            else
            {
                logs_read_index = logs_write_index;
            }
        }
    }
    va_end(args_copy);
    return vprintf(fmt, args);
}

void logs_init()
{
    logs_buffer = heap_caps_malloc(LOGS_BUFFER_SIZE, MALLOC_CAP_SPIRAM);
    if (!logs_buffer)
    {
        ESP_LOGE("logs", "Failed to allocate logs buffer");
        return;
    }
    logs_write_index = 0;
    logs_read_index = 0;
    logs_buffer_full = false;

    esp_log_set_vprintf(logs_vprintf);
}

const char *logs_get_buffer()
{
    return logs_buffer;
}

int logs_get_available_bytes()
{
    if (!logs_buffer_full)
    {
        return logs_write_index - logs_read_index;
    }
    else if (logs_write_index > logs_read_index)
    {
        return logs_write_index - logs_read_index;
    }
    else
    {
        return (LOGS_BUFFER_SIZE - logs_read_index) + logs_write_index;
    }
}

int logs_read(char *dst, int max_bytes)
{
    if (logs_read_index == logs_write_index && !logs_buffer_full)
    {
        // Buffer is empty
        return 0;
    }

    int bytes_to_read = logs_get_available_bytes();
    if (bytes_to_read > max_bytes)
    {
        bytes_to_read = max_bytes;
    }

    int bytes_read = 0;

    // If the read would wrap around the buffer end
    if (logs_read_index + bytes_to_read > LOGS_BUFFER_SIZE)
    {
        // Read until the end of the buffer
        int first_chunk = LOGS_BUFFER_SIZE - logs_read_index;
        memcpy(dst, logs_buffer + logs_read_index, first_chunk);
        bytes_read += first_chunk;

        // Read the remaining from the beginning
        int second_chunk = bytes_to_read - first_chunk;
        memcpy(dst + first_chunk, logs_buffer, second_chunk);
        bytes_read += second_chunk;

        logs_read_index = second_chunk;
    }
    else
    {
        // Simple case - no wrapping
        memcpy(dst, logs_buffer + logs_read_index, bytes_to_read);
        bytes_read = bytes_to_read;

        logs_read_index += bytes_to_read;
        if (logs_read_index >= LOGS_BUFFER_SIZE)
        {
            logs_read_index = 0;
        }
    }

    // If we've read everything, reset the full flag
    if (logs_read_index == logs_write_index)
    {
        logs_buffer_full = false;
    }

    return bytes_read;
}

void logs_reset_read_index()
{
    logs_read_index = 0;
}

int logs_get_write_index()
{
    return logs_write_index;
}

int logs_get_read_index()
{
    return logs_read_index;
}

bool logs_is_buffer_full()
{
    return logs_buffer_full;
}
