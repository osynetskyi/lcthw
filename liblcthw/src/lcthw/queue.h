#ifndef _lcthw_Queue_h
#define _lcthw_Queue_h

#include <lcthw/list.h>

typedef List Queue;

#define QUEUE_FOREACH(L, V) LIST_FOREACH(L, last, prev, V)

#define Queue_create List_create

#define Queue_destroy List_destroy

#define Queue_count List_count

#define Queue_peek List_first

#define Queue_send List_push

#define Queue_recv List_shift

#endif
