#ifndef _lcthw_Cbuffer_h
#define _lcthw_Cbuffer_h

typedef struct {
    char *data;
    int length;
    int start;
    int end;
} CBuffer;

CBuffer *CBuffer_create(int length);

void CBuffer_destroy(CBuffer * cb);

int CBuffer_write(CBuffer * cb, char *data, int length);

int CBuffer_read(CBuffer * cb, char *target, int length);

int CBuffer_empty(CBuffer * cb);

int CBuffer_data_available(CBuffer * cb);

int CBuffer_space_available(CBuffer * cb);

void CBuffer_clear(CBuffer * cb);

#endif
