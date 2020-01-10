// Programmers: Chiona Kiriaki, Biliouri Victoria. 
// Hw3~Prog2~2017
// This is a program that the user unputs some commands which
// relate to the parallel execution of processes.
// It creates a double connected list whose nodes
// are running processes.
// the processes are executed with time scheduling
// with circle 20sec.
// the commands that the program can execute are
// exec, term, sig, info, quit.

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include<sys/time.h>

#define NAME_SIZE 50
#define SIZE 100

//~~~~~~~~~~~~~~~~~~~~~~~~~orismos tou kombou tis listas~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
typedef struct Node {
    
    struct Node *next, *prev;				//deiktes gia tin dipli diasyndesi tis listas
    pid_t pid;								//pid tis diergasias
    int run;								//1 h 0 analogos an i diergasia trexei i oxi
    char *name_list;						//onoma toy programmatos 
    char **args;							//double pointer , pinakas apo deiktes me ta orismata
    int argc;								//arithmos orismatwn
} Node;

Node *head;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~arxikopoihsh ths listas~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void list_init() {
    
    head = (struct Node *)malloc(sizeof(struct Node));
	if (head == NULL) {
		fprintf(stderr, "Error in function malloc!\n");
		exit(1);
	}
    head->next = head;
    head->prev = head;
    
    head->pid = 0;
    head->run = -1;
    head->name_list = NULL;
    head->args = NULL;
    head->argc = 0;
}

//~~~~~~~~~~~~~~~~~sunartisi pou prosthetei enan neo kosmo sthn lista~~~~~~~~~~~~~~~~//
Node *list_insert(int pid, char *name, char *args){
	
    Node *new_node;
    int i, argc=0;
    char *temp;
    
    new_node = (struct Node *)malloc(sizeof(struct Node)); 					//dunamiki desmeusi mnimis gia ton neo komvo
	if (new_node == NULL) {
		fprintf(stderr, "Error in function malloc!\n");
		exit(1);
	}
    
    new_node->pid = pid;
    new_node->run = 0;
    new_node->name_list = strdup(name);										//antigrafi tou pointer tou onomatos kai desmeysi mnimis gia to onoma
	if (new_node->name_list == NULL) {
		fprintf(stderr, "Error in function strdup!\n");
		exit(1);
	}
    
    temp = strtok(NULL,"\t\n ");											//eyresh tou arithmou orismatwn me xrisi tis strtok
    if(temp != NULL){
        argc++;
    }
    
    while(temp != NULL){													//diaxwrismos twn orismatwn me xrisi tis strtok
        
        temp = strtok(NULL,"\t\n ");
        argc++;
    }
    new_node->argc = argc;
    
    new_node->args = (char **)malloc((argc+1)*(sizeof(char *)));			//dunamiki desmeysi mnimis gia ta orismata
	if (new_node->args == NULL) {
		fprintf(stderr, "Error in function malloc!\n");
		exit(1);
	}
	
    temp = args;
    for(i=0; i<argc; i++){													//dimiourgia tou pinaka deiktwn
        new_node->args[i] = strdup(temp);
		if (new_node->args[i] == NULL) {
			fprintf(stderr, "Error in function strdup!\n");
			exit(1);
		}
        temp = strchr(temp, '\0');
        temp++;
             
    }
    new_node->args[argc]=NULL;												//topothetisi null stin teleytaia thesi tou pinaka deiktwn
    
    new_node->next = head;													//dipli diasyndesi tou kainoyriou komvou me tous upoloipous stin lista
    new_node->prev = head->prev;
    head->prev->next = new_node;
    head->prev = new_node;

    return(new_node);
}

//~~~~~~~~~~~~~~~~sunartisi pou diagrafei enan komvo apo tin lista~~~~~~~~~~~~~~~~~~~//
void list_remove(pid_t pid) {
    
    Node *curr;
    
    int i;
    
    for (curr = head->next; (curr != head) && (curr->pid != pid); curr = curr->next);  //diatreksi tis listas gia eyresh tou sygkekrimenou kovou
    
    if (curr != head) {									//an vrethei o zitoumenos komvos
        curr->prev->next = curr->next;					//xanetai i syndesh me tous ypoloipus komvous, apokathistatai h omalh syndesh tis listas
        curr->next->prev = curr->prev;
        
        free(curr->name_list);							//kai apeleytherwnontai diadoxika ta pedia (osa einai pointers kai epeita olos o komvos)
        for(i=1; curr->args[i]!=NULL ;i++) {
            free(curr->args[i]);
        }
        free(curr->args);
        free(curr);
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~sunartisi pou katastrefei tin lista~~~~~~~~~~~~~~~~~~~~~~~~//
void destroy_list() {
    
    Node *curr, *tmp;
    int i;
    
    for (curr = head->next; curr != head; curr = tmp) {				//diatreksi olis tis listas kai apeleytherwsi olwn twn komvwn diadoxika
        
        free(curr->name_list);
        for(i=1; curr->args[i] !=NULL;i++) {
            free(curr->args[i]);
        }
        free(curr->args);
        tmp = curr->next;
        free(curr);
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~ektypwsh olwn twn komvwn tis listas~~~~~~~~~~~~~~~~~~~~~~~~~//
void print_list() {
    
    Node *curr;
    int i;
    
    for(curr=head->next; curr != head; curr = curr->next){						//diatreksi olis tis listas
        
        fprintf(stderr,"pid: %d, name: (%s", curr->pid, curr->name_list);
        
        for (i=0; i<curr->argc; i++){
            
            fprintf(stderr, ", %s",curr->args[i]);
            
        }
        fprintf(stderr,")");
        if (curr->run ==1){														//an to pedio run einai 1 tote ektypwnetai kai to (R)
            fprintf(stderr,"(R)");
        }
        fprintf(stderr,"\n");
    }
}

//~~~~~~~~~~~~~~~~~~~sunartisi pou allazei to pedio run enos komvou~~~~~~~~~~~~~~~~~~//
void run_change(pid_t pid, int run){
    
    Node *curr;
    Node *temp;
        
    for (curr = head->next; (curr != head) &&(curr->pid != pid); curr = curr->next);	//psaxnei ton komvo me to pid pou theloume
    if (curr != head){																	//kai an ton vrei allazei to pedio run
        curr->run = run;
        for( temp = curr->next; temp != curr; temp = temp->next){
            temp->run = 0;
        }
    }
}

//~~~~~~~~~~~~~~sunartisi pou allazei poios komvos trexei ayti tin stimgi~~~~~~~~~~~~//
void start_stop(){
	
	int ret_value;
    struct Node *curr;
        
    for (curr = head->next; (curr != head) &&(curr->run != 1); curr = curr->next);			//psaxnei an kapoios komvos trexei twra
    
    if (curr != head){												//an trexei
        
        if( head->next != head->prev){      						//kai an den einai mono mia ergasia sthn lista
            
            if(curr->next != head){									//kai an o epomenos komvos den einai o head
                curr->run = 0 ;										//allazei to run tis se 0 kai stamataei me to sima SIGSTOP
                curr->next->run = 1;								//allazei to run tis epomenis kai tin ksekinaei me to sima SIGCONT
                ret_value = kill(curr->pid,SIGSTOP);
				if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigstop!\n");
                    exit(1);
                }
                ret_value = kill(curr->next->pid,SIGCONT);
				if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigcont!\n");
                    exit(1);
                }
            }
            else{													//an o epomenos komvos einai o head tote 
                curr->run = 0 ;										//to run tis trexousas ginetai ena kai stamataei me to sima SIGSTOP
                head->next->run = 1;								//run tou epomenou komvou meta to head ginetai ena kai ksekinaei me SIGCONT
                kill(curr->pid,SIGSTOP);
                kill(head->next->pid,SIGCONT);
            }
        }
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~handler~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void handler(int signum) {
    
	int ret_value;
    struct Node *curr;
    
    fprintf(stderr, "BEEP!!!\n");							//otan kaleitai o handler (diladi kathe 20 deyterolepta) ektypwnetai to alarm
    
    for (curr = head->next; (curr != head) && (curr->run != 1); curr = curr->next);			//vriskei an kapoia diergasia trexei ayti tin stigmi
	
    if (curr != head) {															//an trexei
        if( head->next != head->prev){     										//kai an den einai mono mia diergasia`stin lista
            if(curr->next != head){												//kai an o epomenos komvos den einai o head
                curr->run = 0 ;													//allazei to run tis se 0 kai stamataei me to sima SIGSTOP
                curr->next->run = 1;											//allazei to run tis epomenis kai tin ksekinaei me to sima SIGCONT
                ret_value = kill(curr->pid,SIGSTOP);
				if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigstop!\n");
                    exit(1);
                }
                ret_value = kill(curr->next->pid,SIGCONT);
				if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigcont!\n");
                    exit(1);
                }
            }
            else{																//an o epomenos komvos einai o head tote 
                curr->run = 0 ;													//to run tis trexousas ginetai ena kai stamataei me to sima SIGSTOP
                head->next->run = 1;											//run tou epomenou komvou meta to head ginetai ena kai ksekinaei me SIGCONT
                ret_value = kill(curr->pid,SIGSTOP);
				if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigstop!\n");
                    exit(1);
                }
                ret_value = kill(head->next->pid,SIGCONT);
				if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigcont!\n");
                    exit(1);
                }
            }
        }
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~main~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int main (int argc, char *argv[]) {
	
    char command[SIZE];
    char *selection, *args, *name, *temp_pid;
	int status = 0, flag_counter = 0, ret_value;
    pid_t pid = 0;
	
    Node *node, *curr;
	
    struct sigaction siguser1 = {{0}};
    struct sigaction alarm = {{0}};
    struct itimerval timer = {{0}} ;
    
    siguser1.sa_handler = SIG_IGN;								//ignore to sigusr1
    siguser1.sa_flags = SA_RESTART;								//i ektelesi na synexizetai apo to idio simeio ektelontas ksana tin entoli pou emeine stin mesh
    sigaction(SIGUSR1, &siguser1 ,NULL);
            
    timer.it_interval.tv_sec = 20;								//arxikopoihsh alarm(20 deyterolepta kai 0 microseconds)
    timer.it_interval.tv_usec = 0;
    timer.it_value.tv_sec = 20;
    timer.it_value.tv_usec = 0;
    
    alarm.sa_handler = handler;									//kathe 20 deyterolepta na ekteleitai o handler
    alarm.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &alarm, NULL);
    
    list_init();
    do{
        fprintf(stderr,"Enter command: \n");
        
        fgets(command,SIZE,stdin);								//apothikeusi tis entolis sto command me fgets
        selection = strtok(command, "\n\t ");					//selection = to eidos entolis pou dothike.
        
        if (flag_counter != 0){									//an uparxei kapoia diergasia idi mesa stin lista
            pid = waitpid(0,&status,WNOHANG);       			//!!!!!!!!!!!!!!!![][][][!!!!!~~erwthsh an h prvth parametros prepei na einai -1 
																//WNOHANG = an den uparxei kapoio sima na min perimenei h waitpid
            if (pid != 0){
                if (WIFEXITED(status)) {						//elegxos gia to an uparxoun termatismenes diergasies
                    start_stop();								//an uparxoun na svinontai kai na ksekinaei i ektelesi tis epomenhs
                    list_remove(pid);
                }
                else if (WIFSIGNALED(status)) {
                    start_stop();
                    list_remove(pid);
                }
            }
         }
        
        if (strcmp(selection, "exec") == 0) {					//an i entoli einai exec
            
            name = strtok(NULL,"\n\t ");						//eyresi tou onomatos tou programmatos
            args = strtok(NULL,"\n\t ");						//diaxwrismos twn arguments apo tin upoloipi entolh
            
            pid = fork();										//dimiourgia paidioy
            if (pid==0){										//dimiourgia tou komvou sto paidi
				
                pid = getpid();
            
                node = list_insert(pid, name, args);
                
                siguser1.sa_handler = SIG_DFL;
                siguser1.sa_flags = SA_RESTART;
                sigaction(SIGUSR1, &siguser1 , NULL);
                
                ret_value = execv(node->name_list, node->args);		//ektelesi tis diergasias me execv
                if (ret_value ==-1){
                    fprintf(stderr, "Error in function execv: %s\n", strerror(errno));
                    exit(-1);
                }
            }
            
            list_insert(pid,name,args);								//dimiourgia tou komvou ston patera
            
            if (flag_counter == 0){									//an ayti i diergasia pou prostithetai einai i monh sto programma
                
                setitimer(ITIMER_REAL, &timer, NULL);				//na ksekinaei i ektelesi tis apeytheias kai na midenizetai to alarm
                run_change(pid, 1);
                flag_counter++;
            }
            else{
                
                for (curr = head->next; (curr != head) &&(curr->run != 1); curr = curr->next); //an yparxei diergasia pou trexei idi
				if (curr != head){															//na min ksekinaei h ektelesi tis kainoyrgias diergasias
					ret_value = kill(pid,SIGSTOP);
					if(ret_value == -1){
						fprintf(stderr, "Error in function kill sigstop!\n");
						exit(1);
					}
				}
				else{																		//an den trexei kamia na ksekinaei na trexei ayth
					run_change(pid, 1);
				}
			}
        }
        else if (strcmp(selection, "term") == 0){											//an i entoli einai term
            
            temp_pid = strtok(NULL,"\n\t ");												//pairnei to pid kai to metatrepei se arithmo pid_t
            pid = atoi(temp_pid);
            
            for (curr = head->next; (curr != head) &&(curr->pid != pid); curr = curr->next);	//vriskei tin diergasia , elegxei an trexei
            if ((curr != head) && (curr->run == 1)){											//tin skotwnei kanontas na treksei i epomeni
                
                start_stop();
                list_remove(pid);
            }
            else if(curr != head){																//an den trexei tote apla tin skotwnei				
                kill(pid,SIGTERM);																//SIGTERM termatismos diergasias
                if(ret_value == -1){
                    fprintf(stderr, "Error in function kill sigterm!\n");
                    exit(1);
                }
                list_remove(pid);
            }
        }
        else if (strcmp(selection, "sig") == 0){												//an i entoli einai sig
            temp_pid = strtok(NULL,"\n\t ");
            pid = atoi(temp_pid);
            
            kill(pid, SIGUSR1);																	//mplokarei to sima SIGUSR1
            if(ret_value == -1){
                fprintf(stderr, "Error in function kill sigusr1!\n");
                exit(1);
            }
            
        }
        else if (strcmp(selection, "info") == 0){												//an i entoli einai info ektypwnei oles tis diergasies
            print_list();
            
        }
        else if(strcmp(selection, "quit") != 0) {
            fprintf(stderr,"Wrong command. Please try again!");
        }
        
    }while(strcmp(selection, "quit") != 0);														//stamataei otan o xristis dwsei quit
    
    for (curr = head->next; (curr != head); curr = curr->next){									//termatismos twn zombies an uparxoun
        ret_value = kill(curr->pid,SIGTERM);
        
        if (ret_value == -1) {
			fprintf(stderr, "Error in function kill sigterm!\n");
			exit(1);
		}

    }
    
    destroy_list(); 	                                                                         //katastrofi tis listas
    
    pid = waitpid(0,NULL,0);

    return(0);
    
}
