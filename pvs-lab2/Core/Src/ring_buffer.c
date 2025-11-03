#include "ring_buffer.h"
#include "string.h"

int rb_write(RingBuffer* buf, char element) {
    buf->elements[buf->head] = element;
    int next_head = (buf->head + 1) % MAX_BUFFER;

    if (next_head != buf->tail) {
        buf->head = next_head;
        return 1;
    }
    return 0;
}

int rb_free(RingBuffer* rb) {
    return rb->head >= rb->tail ? MAX_BUFFER - rb->head + rb->tail : rb->tail - rb->head - 1;
}

int rb_write_array(RingBuffer* rb, char* array, int len) {
  if (rb_free(rb) <= len) return 0;
  
  for (int i = 0; i < len; i++)
    rb_write(rb, array[i]);

  return 1;
}

int rb_read(RingBuffer* buf, char* dst) {
    if (buf->tail != buf->head) {
        *dst = buf->elements[buf->tail];
        buf->tail = (buf->tail + 1) % MAX_BUFFER;\
        return 1;
    }
    return 0;
}
