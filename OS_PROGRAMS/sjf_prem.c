#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct {
    int id;
    int arrival_time;
    int burst_time;
    int rt;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    bool done;
    int st;
} Process;

typedef struct {
    int id;
    int st;
    int et;
} Gantt;


void main() {
    int n = 3;
    Process processes[3] = {
        {1, 0, 8, 8, 0, 0, 0, false, 0},
        {2, 1, 4, 4, 0, 0, 0, false, 0},
        {3, 2, 2, 2, 0, 0, 0, false, 0}
    };

    Gantt gantt[20];
    int gt = 0;

    for (int i = 0; i<n; i++) {
        for (int j = i+1; j<n; j++) {
            if (processes[i].burst_time > processes[j].burst_time) {
                Process temp = processes[i];
                processes[i] = processes[j];
                processes[j] = temp;
            }
        }
    }

    int ct = 0;
    int done = 0;

    while (done != n) {
        bool found = false;
        for (int i = 0; i<n; i++) {
            if (!processes[i].done && processes[i].arrival_time <= ct) {
                printf("choosen %d for %d\n", processes[i].id, ct);
                found = true;
                processes[i].rt = processes[i].rt - 1;
                if (processes[i].rt == 0) {
                    processes[i].done = true;
                    processes[i].completion_time = ct + 1;
                    done++;
                }

                gantt[ct].id = processes[i].id;
                gantt[ct].st = ct;
                gantt[ct].et = ct + 1;
                gt++;
            }

            if (found) {
                break;
            }
        }
        ct++;
    }

    for (int i = 0; i<n; i++) {
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
    }

    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\tTurnaround Time\tWaiting Time\n");
    for (int i = 0; i<n; i++) {
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].arrival_time, processes[i].burst_time, processes[i].completion_time, processes[i].turnaround_time, processes[i].waiting_time);
    }

    printf("Gantt Chart\n");
    printf("|");
    for (int i = 0; i<gt; i++) {
        printf("  P%d  |", gantt[i].id);
    }
    printf("\n");
    for (int i = 0; i<gt; i++) {
        if (gantt[i].st < 10) {
            printf("%d      ", gantt[i].st);
        } else {
            printf("%d     ", gantt[i].st);
        }
    }
    printf("%d\n", ct);
    printf("\n");
}
