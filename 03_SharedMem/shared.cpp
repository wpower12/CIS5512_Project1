/*
 * hello.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: mitranes
 */

/* m.cpp */
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <unordered_map>
#include <string.h>
#include <fcntl.h>
#include <fstream>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
// #include <errno.h>

using namespace std;

int main()
{

	int shm_fd;//shared memory file descriptor
	int file; //file descriptor for Anna Karenina
	struct stat fileStats;
	int status;
	size_t size = 1986560;
	void * mapped;
	char* path;


	int counter = 0;
    pid_t pid = fork();


    if (pid == 0)
    {
        // CHILD PROCESS
        int i = 0;
        for (; i < 5; ++i)
        {
            printf("child process: counter=%d\n", ++counter);
        }
    }
    else if (pid > 0)
    {
        // PARENT PROCESS
    	path = "AnnaKarenina.txt";
    	//first open file

    	shm_fd = shm_open(path, O_CREAT | O_RDONLY | O_RDWR, S_IRUSR | S_IWUSR );
    	if(shm_fd < 0)
    		cout << "Couldn't open Anna K file.";

    	ftruncate(shm_fd, size);

    	//create shared memory--memory map the file
    	//mapped = mmap(NULL,size,PROT_READ,MAP_SHARED,shm_fd,0);
    	mapped = mmap(NULL,size,PROT_READ,MAP_SHARED,shm_fd,0);
    	if(mapped == MAP_FAILED){
    		cout<<"\nIssue with your MAP!";
    		cout<< "map failed: %s "<< strerror(errno);
    	}


        cout<< "page size " << sysconf(_SC_PAGE_SIZE);
        int j = 0;
        for (; j < 5; ++j)
        {
            printf("parent process: counter=%d\n", ++counter);
        }
    }
    else
    {
        // fork failed
        printf("fork() failed!\n");
        return 1;
    }

    printf("--end of program--\n");



//UNORDERED MAP EXAMPLE BELOW.....

  unordered_map<string, int> map;

  map["word"] = 1;
  map["test"]= 1;
  map["mitra"]=1;
  map["mitra"]=1;
  map["hello"]=1;
  map["help"]=1;


  //prints everything in map
  for ( auto ii = map.begin() ; ii != map.end() ; ii++ )
          cout << ii->first << " : " << ii->second << endl;
  cout << "mymap's buckets contain:\n";
    for ( unsigned i = 0; i < map.bucket_count(); ++i) {
      cout << "bucket #" << i << " contains:";
      for ( auto local_it = map.begin(i); local_it!= map.end(i); ++local_it )
          cout << " " << local_it->first << ":" << local_it->second;
          cout << std::endl;
        }

  return 0;
}