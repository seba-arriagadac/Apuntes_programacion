
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

#define MAX_BURST 10
#define FILL_RATE 1

int bucketLevel=MAX_BURST;
int BUCKET_SIZE= MAX_BURST;
int PACKET_SIZE=1; /* one char per "packet" going to the screen */
int fillingRate= FILL_RATE; 

pthread_mutex_t bucketLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bucketChange = PTHREAD_COND_INITIALIZER; 

sigset_t mask;

void * fillBucket(void * arg) /* llena el balde a una tasa constante 
                              /* mientras el balde no este lleno*/  
{
  struct itimerval timerval;
  struct timeval period;
  int signo;
  
  period.tv_sec=1;
  period.tv_usec=0;
  timerval.it_interval=timerval.it_value=period;
  
  setitimer(ITIMER_REAL, & timerval, NULL);
  while (1) {
    signo=SIGINT;  /* anything different from SIGALRM */
    while(signo!=SIGALRM)//espera hasta q llegue sigalarm
       sigwait(&mask, &signo); /* to wait for SIGALRM in a thread */
    pthread_mutex_lock(&bucketLock);
    if (bucketLevel+fillingRate <= BUCKET_SIZE)// si no esta lleno, le hecha
      bucketLevel+=fillingRate;
    pthread_mutex_unlock(&bucketLock);//Se desbloque
    pthread_cond_signal(&bucketChange);//Se avisa del cambio
  }
}

int main(int argc, char * argv[])
{
  pthread_t tid;
  char c;
  int err;
    /* this is to handle signals in a thread. */
  sigemptyset(&mask);   /* Se resetea la mascara  */
  sigaddset(&mask,SIGALRM);  /* include SIGALRM in the set */
  pthread_sigmask(SIG_BLOCK, &mask, NULL);   /* block SIGALRM, so it is not sent 
                                              to this thread */  
  err = pthread_create(&tid, NULL, fillBucket, NULL);
  if (err != 0){
    printf("can't create thread \n");
    exit(0);
  }
  c = getchar();
  while (c!=EOF) {//ctrl+D es un fin de archivo
    pthread_mutex_lock(&bucketLock);
    while ( bucketLevel < PACKET_SIZE) /* while bucket is almost empty */
      pthread_cond_wait(&bucketChange, &bucketLock);
    bucketLevel-=PACKET_SIZE;
    pthread_mutex_unlock(&bucketLock);
    putchar(c); fflush(stdout);
    c = getchar();    
  } 
  exit(0);
}
