#include<pthread.h>
#include<iostream>
#include<time.h>
#include<unistd.h>
#include<iomanip>
#include<stdlib.h>
#include<string.h>

using namespace std;
void* ATB(void*);
void* BTA(void*);

pthread_mutex_t mutex,atb,bta,mutex1;
time_t start;

int atbcnt,btacnt;
int atbcyc,btacyc;
int onatb,onbta;
int VILLCNT,STARVCNT;

void display(int id,string str="")
{
	pthread_mutex_lock(&mutex);
	for(int i=0;i<VILLCNT;i++)
	{ 
		if(id==-1) cout<<setw(7)<<"Vill. "<<i+1<<'|';
		else
		{
			if(i==id-1) cout<<setw(8)<<str<<'|';
			else cout<<setw(9)<<'|';
		}
	}
	cout<<time(NULL)-start<<"sec"<<endl;
	pthread_mutex_unlock(&mutex);
}

void* ATB(void* id)
{
	display(*(int*)id,"Appeared");
	display(*(int*)id,"A to B");
	display(*(int*)id,"Waiting");	
	pthread_mutex_lock(&atb);
	display(*(int*)id,"Got Perm");
	pthread_mutex_lock(&mutex);	
	::atbcnt++;
	::atbcyc++;	
	if(::atbcnt==1)
		pthread_mutex_trylock(&bta);
	if(::atbcyc!=STARVCNT)
		pthread_mutex_unlock(&atb);
	else
		::atbcyc=0;
	pthread_mutex_unlock(&mutex);
	display(*(int*)id,"Crossing");
	pthread_mutex_lock(&mutex1);
	::onatb++;
	while(::onatb>1);
	pthread_mutex_unlock(&mutex1);
	sleep((rand()%5)+2);
	display(*(int*)id,"Crossed!");
	pthread_mutex_lock(&mutex);
	::atbcnt--;
	::onatb--;
	if(::atbcnt==0)
		pthread_mutex_unlock(&bta);
	pthread_mutex_unlock(&mutex);
	sleep(2);
	if(!pthread_mutex_trylock(&bta))
	{
		pthread_mutex_unlock(&atb);
		pthread_mutex_unlock(&bta);
	}
	pthread_exit(NULL);
}
void* BTA(void* id)
{
	display(*(int*)id,"Appeared");
	display(*(int*)id,"B to A");
	display(*(int*)id,"Waiting");	
	pthread_mutex_lock(&bta);
	display(*(int*)id,"Got Perm");
	pthread_mutex_lock(&mutex);
	::btacnt++;
	::btacyc++;
	if(::btacnt==1)
		pthread_mutex_trylock(&atb);
	if(::btacyc!=STARVCNT)
		pthread_mutex_unlock(&bta);
	else
		::btacyc=0;
	pthread_mutex_unlock(&mutex);
	display(*(int*)id,"Crossing");
	pthread_mutex_lock(&mutex1);
	::onbta++;
	while(::onbta>1);
	pthread_mutex_unlock(&mutex1);
	sleep((rand()%5)+2);
	display(*(int*)id,"Crossed!");
	pthread_mutex_lock(&mutex);
	::btacnt--;
	::onbta--;
	if(::btacnt==0)
		pthread_mutex_unlock(&atb);
	pthread_mutex_unlock(&mutex);
	sleep(2);
	if(!pthread_mutex_trylock(&atb))
	{
		pthread_mutex_unlock(&bta);
		pthread_mutex_unlock(&atb);
	}
	pthread_exit(NULL);
}

int main(int a, char *b[10])
{
    int m,n,STARVCNT=3;
	::VILLCNT=(rand()%5)+4;
    if(a==4)
    {
        m=atoi(b[1]);
        n=atoi(b[2]);
        ::STARVCNT=atoi(b[3]);
		::VILLCNT=m+n;
    }
    else if(a==3)
    {
        m=atoi(b[1]);
        n=atoi(b[2]);
		::VILLCNT=m+n;
    }
    else if(a==2)
        ::STARVCNT=atoi(b[1]);
    else if(a>4)
    {
        cout<<"Invalid number of arguments\n";
        exit(0);
    }
    
	::start=time(NULL);
    
	pthread_t vill[VILLCNT];
	::atbcnt=0;::btacnt=0;
	::atbcyc=0;::btacyc=0;
	::onatb=0;::onbta=0;
	int id[VILLCNT];
    	int t;
	
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&atb,NULL);
	pthread_mutex_init(&bta,NULL);
	pthread_mutex_init(&mutex1,NULL);
	display(-1);
	srand(time(NULL));
    	for(int i=0;i<VILLCNT;i++)
	{
        	void* (*villgen)(void*);
        	id[i]=i+1;
        
        	t=rand()%2;
	    	if(t==1 && m!=0)	//villager creation random village
	    	{	
            		villgen=&ATB;
            		m--;
        	}   
	    	else if(t==0 && n!=0)
        	{
		    villgen=&BTA;
            		n--;
        	}	
		else if(t==0 && m!=0)
		{
			    villgen=&ATB;
		    m--;
		}
		else if(t==1 && n!=0)
		{
			    villgen=&BTA;
		    n--;
		}
		    sleep(rand()%4);
		    pthread_create(&vill[i],NULL,villgen,(&id[i]));
	}
	pthread_exit(NULL);
	return 0;
}
