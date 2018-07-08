#define _GNU_SOURCE
#include <endian.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

void loop()
{
        syscall(__NR_mmap, 0x20a00000, 0x600000, 0, 0x66033, -1, 0);
        syscall(__NR_remap_file_pages, 0x20a00000, 0x600000, 0, 0x20000000000000, 0);
}

int main()
{
        syscall(__NR_mmap, 0x20000000, 0x1000000, 3, 0x32, -1, 0);
        loop();
        return 0;
}
