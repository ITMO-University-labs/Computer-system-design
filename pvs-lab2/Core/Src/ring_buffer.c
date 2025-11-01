#include "ring_buffer.h"

int rb_write(RingBuffer* buf, char element) {
    buf->elements[buf->head] = element;
    int next_head = (buf->head + 1) % MAX_BUFFER;

    if (next_head != buf->tail) {
        buf->head = next_head;
        return 1;
    }
    return 0;
}

int rb_read(RingBuffer* buf, char* dst) {
    if (buf->tail != buf->head) {
        *dst = buf->elements[buf->tail];
        buf->tail = (buf->tail + 1) % MAX_BUFFER;\
        return 1;
    }
    return 0;
}
