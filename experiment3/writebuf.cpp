#include <pthread.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#define N 10

const char* input_addr = "input.png";

union semun {
    int val;               /* value for SETVAL */
    struct semid_ds* buf;  /* buffer for IPC_STAT, IPC_SET */
    unsigned short* array; /* array for GETALL, SETALL */
    struct seminfo* __buf; /* buffer for IPC_INFO */
};
struct shared_use_ds {
    int start;  // read,start指向待读的
    int end;    // write,end指向待写区
    int length;
    char text[N];  //环形缓冲区
};

void P(int semid, int index) {
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = -1;
    sem.sem_flg = 0;        //操作标记：0或IPC_NOWAIT等
    semop(semid, &sem, 1);  // 1:表示执行命令的个数
    return;
}
void V(int semid, int index) {
    struct sembuf sem;
    sem.sem_num = index;
    sem.sem_op = 1;
    sem.sem_flg = 0;
    semop(semid, &sem, 1);
    return;
}
union semun arg;
struct sembuf sem;

int main() {
    int semid;
    if ((semid = semget((key_t)5678, 2, IPC_CREAT | 0666)) ==
        -1) {  //获取信号量
        printf("writebuf.cpp semget error!\n");
        exit(2);
    }

    int shmid = shmget((key_t)1234, sizeof(struct shared_use_ds),
                       0666 | IPC_CREAT);  //获取共享内存
    if (shmid == -1) {
        printf("shmget error!\n");
        exit(2);
    }

    struct shared_use_ds* write_addr = NULL;
    write_addr = (struct shared_use_ds*)shmat(shmid, 0, 0);

    FILE* fpr = NULL;
    fpr = fopen(input_addr, "rb+");
    if (fpr == NULL) {
        printf("fopen input.png error!\n");
        exit(2);
    }

    char ch;
    int put = 0;
    while (!feof(fpr)) {
        fread(&ch, sizeof(char), 1, fpr);
        P(semid, 1);  //缓冲区空余位置数
        write_addr->text[write_addr->end] = ch;
        write_addr->end = (write_addr->end + 1) % N;
        write_addr->length = -1;
        V(semid, 0);  //缓冲区已写入数
        put++;
        if (put % 10 == 0) {
            printf("PUT: have put %d bytes to buf\n", put);
        }
    }
    write_addr->length = put;
    fclose(fpr);
    printf("success read input.png!\n");
    return 0;
}