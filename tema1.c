#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300
#define HOME "/home"

struct Dir;
struct File;

typedef struct Dir{
	char *name; // numele directorului
	struct Dir* parent; // pointer catre parintele directorului(null pentru radacina)
	struct File* head_children_files; // pointer catre primul element de tip File din interiorul directorului
	struct Dir* head_children_dirs; // pointer catre primul element de tip Dir din interiorul directorului
	struct Dir* next; // pointer catre urmatorul element din lista in care se afla directorul
} Dir; // structura de tip director

typedef struct File {
	char *name; // numele fisierului
	struct Dir* parent; // pointer catre directorul parinte
	struct File* next; // pointer catre urmatorul element din lista de fisiere
} File;  // structura de tip fisier

/*interschimbare fisiere*/
void swap_File(File* f1 , File* f2){
	char* aux = f1->name;
	f1->name = f2->name;
	f2->name = aux;
}

/*interschimbare directoare si 
continutul lor*/
void swap_Dir(Dir* d1 , Dir* d2){
	char* aux = d1->name;
	d1->name = d2->name;
	d2->name = aux;

	File* aux2 = d1->head_children_files;
	d1->head_children_files = d2->head_children_files;
	d2->head_children_files = aux2;

	Dir* aux3 = d1->head_children_dirs;
	d1->head_children_dirs = d2->head_children_dirs;
	d2->head_children_dirs = aux3;
}

/*functie recursiva care elibereaza 
memoria pentru intreg sistemul de fisiere si directoare*/
void free_rec(Dir* target){
	Dir* it_dir = target->head_children_dirs;
	Dir* aux_dir;
	while(it_dir != NULL){
		aux_dir = it_dir->next;
		free_rec(it_dir);
		it_dir = aux_dir;
	}

	File* it_file = target->head_children_files;
	File* aux_file;
	while(it_file != NULL){
		aux_file = it_file->next;
		free(it_file->name);
		free(it_file);
		it_file = aux_file;
	}

	free(target->name);
	free(target);
}

void touch (Dir* parent, char* name) {
	File* it = parent->head_children_files;
	Dir* it2 = parent->head_children_dirs;
	/*navigam prin fisiere si verificam daca 
	exista un fisier cu numele dat*/
	while(it != NULL){
		if(strcmp(it->name,name) == 0){
			printf("File already exists\n");
			return;
		}
		it = it->next;
	}

	/*navigam prin directoare si verificam daca 
	exista un director cu numele dat*/
	while(it2 != NULL){
		if(strcmp(it2->name,name) == 0){
			printf("File already exists\n");
			return;
		}
		it2 = it2->next;
	}

	/*creare fisier nou si adaugare la 
	lista de fisiere in cazul in care lista
	este goala si cand se adauga la finalul
	listei*/
	File* new_file = (File*)malloc(sizeof(File));
	new_file->name = (char*)malloc((strlen(name)+1)*sizeof(char));
	memcpy(new_file->name , name , (strlen(name)+1)*sizeof(char));
	new_file->parent = parent;

	if(parent->head_children_files == NULL){
		new_file->next = parent->head_children_files;
		parent->head_children_files = new_file;
		return;
	}
	
	File* it3 = parent->head_children_files;
	while(it3->next != NULL){
		it3 = it3->next;
	}

	it3->next = new_file;
	new_file->next = NULL;
}

void mkdir (Dir* parent, char* name) {
	Dir* it = parent->head_children_dirs;
	File* it2 = parent->head_children_files;
	
	/*verificare daca nu exista deja un 
	director cu numele dat*/
	while(it != NULL){
		if(strcmp(it->name,name) == 0){
			printf("Directory already exists\n");
			return;
		}
		it = it->next;
	}

	/*verificare daca nu exista un fisier 
	cu numele dat*/
	while(it2 != NULL){
		if(strcmp(it2->name,name) == 0){
			printf("File already exists\n");
			return;
		}
		it2 = it2->next;
	}

	/*creare si adaugare director la lista
	de directoare*/
	Dir* new_dir = (Dir*)malloc(sizeof(Dir));
	new_dir->name = (char*)malloc((strlen(name)+1)*sizeof(char));
	memcpy(new_dir->name , name , (strlen(name)+1)*sizeof(char));
	new_dir->parent = parent;
	new_dir->head_children_dirs = NULL;
	new_dir->head_children_files = NULL;

	if(parent->head_children_dirs == NULL){
		new_dir->next = parent->head_children_dirs;
		parent->head_children_dirs = new_dir;
		return;
	}
	
	Dir* it3 = parent->head_children_dirs;
	while(it3->next != NULL){
		it3 = it3->next;
	}

	it3->next = new_dir;
	new_dir->next = NULL;
}

void ls (Dir* parent) {
	Dir* it_dir = parent->head_children_dirs;
	File* it_file = parent->head_children_files;
	
	/*afisare directoare si fisiere folosind iteratori*/
	while(it_dir != NULL){
		printf("%s\n", it_dir->name);
		it_dir = it_dir->next;
	}

	while(it_file != NULL){
		printf("%s\n", it_file->name);
		it_file = it_file->next;
	}

}

void rm (Dir* parent, char* name) {
	File* it = parent->head_children_files;
	if(it == NULL){
		printf("Could not find the file\n");
		return;
	}
	
	/*stergere fisier si dezalocarea numelui si a structurii*/
	if(strcmp(it->name , name) == 0){
		parent->head_children_files = it->next;
		free(it->name);
		free(it);
		return;
	}

	while(it->next != NULL){
		if(strcmp(it->next->name , name) == 0){
			File* aux = it->next->next;
			free(it->next->name);
			free(it->next);
			it->next = aux;
			return;
		}
		it = it->next;
	}
	
	printf("Could not find the file\n");
}

void rmdir (Dir* parent, char* name) {
	Dir* it = parent->head_children_dirs;
	if(it == NULL){
		printf("Could not find the dir\n");
		return;
	}
	
	/*acelasi "mecanism" ca la rm doar ca aici 
	pentru dezalocare am facut o functie auxiliara
	care elibereaza recursiv memoria alocata*/
	if(strcmp(it->name , name) == 0){
		parent->head_children_dirs = it->next;
		free_rec(it);
		return;
	}

	while(it->next != NULL){
		if(strcmp(it->next->name , name) == 0){
			Dir* aux = it->next->next;
			free_rec(it->next);
			it->next = aux;
			return;
		}
		it = it->next;
	}
	printf("Could not find the dir\n");
}

/*parcurg sistemul de directoare si cand gasesc pe cel cu 
numele dat , target va deveni el*/
void cd (Dir** target, char *name) {
	Dir* it = (*target)->head_children_dirs;
	if(strcmp(name , "..") == 0){
		if((*target)->parent != NULL){
			*target = (*target)->parent;
		}
		return;
	}
	
	if(it == NULL){
		printf("No directories found!\n");
		return;
	}
	
	while(it != NULL){
		if(strcmp(it->name , name) == 0){
			*target = it;
			return;
		}
		it = it->next;
	}

	printf("No directories found!\n");
}

/*retin recursiv calea pana in directorul in care
ma aflu intr-un sir alocat dinamic la care concatenez
numele directorului curent*/
char *pwd (Dir* target) {
	if(target->parent == NULL){
		char *s = (char*)calloc(strlen(target->name)+1,sizeof(char));
		memcpy(s,target->name,strlen(target->name)+1);
		return s;
	}
	char *s = pwd(target->parent);
	s = (char*)realloc(s, (strlen(s)+strlen(target->name)+2)*sizeof(char));
	s[strlen(s)] = '/';
	strcat(s,target->name);
	return s;
}

/*sfarsitul programului si eliberarea structurii 
de director folosita*/
void stop (Dir* target) {
	while(target->parent != NULL){
		target = target->parent;
	}
	free_rec(target);
}

/*afisez recursiv sub forma arborescenta sistemul 
de fisiere avand in vedere si cele 4 spatii*/
void tree (Dir* target, int level) {
	Dir* it_dir = target->head_children_dirs;
	while(it_dir != NULL){
		for(int i=0; i<level; i++){
			printf("    ");
		}
		printf("%s\n", it_dir->name);
		tree(it_dir,level+1);
		it_dir = it_dir->next;
	}

	File* it_file = target->head_children_files;
	while(it_file != NULL){
		for(int i=0; i<level; i++){
			printf("    ");
		}
		printf("%s\n", it_file->name);
		it_file = it_file->next;
	}
}

void mv (Dir* parent, char *oldname, char *newname) {
	Dir* it_dir = parent->head_children_dirs;
	File* it_file = parent->head_children_files;
	int exist = 0;

	/*verificare daca nu deja exista un director 
	cu numele newname*/
	while(it_dir != NULL){
		if(strcmp(it_dir->name,newname) == 0){
			exist = 1;
			break;
		}
		it_dir = it_dir->next;
	}

	/*verificare daca nu deja exista un fisier
	cu numele newname*/
	while(it_file != NULL){
		if(strcmp(it_file->name,newname) == 0){
			exist = 1;
			break;
		}
		it_file = it_file->next;
	}

	it_dir = parent->head_children_dirs;
	it_file = parent->head_children_files;

	/*daca nu exista inlocuiesc oldname cu newname si 
	interschimb directorul nou alaturi de continutul lui 
	pana cand ajunge ultimul element din lista*/ 
	while(it_dir != NULL){
		if(strcmp(it_dir->name,oldname) == 0){
			if(exist == 1){
				printf("File/Director already exists\n");
				return;
			}
			free(it_dir->name);
			it_dir->name = (char*)malloc((strlen(newname)+1)*sizeof(char));
			memcpy(it_dir->name , newname , (strlen(newname)+1)*sizeof(char));
			while(it_dir->next != NULL){
				swap_Dir(it_dir, it_dir->next);
				it_dir = it_dir->next;
			}
			return;
		}
		it_dir = it_dir->next;
	}

	/*analog , daca nu exista inlocuiesc oldname cu newname si 
	interschimb fisierul nou pana cand ajunge ultimul
	element din lista*/ 
	while(it_file != NULL){
		if(strcmp(it_file->name,oldname) == 0){
			if(exist == 1){
				printf("File/Director already exists\n");
				return;
			}
			free(it_file->name);
			it_file->name = (char*)malloc((strlen(newname)+1)*sizeof(char));
			memcpy(it_file->name , newname , (strlen(newname)+1)*sizeof(char));
			while(it_file->next != NULL){
				swap_File(it_file , it_file->next);
				it_file = it_file->next;
			}
			return;
		}
		it_file = it_file->next;
	}

	printf("File/Director not found\n");
}

int main () {
	Dir* curr_dir = (Dir*)malloc(sizeof(Dir));
	curr_dir->head_children_dirs = NULL;
	curr_dir->head_children_files = NULL;
	curr_dir->next = NULL;
	curr_dir->name = (char*)malloc((strlen(HOME)+1)*sizeof(char));
	memcpy(curr_dir->name , HOME , (strlen(HOME)+1)*sizeof(char));
	curr_dir->parent = NULL;
	int is_running = 1;
	char* buffer = (char*)malloc(MAX_INPUT_LINE_SIZE*sizeof(char));
	char* it;
	do
	{
		fgets(buffer , MAX_INPUT_LINE_SIZE , stdin);
		int buff_len = strlen(buffer);
		if(buffer[buff_len-1] == '\n'){
			buffer[buff_len-1] = '\0';
		}
		it = strtok(buffer , " ");
		if(strcmp(it , "touch") == 0){
			it = strtok(NULL , " ");
			touch(curr_dir , it);
		}
		else if(strcmp(it , "mkdir") == 0){
			it = strtok(NULL , " ");
			mkdir(curr_dir , it);
		}
		else if(strcmp(it , "ls") == 0){
			ls(curr_dir);
		}
		else if(strcmp(it , "rm") == 0){
			it = strtok(NULL , " ");
			rm(curr_dir , it);
		}
		else if(strcmp(it , "rmdir") == 0){
			it = strtok(NULL , " ");
			rmdir(curr_dir , it);
		}
		else if(strcmp(it , "cd") == 0){
			it = strtok(NULL , " ");
			cd(&curr_dir , it);
		}
		else if(strcmp(it , "pwd") == 0){
			char *s = pwd(curr_dir);
			printf("%s\n",s);
			free(s);
		}
		else if(strcmp(it , "tree") == 0){
			tree(curr_dir,0);
		}
		else if(strcmp(it , "mv") == 0){
			char* str1 = strtok(NULL , " ");
			char* str2 = strtok(NULL , " ");
			mv(curr_dir , str1 , str2);
		}
		else if(strcmp(it , "stop") == 0){
			stop(curr_dir);
			is_running = 0;
		}


	} while (is_running);
	
	free(buffer);
	return 0;
}
