#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <X11/Xlib.h>

#define BUFFER_LENGTH 256

int main(void);
void xsetroot(char* name);
char* get_time(void);
float get_mem(void);
float get_cpu(void);

int main(void)
{
    do {
        char name[256];
        sprintf(name, " [MEM:%.2f%%] [CPU:%.2f%%] [%s] ", 
            get_mem(), get_cpu(), get_time());
        xsetroot(name);
        sleep(1);
    } while (1);
}

void xsetroot(char* name)
{
    Display *display;
    if ((display = XOpenDisplay(0x0)) == NULL) {
        printf("Cannot open display");
        exit(1);
    }
    
    XStoreName(display, DefaultRootWindow(display), name);
    XSync(display, 0);

    XCloseDisplay(display);
}

char* get_time(void)
{
    time_t _time = time(NULL);
    struct tm tm = *localtime(&_time);

    char* out = malloc(sizeof(char)*BUFFER_LENGTH);
    sprintf(out, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return out;
}

/* 
* TODO: Migrate to using sysinfo.h for this
*/

float get_mem(void)
{
    FILE *meminfo = fopen("/proc/meminfo", "r");
    char buffer[BUFFER_LENGTH];
    float total_ram, free_ram;
    while(fgets(buffer, sizeof(buffer), meminfo)) {
        sscanf(buffer, "MemTotal: %f kB", &total_ram);
        sscanf(buffer, "MemFree: %f kB", &free_ram);
    }
    fclose(meminfo);

    return (total_ram-free_ram)/total_ram*100;
}


float get_cpu(void)
{
    FILE *loadavg = fopen("/proc/loadavg", "r");
    char buffer[BUFFER_LENGTH];
    float cpu_avg;
    while(fgets(buffer, sizeof(buffer), loadavg)) 
        sscanf(buffer, "%f", &cpu_avg);

    return cpu_avg*100 / get_nprocs_conf();
}

