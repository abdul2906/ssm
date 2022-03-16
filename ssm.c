#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <X11/Xlib.h>

int main(void);
void xsetroot(char* name);
char* get_time(void);
float get_mem(void);
int* get_cpu_stat(void);
float get_cpu(int* last_stat);

int main(void)
{
    do {
        int* cpu_stat = get_cpu_stat();
        sleep(1); /* Will break if you wait less than one second */
        char name[48];
        char* time = get_time();
        sprintf(name, " [MEM:%.2f%%] [CPU:%.2f%%] [%s] ", 
            get_mem(), get_cpu(cpu_stat), time);
        free(time);
        xsetroot(name);
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

    char* out = malloc(sizeof(char)*32);
    sprintf(out, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return out;
}

float get_mem(void)
{
    FILE *meminfo = fopen("/proc/meminfo", "r");
    char buffer[48];
    float total_ram, free_ram;
    while(fgets(buffer, sizeof(buffer), meminfo)) {
        sscanf(buffer, "MemTotal: %f kB", &total_ram);
        sscanf(buffer, "MemFree: %f kB", &free_ram);
    }
    fclose(meminfo);

    return (total_ram-free_ram)/total_ram*100;
}

int* get_cpu_stat(void)
{
    FILE* stat = fopen("/proc/stat", "r");
    char buffer[64];
    int sum = 0;
    while (fgets(buffer, sizeof(buffer), stat)) {
        int user, nice, system, idle, iowait, irq, softirq, steal, guest;
        sscanf(buffer, "cpu %d %d %d %d %d %d %d %d %d", 
            &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest);
        int _cpu_stat[] = { user, nice, system, idle, iowait, irq, softirq, steal, guest };

        int* cpu_stat = malloc(9*sizeof(int));
        memcpy(cpu_stat, _cpu_stat, 9*sizeof(int));
        fclose(stat);
        return cpu_stat;
    }
    fclose(stat);
}

float get_cpu(int* last_stat)
{
    int* stat = get_cpu_stat();
    float sum, last_sum = 0.0f;
    for (int i = 0; i <= 9; i++) {
        sum += stat[i];
        last_sum += last_stat[i];
    }
    float delta = sum - last_sum;
    float idle = stat[3]-last_stat[3];
    float used = delta - idle;
    free(last_stat);

    return 100*used/delta;
}
