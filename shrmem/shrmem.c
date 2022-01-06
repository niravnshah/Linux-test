#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// void sig_hander(int a)
// {
//     printf("Yes.. got the signal\n");
//     //signal(SIGUSR1, SIG_DFL);
// }

void share( char* str, int pid )
{
    if( pid % 2 ){
        sleep(4);
        printf("Before str=%s\n", str);
        strcpy( str, "ABCD" );
        printf("after str=%s\n", str);
    }
    else {
        sleep(8);
        printf("Before str=%s\n", str);
        strcpy( str+4, "DEFG" );
        printf("After str=%s\n", str);
    }
}

int main()
{
    int is_child, children = 2;
    printf("hello\n");

    // key_t key = ftok("shmfile",65);
    int shmid = shmget(IPC_PRIVATE,1024,0666|IPC_CREAT);
    char *str = (char*) shmat(shmid,(void*)0,0);
    memset(str, 0, 1024);

    int pid[100];
    for( int i = 0; i < children; i++ ) {
        pid[i] = fork();
        if( pid[i] == 0 ) {
            is_child = 1;
            break;
		}
    }

    if( is_child == 1 ) {
        sleep((rand()%10));
        printf("I am child, my id is %d, my parent is %d\n", getpid(), getppid());
        printf("Sending signal to parent\n");
        share(str, getpid());
        //kill( getppid(), SIGUSR1 );
        exit(0);
    }
    else {
        printf("I am parent, my id is %d, my parent is %d\n", getpid(), getppid());
        printf("Awaiting signal from child\n");
        //signal( SIGUSR1, &sig_hander);
    }
    printf("Forking done... waiting\n");

    


    shmdt(str);
    while(1);
    for( int i = 0; i < children; i++ )
        waitpid(pid[i],NULL,0);
    printf("Str=%s",str);
    printf("All accounted for\n");
    return 0;
}