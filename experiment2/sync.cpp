#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

union semun {
    int val;               /* value for SETVAL */
    struct semid_ds* buf;  /* buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* array for GETALL, SETALL */
    struct seminfo* __buf; /* buffer for IPC_INFO */
};

int i = 0, sum = 0;
int semid;  //信号量集合首地址
pthread_t p1, p2, p3;
union semun arg;
void P(int semid, int index);
void V(int semid, int index);
void* thread_caculator(void*);
void* thread_printer_even(void*);
void* thread_printer_odd(void*);

int main() {
    // int key;  //系统建立IPC通讯(消息队列、信号量和共享内存)时指定一个ID值
    // int ret;
    // key = ftok("/tmp", 0x66);
    // if (key < 0) {
    //     perror("ftok key error");
    //     return -1;
    // }

    // semid = semget(key, 2,
    //                IPC_CREAT | 0666);  // create sign;return intenger sign
    //                index
    semid = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);  //创建一个信号灯
    int ret;
    if (semid == -1) {
        perror("create error\n");
        return 0;
    }
    /***对0号信号量设置初始值***/
    arg.val = 1;
    ret = semctl(semid, 0, SETVAL, arg);
    ret = semctl(semid, 1, SETVAL, arg);
    arg.val = 0;
    ret = semctl(semid, 2, SETVAL, arg);
    if (ret < 0) {
        perror("ctl sem error");
        semctl(semid, 0, IPC_RMID, arg);
        return -1;
    }

    ret = pthread_create(&p1, NULL, thread_caculator, NULL);
    if (ret != 0) {
        printf("Create pthread error!\n");
        return -1;
    }

    ret = pthread_create(&p2, NULL, thread_printer_even, NULL);
    if (ret != 0) {
        printf("Create pthread error!\n");
        return -1;
    }

    ret = pthread_create(&p3, NULL, thread_printer_odd, NULL);
    if (ret != 0) {
        printf("Create pthread error!\n");
        return -1;
    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    semctl(semid, 0, IPC_RMID, arg);
    semctl(semid, 1, IPC_RMID, arg);
    semctl(semid, 2, IPC_RMID, arg);
    return 0;
}

void* thread_caculator(void*) {
    for (; i <= 100; i++) {
        P(semid, 0);
        sum += i;
        V(semid, 2);
    }
}

void* thread_printer_even(void*) {
    while (1) {
        P(semid, 2);
        P(semid, 1);
        if (sum % 2 == 0) {
            printf("I am thread_printer_even.\nsum=%d\n", sum);
            V(semid, 1);
            V(semid, 0);
        } else {
            V(semid, 1);
            V(semid, 2);
        }
        if (i == 101) break;
    }
}

void* thread_printer_odd(void*) {
    while (1) {
        P(semid, 2);
        P(semid, 1);
        if (sum % 2 == 1) {
            printf("I am thread_printer_odd.\nsum=%d\n", sum);
            V(semid, 1);
            V(semid, 0);
        } else {
            V(semid, 1);
            V(semid, 2);
        }
        if (i == 101) break;
    }
}

void P(int semid, int index) {
    struct sembuf sem = {(short unsigned int)index, -1, 0};
    semop(semid, &sem, 1);
}

void V(int semid, int index) {
    struct sembuf sem = {(short unsigned int)index, 1, 0};
    semop(semid, &sem, 1);
}