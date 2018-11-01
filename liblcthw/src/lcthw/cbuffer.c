#include <lcthw/cbuffer.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include "dbg.h"
#include <sys/syscall.h>
#include <string.h>

#define PAGE_SIZE 4096

int memfd_create(const char *name, int flags)
{
    return syscall(__NR_memfd_create, name, flags);
}

CBuffer *CBuffer_create(int length)
{
    int fd = 0;
    check(length % PAGE_SIZE == 0, "Length of CBuffer should be aligned with \
	    memory page size, which is %d", PAGE_SIZE);
    void *address = NULL;
    int rc = 0;

    CBuffer *buffer = calloc(1, sizeof(CBuffer));
    buffer->start = 0;
    buffer->end = 0;
    buffer->length = length;
    //buffer->data = calloc(1, buffer->length); 

    fd = memfd_create("circular_buffer", 0);
    check(fd >= 0, "Failed to create an anonymous file.");
    rc = ftruncate(fd, length);
    check(!rc, "Failed to truncate file.");

    buffer->data = mmap(NULL, length * 2, PROT_NONE, MAP_PRIVATE | 
	    MAP_ANONYMOUS, -1, 0);
    check(buffer->data, "Failed to mmap buffer->data");

    address = mmap(buffer->data, length, PROT_READ | PROT_WRITE, MAP_SHARED | 
	    MAP_FIXED, fd, 0);
    check(address == buffer->data, "Failed to mmap 1st half.");

    address = mmap(buffer->data + length, length, PROT_READ | PROT_WRITE, 
	    MAP_SHARED | MAP_FIXED, fd, 0);
    check(address == buffer->data + length, "Failed to mmap 2nd half.");

    close(fd);
    return buffer;
error:
    if (fd)
        close(fd);
    return NULL;
}

void CBuffer_destroy(CBuffer * cb)
{
    if (cb) {
        munmap(cb->data, cb->length * 2);
	free(cb);
    }
}

int CBuffer_data_available(CBuffer * cb)
{
    return cb->end - cb->start;
}

int CBuffer_space_available(CBuffer * cb)
{
    return cb->length - CBuffer_data_available(cb);
}

int CBuffer_empty(CBuffer * cb)
{
    return cb->start == cb->end;
}

void CBuffer_clear(CBuffer * cb)
{
    cb->start = cb->end = 0;
}

int CBuffer_write(CBuffer * cb, char *data, int length)
{
    check(length < CBuffer_space_available(cb), "Not enought space: requested \
		%d, available %d", length, CBuffer_space_available(cb));

    void *res = memcpy(cb->data + cb->end, data, length);
    check(res == cb->data + cb->end, "Error in memcpy on writing.");
    
    cb->end += length;
    if (cb->end >= cb->length) {
        cb->end -= cb->length;
    }

    return length;
error:
    return -1;
}

int CBuffer_read(CBuffer * cb, char *target, int length)
{
    check(length <= CBuffer_data_available(cb), "Not enough data: requested \
		%d, available %d", length, CBuffer_data_available(cb));

    void *res = memcpy(target, cb->data + cb->start, length);
    check(res == target, "Error in memcpy on reading.");

    cb->start += length;
    if (cb->start >= cb->length) {
        cb->start -= cb->length;
    }

    return length;
error:
    return -1;
}
