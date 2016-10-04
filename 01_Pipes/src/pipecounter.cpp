/* Project 1 - Subproject 1 - Pipes
 * 
 * Group: Bissell, Neshatfar, Power
 *
 * Parent process reads the file line by line, and places
 * each line into the buffer. Child process reads the buffer and
 * counts the words, adding to their values in the hashmap.
 */

#include <stdio.h>	
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <unordered_map>

int main( int argc, char** args ){
	int pid, pip[2];
	pipe(pip);	

	// The semaphore objects, in shared memory
	// Enforces a strict alternation between reading and writing.
	sem_t *canRead, *canWrite;
	if((canWrite = sem_open("write", O_CREAT+O_EXCL, 0644, 1)) == SEM_FAILED ){
		sem_unlink( "write" );
		canWrite = sem_open("write", O_CREAT, 0644, 1);
	}
	if((canRead = sem_open("read", O_CREAT+O_EXCL, 0644, 0)) == SEM_FAILED ){
		sem_unlink( "read" );
		canRead = sem_open("read", O_CREAT+O_EXCL, 0644, 0);
	}

	pid = fork();

	// ****** Child **********
	if (pid == 0) { 
		close( pip[1] );  

		std::ofstream logf ("log.txt", std::ofstream::out);
		std::unordered_map<std::string,int> wordcounts;

		char readbuffer[200];	
		char * token   = (char *)malloc(50);
		int r = 1;
		int tcount = 0;
		while( r > 0){
			// ** Critical Section for 'Readers'
			sem_wait( canRead );
			memset(&readbuffer, 0, sizeof(readbuffer));	// Clearing buffer before reading.
			r = read( pip[0], readbuffer, sizeof(readbuffer) );
			sem_post( canWrite );
			// ** End Critical Section 

			// Split the read line on ' ' and check each token in the hashmap
			token = strtok( readbuffer, " " );
			while(token){
				tcount++;
				auto result = wordcounts.find( token );
				if(result == wordcounts.end()){
					// Not found, insert
					wordcounts.insert({ token, 1 });
				} else {
					result->second = result->second+1;
				}
				token = strtok(NULL, " ");
			}
		}

		logf << "read: " << tcount << " tokens\n";
		logf.close();

		// iterate over hash-map to build output file.
		std::ofstream outfile ("out.txt", std::ofstream::out);
		for ( auto it = wordcounts.begin(); it != wordcounts.end(); ++it ){
			outfile << " " << it->first << ":" << it->second << "\n";
		}
		outfile.close();

	} else { 
	// ************ Parent ****************
		close( pip[0] );

		std::ifstream infile ("bin/ANNA_KARENINA.txt");	
		
		std::string line;
		int lcount = 0;
		if( infile.is_open() ){
			while( std::getline( infile, line ) ){ // Getting a line to write
				while( line.empty() ){			   // Skip over empty lines
					if(!std::getline( infile, line )){
						break;	 
					}
				}

				// ** Critical Section for Writers **
				sem_wait( canWrite );
				write( pip[1], line.c_str(), line.size() );
				sem_post( canRead );
				// ** End Critical Section **

				lcount++;
			}
			sem_post( canRead );
			close(pip[1]);
		} else {
			std::cout << "Cannot Read Input File.\n";
		}
		printf("Read %d lines\n", lcount);
		infile.close();
		sem_close( canWrite );
		sem_close( canRead );
	}

	return 0;
}