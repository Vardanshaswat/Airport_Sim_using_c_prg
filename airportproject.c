#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<math.h>
#include<stdbool.h>
#include<time.h>
#include<limits.h>

#define MAXQUEUE 5
#define TRUE 1
#define FALSE 0

#define Error {return 0;}

typedef enum action{ARRIVE ,DEPART}Action;

typedef struct plane
{
    int id;
    int tm;
}Plane;// struct to define the identification of the plane

//typedef char QueueEntry;
typedef Plane QueueEntry;
typedef struct queue
{
    int count;
    int front;
    int rear;
    QueueEntry entry[MAXQUEUE];
}Queue;  //define the queue


void Append(QueueEntry, Queue *);
void CreateQueue(Queue *);
void Serve(QueueEntry *, Queue *);
int QueueSize(Queue *);
bool QueueEmpty(Queue *);
bool QueueFull(Queue *);

int main(void)
{
    Queue landing, takeoff;
    Queue*pI = &landing;
    Queue *pt= &takeoff;
    Plane plane;

    int curtime;
    int endtime;
    double expectarrive;
    double expectdepart;
    int i;
    int idletime;
    int landwait;
    int nland;
    int nplanes;
    int nrefuse;
    int ntakeoff;


    int pri;
    int takeoffwait;//defing the variable required for different program


    CreateQueue(pI);//function to create a queue landing
    CreateQueue(pt);//akeoff/function to create a queue
    nplanes=nland=ntakeoff=nrefuse=0;
    landwait=takeoffwait=idletime=0;
    start(&endtime, &expectarrive, &expectdepart);
    for(curtime=1; curtime<=endtime;curtime++)
    {
        pri=PoissonRandom(expectarrive);
        for(i=1; i<=pri; i++)
        {
            NewPlane(&plane, &nplanes, curtime, ARRIVE);
            if(QueueFull(pI))
                Refuse(plane, &nrefuse, ARRIVE);
            else
                Append(plane, pI);

        }

        pri=PoissonRandom(expectdepart);
        for(i=0; i<=pri; i++)
        {


            NewPlane(&plane, &nplanes, curtime, DEPART);
        if(QueueFull(pt))
            Refuse(plane, &nrefuse, DEPART);
        else
            Append(plane, pt);
    }

    if(!QueueEmpty(pI))
    {
        Serve(&plane, pI);
        Land(plane, curtime, &nland, &landwait);
    }
     else if(!QueueEmpty(pt))
    {
        Serve(&plane,pt);
        Fly(plane,curtime,&ntakeoff, &takeoffwait);
    }else
    Idle(curtime, &idletime);
    }

Conclude(nplanes, nland, ntakeoff, nrefuse, landwait, takeoffwait, idletime, endtime, pt,pI);
return 0;
}


//int *endtime, double *expectarrive, double *expectdepart)
void start(int *endtime, double *expectarrive, double *expectdepart)
{
    bool ok;
    printf("This program simulates an airport with only one eunway.\n""One plane can land or depart in each unit time .\n" "U to %d planes can be waiting to land or take off" "at any time,\n",MAXQUEUE);
    printf("How many units of time simulation run?");
    scanf("%d",endtime);
    Randomize();
    do
    {
        printf("Expected no of arrivals per unit time?" "(real number)?");
        scanf("%lf",expectarrive);
        printf("Expected number of departure per unit of time? ");
        scanf("%lf",expectdepart);
        if(*expectarrive < 0.0 || *expectdepart <0.0)
        {
            printf("These numbers must be non negative.\n");
            ok = FALSE;
        }else if(*expectarrive + *expectdepart >1.0)
        {
            printf("The airpport is saturated","read new numbers");
            ok=!UsersSaysYes();
        }else
        ok=TRUE;

    }while(ok==FALSE);
}//printing the massage an intilizaing;;



void NewPlane(Plane *p, int *nplanes, int curtime , Action kind)
{
    (*nplanes)++;
    p->id=*nplanes;
    p->tm=curtime;
    switch(kind)
    {
    case ARRIVE:
        printf("  Plane %3d ready to land.\n", *nplanes);
        break;
    case DEPART:
        printf("  Plane %3d ready to take off.\n", *nplanes);
        break;

    }
}

void Refuse(Plane p, int *nrefuse, Action kind)
{
    switch(kind)
    {
    case ARRIVE:
        printf(" Plane %3d directed to another airport.\n",p.id);
        break;
    case DEPART:
        printf("Plane %3d told to try later.\n",p.id);
        break;
    }
    (*nrefuse)++;
}


void Land(Plane p , int curtime, int *nland, int *landwait)
{
    int wait;
    wait = curtime-p.tm;
    printf("%3d: Plane %3d landed;in queue %d units. \n",curtime, p.id, wait);


(*nland)++;
*landwait+=wait;
}





//processing a depart plane


void Fly(Plane p, int curtime, int *ntakeoff, int *takeoffwait)
{
    int wait;
    wait=curtime - p.tm;
    printf("%3d: Plane %3d took off; in queue %d units.\n",curtime,p.id,wait);
    (*ntakeoff)++;
    *takeoffwait+=wait;

}

void Idle(int curtime, int *idletime)
{
    printf("%3d: Runway is idle.\n", curtime);
    (*idletime)++;
}


void Conclude(int nplanes, int nland, int ntakeoff,int nrefuse, int landwait, int takeoffwait, int idletime, int endtime, Queue *pt, Queue *pI)
{
    printf("Simulation has concluded after %d units. \n",endtime);
    printf("Total numbers of plane processed: %3d\n", nplanes);
    printf("  Numbers of plane landed:   %3d\n", nland);
    printf(" Numbers of plane taken off:  %3d\n", ntakeoff);
    printf(" Numbers of plane refused use: %3d\n", nrefuse);
    printf(" Numbers left ready to land: %3d\n", QueueSize(pI));
    printf(" Numbers left ready to take off: %3d\n", QueueSize(pt));

    if(endtime>0)
        printf(" Percentage of time runway idle:  %6.2f\n",((double)idletime/endtime)*100.0);
    if(nland>0)
        printf(" Average wait time to land:  %6.2f\n",((double)landwait/nland));
    if(ntakeoff>0)
        printf(" Average wait time take off: %6.2f\n",(double)takeoffwait/ntakeoff);
}

void CreateQueue(Queue *q)
{
    q->count=0;
    q->front=0;
    q->rear=-1;
}

void Append(QueueEntry x, Queue *q)
{
    if(QueueFull(q))
    return ;
    else
    {
        q->count++;
        q->rear=(q->rear+1)%MAXQUEUE;
        q->entry[q->rear]=x;

    }
}


void Serve(QueueEntry *x, Queue *q)
{


        if(QueueEmpty(q))
        return;
        else
        {
            q->count--;
            *x=q->entry[q->front];
            q->front=(q->front+1)%MAXQUEUE;
        }


}


int QueueSize(Queue *q)
{
    return q->count;
}

bool QueueEmpty(Queue *q)
{
    return q->count<=0;
}

bool QueueFull(Queue *q)
{
    return q->count>=MAXQUEUE;
}

int UsersSaysYes()
{
   return 1;
}

void Randomize(void)
{
    srand((unsigned int)(time(NULL)%100));
}

int PoissonRandom(double expectedvalue)
{
    int n=0;
     double limit;
    double x;
    limit = exp(-expectedvalue);
    x=rand()/(double)INT_MAX;
    while(x>limit)
    {
        n++;
        x *=rand()/(double)INT_MAX;

    }
    return n;
}




