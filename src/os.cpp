
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "porting.hpp"
#include "os.hpp"


bool 
os::unlink(const char *path)
{
#ifdef WIN32
    return (0!=DeleteFile(path));
#else
    return (0==::unlink(path));
#endif
}

unsigned long 
os::get_filesize(const char *path)
{
    struct stat buf;
    if (stat(path, &buf)<0)
        return (0);
    return ((unsigned long)buf.st_size);
}
