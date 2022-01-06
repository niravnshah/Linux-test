#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

pthread_mutex_t* mutex_fuel;
pthread_cond_t* cond_fuel;
int * fuel;


void* fuel_fill(void* arg) {
    int* fuel = (int*) arg;
    for (int i = 0; i < 15; i++) {
        pthread_mutex_lock(mutex_fuel);
        *fuel += 65;
        printf("Filling fuel... %d\n", *fuel);
        pthread_mutex_unlock(mutex_fuel);
        pthread_cond_broadcast(cond_fuel);
        sleep(5);
    }
    exit(0);
}

void* car(void* arg) {
    printf("Here to get fuel\n");
    int* fuel = (int*) arg;

    pthread_mutex_lock(mutex_fuel);
    while (*fuel < 40) {
        printf("No fuel, waiting.. \n");
        pthread_cond_wait(cond_fuel, mutex_fuel);
        // sleep(1);
    }
    *fuel -= 40;
    printf("Got fuel. Remaining fuel... %d\n", *fuel);
    pthread_mutex_unlock(mutex_fuel);
    exit(0);
}



int main(int argc, char* argv[])
{
    int const threads = 5;
    pthread_t th[threads];
    int pid[threads];
    int shmid_int = shmget(IPC_PRIVATE,1024,0666|IPC_CREAT);
    int shmid_mutex = shmget(IPC_PRIVATE,1024,0666|IPC_CREAT);
    int shmid_cond = shmget(IPC_PRIVATE,1024,0666|IPC_CREAT);
    fuel = (int*) shmat(shmid_int, (void*)0, 0);
    mutex_fuel = (pthread_mutex_t*) shmat(shmid_mutex, (void*)0, 0);
    cond_fuel = (pthread_cond_t*) shmat(shmid_cond, (void*)0, 0);

    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;

    pthread_mutexattr_init(&mutex_attr);
    pthread_condattr_init(&cond_attr);

    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    int i;

    pthread_mutex_init(mutex_fuel, &mutex_attr);
    pthread_cond_init(cond_fuel, &cond_attr);
    
    for (i = 0; i < threads; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            if (i == 0) {
                fuel_fill((void*)fuel);
                // if (pthread_create(&th[i], NULL, fuel_fill, fuel) != 0) {
                //     perror("Failed to create fuel thread\n");
                // }
            } else {
                car((void*)fuel);
                // if (pthread_create(&th[i], NULL, car, fuel) != 0) {
                //     perror("Failed to create car thread\n");
                // }
            }
        }
    }

    for (i = 0; i < threads; i++) {
        waitpid(pid[i],NULL,0);
        // if (pthread_join(th[i], NULL) != 0) {
        //     perror("Failing to join the thread\n");
        // }
    }

    shmdt(fuel);

    pthread_mutexattr_destroy(&mutex_attr);
    pthread_condattr_destroy(&cond_attr);
    pthread_cond_destroy(cond_fuel);
    pthread_mutex_destroy(mutex_fuel);
    return 0;
}