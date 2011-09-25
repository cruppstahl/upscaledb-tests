
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

const char *
os::hostname(void)
{
    static char hn[1024];
    static bool init=false;
    if (!init) {
        gethostname(&hn[0], sizeof(hn));
        init=true;
    }
    return (&hn[0]);
}

const char *
os::architecture(void)
{
    static utsname buf;
    static char arch[1024];
    static bool init=false;
    if (!init) {
        uname(&buf);
        init=true;
        sprintf(arch, "%s %s %s", 
                buf.sysname, 
                buf.release, 
                buf.machine);
    }
    return (&arch[0]);
}

