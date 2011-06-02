
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

ham_u64_t 
os::now(void)
{
#ifdef WIN32
	ham_u64_t usec, sec;
    union {
        __int64 ns100; /*time since 1 Jan 1601 in 100ns units */
        FILETIME ft;
    } now;
    GetSystemTimeAsFileTime(&now.ft);
    usec = (long) ((now.ns100 / 10LL) % 1000000LL);
    sec = (long) ((now.ns100 - 116444736000000000LL) / 10000000LL);
	return (ham_u64_t)(sec)*1000+(ham_u64_t)(usec)/1000;
#else
	struct timeval tv;
    gettimeofday(&tv, 0);
    return (ham_u64_t)(tv.tv_sec)*1000+(ham_u64_t)(tv.tv_usec)/1000;
#endif
}
