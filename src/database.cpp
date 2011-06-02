
#include "porting.hpp"
#include "config.hpp"
#include "database.hpp"


void 
database::print_profile(void)
{
    if (!m_config->profile)
        return;

    static const char *s[]={
        "misc  ", 
        "insert", 
        "erase ", 
        "find  ", 
        "cursor"
    };

    float f, total=0;

    if (m_config->output_xml)
        printf("\t\t<performance backend=\"%s\">\n", get_name());

    for (int i=0; i<5; i++) {
        f=(float)m_profile[i];
        total+=f;
        f/=1000.f;
        if (m_config->output_xml) {
            printf("\t\t\t<parameter>\n");
            printf("\t\t\t\t<name>%s</name>\n", s[i]);
            printf("\t\t\t\t<value>%f</value>\n", f);
            printf("\t\t\t</parameter>\n");
        }
        else
            printf("%s: profile of backend %s:\t%f sec\n", s[i], get_name(), f);
    }

    total/=1000.f;
    if (m_config->output_xml) {
        printf("\t\t\t<parameter>\n");
        printf("\t\t\t\t<name>total</name>\n");
        printf("\t\t\t\t<value>%f</value>\n", total);
        printf("\t\t\t</parameter>\n");
        printf("\t\t</performance>\n");
    }
    else
        printf("total : profile of backend %s:\t%f sec\n\n", get_name(), total);
}

const char *
database::get_name(void)
{
    return ("database");
}
