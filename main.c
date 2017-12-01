#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include "rbt_header.h"
#define OPS_LIMIT 20
#define WRITE_LIMIT 18


int _dataCount=0;
int _opsCount=0;
int count=0;
//#define IOCTL_SET_END _IOW('k',1, int)

void* _inputData(void* sleepTime){
		
	//loctl =0->read, 1->write
	int flag,fd,send=0;
	pthread_t myid;
        char buffer[5];
	char val_array[2], data_array[1];	
		
	while(_opsCount < OPS_LIMIT)
	{
		
		pthread_t myid;
		myid = pthread_self();
		printf("Thread ID accessing the tree = %lu\n",myid);
		//Open the file for Read and write operations
		fd = open("/dev/rbt530_dev", O_RDWR);
		
		int key,data;

		if(_dataCount < WRITE_LIMIT ) {
			flag=1;
			data = 1;
		 }	
		else{
			int temp = rand() % 2;
			flag = temp;
			data = rand() % 2;
		}
			

		if(flag == 1){

			if (fd < 0 ){
          		      printf("Can not open device file.\n");
                		close(fd);
			      	return 0;
       			 }else{
				//generate random data and decide whether to insert/replace or erase a node.
			 	 key = rand() % 100;
				//key = send++;
				_dataCount++;
				
				val_array[0] = key;
				val_array[1] = data;

				int res = write(fd, val_array, 2);
				//printf("write returned : %d \n",res);
				printf("data entered : key =  %d, data = %d \n",val_array[0], val_array[1]);
		
			}
		}
		else {
		    		//Randomly decide whether to read or write.
				unsigned long cmdVal = rand() % 2;

				printf("IOCTL Id is %lu , cmd :%lu \n",IOCTL_SET_END,cmdVal);	
				long set_cmd = ioctl(fd, IOCTL_SET_END, cmdVal);
				printf("Ioctl set. It returned : %ld\n",set_cmd);
				
				int result = read(fd,data_array,1);
				
				if(cmdVal==0)
					printf("Reading minimum Value : %d\n",result);
				else
					printf("Reading maximum value : %d\n",result);
			}
				

		//Once read or write is done increment the operation count and release the mutex
		_opsCount++;
		//printf("Operations finished = %d \n", _opsCount);
		printf("---------------------------------------\n");		
		close(fd);
		//Sleep for sometimei
		//count++;
		sleep(*((int*)sleepTime));
		
	}
	pthread_exit(NULL);

}

void* kprobe_test(void* sleepTime){
	char input[2];
	input[0]=0;
	input[1]=1;
	char output[256];
	memset(output,0,256);
 	int fd,retVal;	
	fd = open("/dev/RBprobe",O_RDWR);

	retVal = write(fd,input,2);
	if (retVal < 0){
		printf("registration of kprobe was not successful!\n");
        }

	while(_dataCount<WRITE_LIMIT){
		int res = 0;
		
		do {
			res = read(fd,output,256);
		} while(res==0);
		//printf("Reading the KPROBE buffer values....\n");
		
		//if(res>0){
		int i=0;	
		//printf("Characters returned = %d\n",res);
		for(i;i<res;i++) printf("%c",output[i]);
		
	}
	//if(_opsCount > OPS_LIMIT){
		input[1] == 0;
		retVal = write(fd,input,2);
		close(fd);
		pthread_exit(NULL);
	//}
}

int main(int argc, char* argv[]){


	int thread_id1,thread_id2,thread_id3,thread_id4, thread_kp;
	pthread_t pthd1,pthd2, pthd3, pthd4, pkp_thread;
	int sleep_id1 = 6, sleep_id2 = 4, sleep_id3 = 7, sleep_id4 = 13, sleep_kprobe = 3;
	
	thread_kp = pthread_create(&pkp_thread, NULL, kprobe_test,(void*)&sleep_kprobe);
	sleep(5);

	thread_id1 = pthread_create(&pthd1, NULL, _inputData, (void*)&sleep_id1);
	thread_id2 = pthread_create(&pthd2, NULL, _inputData, (void*)&sleep_id2);
	thread_id3 = pthread_create(&pthd3, NULL, _inputData, (void*)&sleep_id3);
	thread_id4 = pthread_create(&pthd4, NULL, _inputData, (void*)&sleep_id4);

	//thread_kp = pthread_create(&pkp_thread, NULL, kprobe_test,NULL);

	pthread_join(pthd1, NULL);
	
	pthread_join(pthd2, NULL);
	pthread_join(pthd3, NULL);
	pthread_join(pthd4, NULL);
	
	pthread_join(pkp_thread,NULL);

	
	char final_array[100];
	memset(final_array,0,100);

	//cmdVal = 2 for reading all the data in the tree.
	int fd = open("/dev/rbt530_dev", O_RDWR);
	long set_cmd = ioctl(fd, IOCTL_SET_END, 2);
	
	int result = read(fd,final_array,100);
	printf("Read the tree of size : %d\n",result);
	
	//Dump all the data from the tree.
	printf("ALL THE TREE DATA : \n");
	int i;
	for(i=0;i<result;i++) printf("%d,",final_array[i]);
 	printf ("\n");	
	close(fd);

	return 0;
}

