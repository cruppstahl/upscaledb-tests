
#include "porting.hpp"
#include "config.hpp"
#include "database.hpp"
#include "metrics.hpp"

void 
database::collect_metrics()
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
        f=(float)m_profile[i];
        total+=f;
        f*=1000000.f;
        Metrics::get_instance()->add_metric(get_id(), s[i], (unsigned long)f);
    }

    total*=1000000.f;
    Metrics::get_instance()->add_metric(get_id(), "perf-total", 
            (unsigned long)total);
}

