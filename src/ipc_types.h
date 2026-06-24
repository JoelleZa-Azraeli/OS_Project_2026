#ifndef IPC_TYPES_H
#define IPC_TYPES_H

#include <sys/types.h>

#define MSG_ARRIVED  0
#define MSG_FINISHED 1
#define MSG_WAITING  2

typedef struct {
    pid_t pid;
    int   msg_type;
    int   node;
    int   next_node;
} TravelerMsg;

#endif
