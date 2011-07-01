
#include "porting.hpp"
#include "config.hpp"
#include "database.hpp"

void 
database::print_profile(database *db)
{
    static const char *s[]={
        "perf-misc  \t", 
        "perf-insert\t", 
        "perf-erase \t", 
        "perf-find  \t", 
        "perf-cursor\t"
    };

    float f, total=0;

    for (int i=0; i<5; i++) {
        f=(float)db->m_profile[i];
        total+=f;
        f/=1000.f;
        printf("\t%s\t%f sec\n", s[i], f);
    }

    total/=1000.f;
    printf("\tperf-total\t\t%f sec\n", total);
    db->print_specific_profile();
}

const char *
database::get_name(void)
{
    return ("database");
}
