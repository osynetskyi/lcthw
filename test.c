#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/syscall.h>

int memfd_create(const char *name, unsigned int flags) {
    return syscall(__NR_memfd_create, name, flags);
}

int main(int argc, char *argv[])
{
    char *buf = NULL;
    void *address = NULL;
    size_t size = 4*1024;
    int fd = memfd_create("circular_buffer", 0);
    ftruncate(fd, size);

    buf = mmap(NULL, size * 2, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    address = mmap(buf, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if (address != buf)
	printf("error at 1st mmap\n");
    printf("buf: %p, buf+size: %p\n", buf, buf+size);
    address = mmap(buf + size, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    if (address != buf + size) {
	printf("error at 2nd mmap: should be %p, actually %p. Errno: %s\n", buf+size, address, strerror(errno));
    }

    close(fd);

    buf[size] = 'b';
    buf[0] = 'a';
    printf("original: %c, mirrored: %c\n", buf[0], buf[size]);

    //close(fd);

    return 0;
}
