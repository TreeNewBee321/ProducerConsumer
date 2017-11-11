#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

/*信号量*/
//空的信号量和满的信号量
sem_t empty_sem, full_sem;   
//静态创建条件变量
pthread_cond_t full = PTHREAD_COND_INITIALIZER;        //满的变量
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;       //空的变量
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;      //互斥锁

#define BUFFERNUM 10    

//缓冲区队列
struct Buffer_Queue {
	char production[BUFFERNUM];  //产品
	int front, rear;             //头指针和尾指针
	int num;                     //缓冲区里面字母数量        
};
//随机产生生产字符
char getRandChar()
{
	int temp = rand()%26;
	return ('a'+temp);
}

//打印进程运行结果
void produce_process(struct Buffer_Queue *q, int id){
		pthread_mutex_lock(&lock);       
		while (q->num == BUFFERNUM)    //缓冲区已经满了，等待
		{            
			pthread_cond_wait(&full, &lock);
		}
		sem_wait(&empty_sem);

		/*生产产品*/
		char c = getRandChar();                                      //随机获取字母
		q->rear = (q->rear + 1) % BUFFERNUM;                         //计算新的尾指针   
		q->production[q->rear] = c;                                  //写入新产品
		q->num++;
		
		/*打印输出结果*/
		printf("-------------------------------------------------------------\n");
		int i;
		printf("Size of data in buffer:(%d)",q->num);                  //打印缓冲区中的数据
		if(q->front < q->rear)
		{
			for(i = q->front; i <= q->rear; i++)
				printf("%c ",q->production[i]);
		}
		else
		{
			for(i = q->front; i < BUFFERNUM; i++)
				printf("%c ",q->production[i]);
			for(i = 0; i <= q->rear; i++)
				printf("%c ",q->production[i]);
		}
		printf("\nThread now:producer%d\n",id);   //打印当前执行的进程
		printf("Data produced: %c\n",c);      //打印产生或消费的数据
		printf("-------------------------------------------------------------\n");

		sem_post(&full_sem);

		if (q->num == 1) {
			pthread_cond_signal(&empty);    
		}
		pthread_mutex_unlock(&lock); 

		sleep(rand() % 2);	
}

//生产者1
void *producer1(void *args)
{
	struct Buffer_Queue *q;
	q = (struct Buffer_Queue *)args;
	while(1)
	{
		produce_process(q,1);
	}
}
void *producer2(void *args)
{
	struct Buffer_Queue *q;
	q = (struct Buffer_Queue *)args;
	while(1)
	{
		produce_process(q,2);
	}
}
void *producer3(void *args)
{
	struct Buffer_Queue *q;
	q = (struct Buffer_Queue *)args;
	while(1)
	{
		produce_process(q,3);
	}
}
//
void consumer_process(struct Buffer_Queue *q, int id){
		pthread_mutex_lock(&lock);        
		while (q->num == 0)           //缓冲区已经空了，等待
		{        
			pthread_cond_wait(&empty, &lock);
		}
		sem_wait(&full_sem);

		/*消费产品*/
		q->front = (q->front + 1) % BUFFERNUM;    //计算新的头指针
		char c = q->production[q->front];         //消费产品
		q->production[q->front] = ' ';            //
		q->num--;

		/*打印输出结果*/
		printf("-------------------------------------------------------------\n");
		int i;
		printf("Size of data in buffer:(%d)",q->num);                   //打印缓冲区中的数据
		if(q->front < q->rear)
		{
			for(i = q->front; i <= q->rear; i++)
				printf("%c ",q->production[i]);
		}
		else
		{
			for(i = q->front; i < BUFFERNUM; i++)
				printf("%c ",q->production[i]);
			for(i = 0; i <= q->rear; i++)
				printf("%c ",q->production[i]);
		}
		printf("\nThread now:consumer%d\n",id);   //打印当前执行的进程
		printf("Data consumed:%c\n",c);      //打印产生或消费的数据
		printf("-------------------------------------------------------------\n");


		sem_post(&empty_sem);
		if (q->num == BUFFERNUM - 1) {
			pthread_cond_signal(&full);
		}
		pthread_mutex_unlock(&lock);
		sleep(rand() % 2);
}
//消费者
void *consumer1(void *args)
{
	struct Buffer_Queue *q;
	q = (struct Buffer_Queue *)args;
	while(1)
	{
		consumer_process(q,1);
	}
}
void *consumer2(void *args)
{
	struct Buffer_Queue *q;
	q = (struct Buffer_Queue *)args;
	while(1)
	{
		consumer_process(q,2);
	}
}

int main(int argc, char *argv[])
{
	/*创建缓冲区*/
	//定义
	struct Buffer_Queue *q;
	q = (struct Buffer_Queue *) malloc(sizeof(struct Buffer_Queue));
	//初始化队列
	q->front = q->rear = BUFFERNUM - 1;
	q->num = 0;

	/*执行进程*/
	//定义四个线程
	pthread_t pid1, cid1, pid2, cid2,pid3;
	//初始化信号量
	sem_init(&empty_sem, 0, BUFFERNUM);   
	sem_init(&full_sem, 0, 0);   
	//创建线程
	pthread_create(&pid1, NULL, producer1, (void *) q);
	pthread_create(&cid1, NULL, consumer1, (void *) q);
	pthread_create(&pid2, NULL, producer2, (void *) q);
	pthread_create(&cid2, NULL, consumer2, (void *) q);
	pthread_create(&pid3, NULL, producer3, (void *) q);
	//销毁线程
	pthread_join(pid1, NULL);
	pthread_join(cid1, NULL);
	pthread_join(pid2, NULL);
	pthread_join(cid2, NULL);
	pthread_join(pid3, NULL);
	//销毁信号量
	sem_destroy(&empty_sem);
	sem_destroy(&full_sem);

	return 0;
}
