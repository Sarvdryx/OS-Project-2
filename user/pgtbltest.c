#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ITERS 1000000 // Number of iterations for the benchmark

void benchmark_ugetpid(void) {
    int start, end;
    int time_syscall, time_usyscall;
    
    // Variables to store the results and prevent compiler optimization
    int pid_sys = 0;
    int pid_usys = 0;

    printf("\n==================================================\n");
    printf("       UGETPID PERFORMANCE BENCHMARK              \n");
    printf("==================================================\n");
    
    // Benchmark 1: Traditional getpid() (Traps into kernel)
    printf("[Test 1] Running traditional getpid() %d times...\n", ITERS);
    start = uptime();
    for (int i = 0; i < ITERS; i++) {
        pid_sys = getpid(); 
    }
    end = uptime();
    time_syscall = end - start;
    printf(" -> getpid() completion time: %d ticks (~%d ms)\n", time_syscall, time_syscall * 100);

    // Benchmark 2: ugetpid() (Reads from shared memory, no trap)
    printf("\n[Test 2] Running ugetpid() via shared memory %d times...\n", ITERS);
    start = uptime();
    for (int i = 0; i < ITERS; i++) {
        pid_usys = ugetpid(); 
    }
    end = uptime();
    time_usyscall = end - start;
    printf(" -> ugetpid() completion time: %d ticks (~%d ms)\n", time_usyscall, time_usyscall * 100);

    printf("--------------------------------------------------\n");
    
    // Correctness check
    if (pid_sys != pid_usys) {
        printf("[Error] ugetpid() returned incorrect PID! (sys: %d, usys: %d)\n", pid_sys, pid_usys);
    } else {
        printf("[Check Correctness] PASS! Both returned PID: %d\n", pid_sys);
    }

    printf("--------------------------------------------------\n");
    printf("[Result] SPEED COMPARISON:\n");
    
    // Calculate and prove the performance benefit via tracing results
    if (time_usyscall > 0) {
        int speedup = time_syscall / time_usyscall;
        printf(" -> ugetpid() is approximately %d times FASTER than getpid()!\n", speedup);
    } else if (time_usyscall == 0 && time_syscall > 0) {
        printf(" -> ugetpid() took 0 ticks, significantly faster than getpid() (%d ticks)!\n", time_syscall);
    } else {
        printf(" -> Test duration too short or QEMU is too fast. Consider increasing ITERS.\n");
    }
    printf("==================================================\n\n");
}

int main(int argc, char *argv[]) {
    benchmark_ugetpid();
    exit(0);
}