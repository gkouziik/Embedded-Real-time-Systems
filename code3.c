#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
double n,tempCPU;
#define QUEUESIZE 15
struct timeval prod,cons,stop,startforCPU,endforCPU;
clock_t start_t, end_t, total_t;     //for CPU
FILE *Wififile, *Difftimes,*cputotal;
typedef struct {
  char buf[QUEUESIZE][250];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;
queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q);
void queueDel (queue *q);

void *producer(void *q){
double period,periodafterloss;
  FILE *fp;
  queue *fifo;
  fifo = (queue *)q;
  cputotal = fopen("cputotal.txt","w"); //open file for reading & writing
     if (cputotal ==NULL)
     {
          printf("Failed to open file . \n" );
          exit(1);
     }
     fclose(cputotal);
  while(1){

     gettimeofday(&prod,NULL);
   //  gettimeofday(&start,NULL);

  time_t rawtime;
  char buf[15],path[250];

  struct tm *info;

  time( &rawtime );
  info = localtime(&rawtime);
  strftime(buf,20,"%H:%M:%S",info);




  fp = popen("/bin/sh searchWifi.sh", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }

  while (fgets(path, sizeof(path)-1, fp) != NULL) {
    strtok(path,"\n"); // diavazw apo to arxeio searchWifi  kai xwrizw me '\n'
    strcat(path,"Timestamps="); // meta to onoma mesw ths strcat grafw to "Timestamp" wste na grapsw meta ton xrono
    strcat(path,buf);   //grafw ton xrono pou exw parei pio panw  mesw tou buf
    strcat(path, "\n");
    pthread_mutex_lock (fifo->mut);   //lock wste na grapsei o producer sthn oura
    while (fifo->full) {
      printf ("producer: queue FULL.\n");
      pthread_cond_wait (fifo->notFull, fifo->mut);    //an einai full dld tail==Queuesize wait mexri na adeiasei ena stoixeio
    }
    strcpy(fifo->buf[fifo->tail],path);
    //printf("%sLAALAL\n",fifo->buf[fifo->tail]);   //antegrapse to path ston buffer(fifo->buf) kai mesw ths add proxwra to tail++ gia na tsekareis an exei gemisei h oura

    queueAdd (fifo);           //aplws auksanei to tail kai vlepei an exei ginei iso me queuesize wste na to kanei 0
    pthread_mutex_unlock (fifo->mut);    //unlock grafthke ena stoixeio sthn oura
    pthread_cond_signal (fifo->notEmpty);  //steile shma ston consumer oti mporei na katanalwsei
  }




pclose(fp);
   gettimeofday(&stop,NULL);
   double eltime;
    eltime = (stop.tv_sec-prod.tv_sec)*1000000 +  (stop.tv_usec-prod.tv_usec);

    printf("ELAPSEDTIME:**   %lf\n",eltime);
period = (n * 1000000) - eltime;

printf("PERIOD:**  %lf\n",period);

gettimeofday(&endforCPU,NULL);
end_t = clock();
total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
tempCPU = (endforCPU.tv_sec-startforCPU.tv_sec) +  (endforCPU.tv_usec-startforCPU.tv_usec)/1000000.00;
double totaltime = (tempCPU - total_t)/tempCPU * 100;
printf("TOTAL%lf\n",totaltime );
 cputotal = fopen("cputotal.txt","a"); //open file and append
          if (cputotal ==NULL)
          {
               printf("Failed to open file . \n" );
               exit(1);
          }
          fprintf(cputotal, "Cpu_idle_percent :%.3lf\n",totaltime);
          fclose(cputotal);
          if(period>0){
              usleep(period);}
          else{
             printf("i lost a sample\n");
             periodafterloss = n * 1000000;
             usleep(periodafterloss);

          }      //sleep thn periodo deigmatolhpsia wste o producer na ksypnhsei meta apo n seconds kai na ksanaparagei
}
}




void Wifiwriteargs(char* argum){
  int ishere=0,i=0;
  char tp[15],wifinames[200],var,tempnames[200];
  char *temp;
  FILE *copyFile;
  strcpy(wifinames,argum);
  temp=strtok(wifinames,"\"");
  temp=strtok(NULL,"\"");
  strcpy(wifinames,temp);
  temp =strtok(NULL,"=");
  temp=strtok(NULL,"\n");
  strcpy(tp,temp);
gettimeofday(&cons, NULL);
double dif_times = ( cons.tv_sec - prod.tv_sec ) + ( cons.tv_usec - prod.tv_usec )/1000000.0;

     Difftimes = fopen("Difftimes.txt","a");
     if (Difftimes ==NULL)
     {
          printf("Failed to open file . \n" );
          exit(1);
     }
     fprintf(Difftimes, "time:%lf\n" , dif_times);
     fclose(Difftimes);

  Wififile=fopen("Wififile.txt", "r+");
  if (Wififile ==NULL)
  {
       printf("Failed to open file . \n" );
       exit(1);
  }


  copyFile = fopen("copyFile.txt","w");
  if (copyFile ==NULL)
  {
       printf("Failed to open file . \n" );
       exit(1);
  }
  fclose(copyFile);


  copyFile = fopen("copyFile.txt","r+");

  if (copyFile ==NULL)
  {
       printf("Failed to open file . \n" );
       exit(1);
  }
  while (1)
  {
       var = fgetc(Wififile);
       if (var=='\"')
       {
            i=0;
            memset(tempnames, 0, 200);
            while (1)
            {
                 var = fgetc(Wififile);
                 if (var == '\"')
                      break;
                 tempnames[i] = var;
                 i++;
            }
            putc('\"',copyFile);
            fprintf(copyFile, "%s", tempnames);
            putc('\"',copyFile);
            if (strcmp(wifinames,tempnames)==0)
            {
                 ishere=1;
                 fprintf(copyFile, " %s", tp);
            }
       }else if (var == EOF)
       {
            break;
       }else
       {

            putc(var,copyFile);
       }
  }
  if (ishere == 0)
  {
       putc('\n',copyFile);
       putc('\"',copyFile);
       fprintf(copyFile, "%s", wifinames);
       putc('\"',copyFile);
       fprintf(copyFile, " %s", tp);
  }

  rewind(Wififile);
  rewind(copyFile);
  while (1)
  {
       var = fgetc(copyFile);
       if (var == EOF)
            break;
       else
       {
            putc(var, Wififile);
       }
  }


  fclose(Wififile);
  fclose(copyFile);
}

void *consumer(void *q){
  queue *fifo;
  char temporary[500];

  fifo = (queue *)q;
   Difftimes = fopen("Difftimes.txt","w"); //open file for reading & writing
     if (Difftimes ==NULL)
     {
          printf("Failed to open file . \n" );
          exit(1);
     }
     fclose(Difftimes);


   //printf("Starting of the program, start_t = %ld\n", start_t);
  while(1) {
    pthread_mutex_lock (fifo->mut);
    while (fifo->empty) {
      // printf ("consumer: queue EMPTY.\n");
       pthread_cond_wait (fifo->notEmpty, fifo->mut);
    }
    strcpy(temporary,fifo->buf[fifo->head]);
   // printf ("consumer: recieved %s\n", temporary);

      queueDel (fifo);
      pthread_mutex_unlock (fifo->mut);
      pthread_cond_signal (fifo->notFull);
      Wifiwriteargs(temporary);

  }
}

int main ()
{

  printf("give me the period\n");
  scanf("%lf",&n);


  pthread_t pro, con;

  queue *fifo;
    Wififile=fopen("Wififile.txt", "w");
    start_t = clock();
   gettimeofday(&startforCPU,NULL);
  if (Wififile ==NULL)                         //arxikopoihsh wifi file
  {
       printf("Failed to open file . \n" );
       exit(1);
  }
  fclose(Wififile);

  fifo = queueInit ();
  if (fifo ==  NULL) {
    fprintf (stderr, "main: Queue Init failed.\n");     //arxikopoihsh ouras
    exit (1);
  }

  pthread_create (&pro, NULL, producer, fifo);     // create producer



  pthread_create (&con, NULL, consumer, fifo);    //create consumer

        pthread_join (pro, NULL);               //joinable



  pthread_join (con, NULL);
  /* Clear Queue */
  queueDelete (fifo);

  return 0;
}


queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);

  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);
  free (q->mut);
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

void queueAdd (queue *q)
{
  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel (queue *q)
{

  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}
