#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
double *timestamp;
int count;
FILE *fp;
struct timeval startingt,endingt;
//int sigpause(int mask);


void myhandler(int signum){
  double eltime;  //elapsed time

  gettimeofday(&endingt,NULL);
  eltime = (endingt.tv_sec-startingt.tv_sec) +  (endingt.tv_usec-startingt.tv_usec)/1000000.00;
  timestamp[count] = eltime;
  fprintf(fp,"sample %d value:%lf ",count,eltime);
  fprintf(fp,"\n");
  count= count +1;

}


int main(int argc,char *argv[]){
if (argc != 3){
  printf("wrong number of arguments");
  exit(0);                                    //to be portable
}
int mysignal;
int samples,period;
int n,i;
samples = atoi(argv[1]);
period = atoi(argv[2]);
printf("number of samples are %d and period is: %d \n",samples,period);

mysignal = SIGALRM;   //apothikeyw thn onomasia tou signal sthn metavlhth mysignal
count = 0; //arxikopoihsh tou counter
fp = fopen("samples.txt","w+");
if (fp == NULL){
  printf("unable to create file");
  exit(1);
}
struct itimerval expire;
struct sigaction new_action;   //without old_action
timestamp = malloc(samples*sizeof(double));
sigset_t mask;
expire.it_interval.tv_sec = period;  //h periodos anamesa se 2 interrupts
expire.it_value.tv_sec = period;  //first interrupt at time period

expire.it_interval.tv_usec = 0;
expire.it_value.tv_usec = 0;
new_action.sa_handler = myhandler; //function pointer
//FLAGS?
new_action.sa_flags=0;
//initialize mask
sigemptyset(&new_action.sa_mask); //adeiazoume thn maska wste na mhn mplokarei kapoio shma
n=sigaction(mysignal,&new_action,NULL);  //stelnei ston handler ton SIGALRM gia thn diaxeirish tou NULL-->old_action
if(n==-1){
  exit(1);
}
n=setitimer(ITIMER_REAL,&expire,NULL);
if(n==-1){
  exit(1);
}
sigfillset(&mask); //gemizw thn maska me ola ta shmata kai sth synexeia tha afairesw to shma pou thelw
n=sigdelset(&mask,mysignal); //afairw to SIGALRM
if(n==-1){
  exit(1);
}
n=sigismember(&mask,mysignal);
if(n==1){ //shmainei oti den to afairesa
  sigdelset(&mask,mysignal);
  }
gettimeofday(&startingt,NULL); //pairnoume ton xrono wste na kseroume pote ksekinhse kathe sample
while(count< samples){
  //printf("mphke");
  sigsuspend(&mask);       //h sigpause tha stamathsei otan dothei SIGALRM kai stalei apo thn sigaction ston myhandler gia thn diaxeirish tou
}
  printf("ta deigmata se seconds einai: \n");
  for(i=0; i<count; i++){
     printf("%lf \n",timestamp[i]);
   }
fclose(fp);

return 0;
}
