#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

int selled;  //已售票
int total;   //总数
int semid;   //信号灯id
void* thread1(void*);
void* thread2(void*);
void* thread3(void*);
void P(int semid, int index);
void V(int semid, int index);

union semun {
    int val;               /* value for SETVAL */
    struct semid_ds* buf;  /* buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* array for GETALL, SETALL */
    struct seminfo* __buf; /* buffer for IPC_INFO */
};
union semun arg;

int main(void) {
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);  //创建一个信号灯
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);
    selled = 0;
    total = 15;
    pthread_t p1, p2, p3;  //子线程id
    int ret1, ret2, ret3;

    ret1 = pthread_create(&p1, NULL, thread1, NULL);
    ret2 = pthread_create(&p2, NULL, thread2, NULL);
    ret3 = pthread_create(&p3, NULL, thread3, NULL);

    if (ret1 != 0 || ret2 != 0 || ret3 != 0) {
        printf("thread created failed");
        exit(0);
    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);
    printf("%d tickets are totally sold\n", selled);
    semctl(semid, 0, IPC_RMID, arg);
    return 0;
}

void* thread1(void*) {
    printf("thread 1 is created\n");
    int sell1 = 0;            //线程1卖的票数
    while (selled < total) {  //开始卖票
        P(semid, 0);          //访问信号灯
        sell1++;
        selled++;
        printf("thread 1 sells %d tickets \n", sell1);
        V(semid, 0);
        sleep(1);
    }
    printf("all tickets are sold\n");
    printf("thread 1 has sold %d tickets\n", sell1);
    printf("thread 1 exited\n");
    pthread_exit(0);
}

void* thread2(void*) {
    printf("thread 2 is created\n");
    int sell2 = 0;            //线程2卖的票数
    while (selled < total) {  //开始卖票
        P(semid, 0);          //访问信号灯
        sell2++;
        selled++;
        printf("thread 2 sells %d tickets \n", sell2);
        V(semid, 0);
        sleep(2);
    }
    printf("all tickets are sold\n");
    printf("thread 2 has sold %d tickets\n", sell2);
    printf("thread 2 exited\n");
    pthread_exit(0);
}

void* thread3(void*) {
    printf("thread 3 is created\n");
    int sell3 = 0;            //线程3卖的票数
    while (selled < total) {  //开始卖票
        P(semid, 0);          //访问信号灯
        sell3++;
        selled++;
        printf("thread 3 sells %d tickets \n", sell3);
        V(semid, 0);
        sleep(3);
    }
    printf("all tickets are sold\n");
    printf("thread 3 has sold %d tickets\n", sell3);
    printf("thread 3 exited\n");
    pthread_exit(0);
}

void P(int semid, int index) {
    struct sembuf sem = {(short unsigned int)index, -1, 0};
    semop(semid, &sem, 1);
}

void V(int semid, int index) {
    struct sembuf sem = {(short unsigned int)index, 1, 0};
    semop(semid, &sem, 1);
}
