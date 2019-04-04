#ifndef _INCLUDE_TIME_MEASURE_H
#define _INCLUDE_TIME_MEASURE_H

#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
namespace  cds_utils {
    double getTime(void) {

        double usertime;
        struct rusage usage;

        getrusage(RUSAGE_SELF, &usage);

        usertime = (double) usage.ru_utime.tv_sec + //(double)usage.ru_stime.tv_sec +
                (double) usage.ru_utime.tv_usec / 1000000.0;// + (double) usage.ru_stime.tv_usec / 1000000.0;

        return (usertime);
    }
}
#endif