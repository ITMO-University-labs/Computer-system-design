#ifndef RING_QUEUE
#define RING_QUEUE

#define MAX_BUFFER 32
typedef struct {
    char elements[MAX_BUFFER];
    int head;
    int tail;
} RingBuffer;

int rb_write(RingBuffer* buf, char element);
int rb_read(RingBuffer* buf, char* dst);

#endif