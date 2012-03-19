
#include "controller.hpp"
#include "thread.hpp"

void 
Controller::run(std::vector<Thread *> &threads) 
{
    m_lineno=1;

    std::vector<Thread *>::iterator it;

    while (!reached_eof()) {
        for (it=threads.begin(); it!=threads.end(); it++)
            (*it)->wakeup();

        while (!reached_line(threads, m_lineno)) {
            boost::mutex::scoped_lock lock(m_mutex);
            m_controller_cond.wait(lock);
        }
        if (has_failure(threads)) {
            printf("got failure(s)\n");
            return;
        }
        m_lineno++;
    }

    for (it=threads.begin(); it!=threads.end(); it++)
        (*it)->wakeup();
}

bool 
Controller::reached_line(std::vector<Thread *> &threads, unsigned line) 
{
    std::vector<Thread *>::iterator it;
    for (it=threads.begin(); it!=threads.end(); it++) {
        printf("    thread %u: line %d, need %u\n", 
            (*it)->get_id(), (*it)->get_lineno(), line);
        if ((*it)->get_lineno()!=line)
            return false;
    }
    return true;
}

bool 
Controller::has_failure(std::vector<Thread *> &threads) 
{
    std::vector<Thread *>::iterator it;
    for (it=threads.begin(); it!=threads.end(); it++) {
        if (!(*it)->success())
            return true;
    }
    return false;
}
