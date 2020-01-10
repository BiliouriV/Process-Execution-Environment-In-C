// Programmers: Chiona Kiriaki, Biliouri Victoria. 
// Hw3~Prog2~2017
// Program used as process for hw3
// input arguments:max number of repeats and 0 or 1 for sugusr1 
// it prints with a time period of 5 secs. in which repeat we are in each moment.

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>

volatile sig_atomic_t counter;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~handler~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void handler (int sig){									//handler gia midenismo tou counter kai ekkinisi apo tin arxi
	counter = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~main~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int main (int argc, char *argv[]) {
    
	int max, block, return_value;
	struct sigaction act = {{0}};
	sigset_t set;
	pid_t pid;
	
	if (argc != 4) {											//an exei dothei swstos arithmos orismatwn
		fprintf(stderr, "Wrong number of arguments!!\n");
		exit(1);
	}
	
	if ((strcmp(argv[0], "-m") == 0) || (strcmp(argv[2], "-b"))) {	//an ta orismata einai swsta -m kai -b
		
		max = atoi(argv[1]);										//metatropi tou max se akeraio
		
		if (max < 0) {												//elegxos an to max einai thetikos akeraios
			fprintf(stderr, "Wrong max number!!\n");
			exit(1);
		}
			
		block = atoi(argv[3]);										//metatropi tou block se akeraio
		if ((block != 0) && (block != 1)){							//elegxos an einai 0 h 1
			fprintf(stderr, "Wrong block number!!\n");
			exit(1);
		}
		
		act.sa_handler = handler;									//orismos twn pediwn tou struct act
		act.sa_flags = SA_RESTART;
		sigaction(SIGUSR1, &act, NULL);
		
		if (block==1) {														//an to block einai 1, adeiazoume to set ton shmatwn
			return_value = sigemptyset(&set);
			if (return_value != 0) {
				fprintf(stderr, "Error in sigemptyset: %s\n", strerror(errno));
				exit(1);
			}
			
			return_value = sigaddset(&set, SIGUSR1);						//prosthiki tou SIGUSR1 sto set
			if (return_value != 0) {
				fprintf(stderr, "Error in sigaddset: %s\n", strerror(errno));
				exit(1);
			}
			
			return_value = sigprocmask(SIG_BLOCK, &set, NULL);				//kai blokaroume to set pou perilamvanei to SIGUSR1
			if (return_value != 0) {
				fprintf(stderr, "Error in sigprocmask: %s\n", strerror(errno));
				exit(1);
			}
		}
		
		pid = getpid();
		
		for (counter = 1; counter <= max; counter++) {						//ektypwsi tou arithmou tis epanalipsis pou vriskomaste me sleep 5 sec
			fprintf(stderr, "Pid: %d. %d/%d \n", (int)pid, (int)counter, max);
			
                        sleep(5);
			
			if ((block == 1) && (counter == max/2)) {						//ksemplokarisma tou simatos SIGUSR1 meta tis mises epanalipseis
				sigprocmask(SIG_UNBLOCK, &set, NULL);
			}
		}
	}
	else {
		fprintf(stderr, "Wrong arguments!!\n");
		exit(1);
	}
	
	return(0);
}