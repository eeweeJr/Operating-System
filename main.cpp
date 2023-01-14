/*
 * The Tree Command
 * SKELETON IMPLEMENTATION TO BE FILLED IN FOR TASK 3
 */

#include <infos.h>

// global counters for number of files within tree, and number of directories within tree
int filecount;
int dircount;

/**
 * checks for the location of the character within a 'string'
 * @param string : array of characters
 * @param check : value we are searching character array for
 * @return integer value demsontrating the position of the character in the 'string'
 */
int contains (const char *string,char check){
	int count=0;
	while(*string) {
		count++;
		if (*string==check){
			return count;
		}
	}
	return 0;
}

/**
 * Checks if filename follows the rules required by the regex pattern and hence should be opened and printed
 * @param filename :name of file which we are checking against the regex 
 * @param pattern : regex pattern that describes the filenames which we are allowed to display
 * @return returns true if file follows the regex rules and should be printed and/or opened, returns false if not
 */
bool follows_rules (const char *filename, const char *pattern){
	int length = strlen(pattern);
	if (strlen(filename)>strlen(pattern)){
		length = strlen(filename);
	}
	for (int i =0; i<length;i++)
	{
		if (pattern[i]=='(') {
			while (pattern[i]!=')'){
				if (pattern[i]=='-'){

				}
				else{

				}
				i++;	
			}
		}
		else if (pattern[i]!=filename[i] && (pattern[i+1]!='*'|| pattern[i+1]!='?')){
			return false;
		}
		else if (pattern[i+1]=='?'){
			if (i==0){
				i++;
			}
			else if (filename[i] != pattern[i] && (((pattern[i-1]!='*')||(pattern[i-1]!='?')) &&pattern[i-1]!=filename[i])){
				return false;
			}
		}
		else if (pattern[i+1]=='*'){
			if (i==0){
				int j = i;
				while (filename[i]==pattern[j]){
					i++;
				}
				i++; 
			}
			else if ((filename[i] == pattern[i] || filename[i] == pattern[i-1])){
				int j = i;
				while (filename[i]==pattern[j]){
					i++;
				}
				i++;
			}
			else{
				return false;
			}
		}
	}
	return true;
}

/**
 * checks if the file is the last in the directory
 * @param path_check : path we are searching through in order
 * @param name : name of file we are attempting to locate the location of
 * @return returns true if the file is the final in the directory, false if not
 * 
 */
bool checkLastEntry(const char* path_check, const char*name){
	HDIR dir = opendir(path_check, 0);
	if (is_error(dir)) {
		return false;
	}
	int check = 0;
	struct dirent de;
    while(readdir(dir, &de)) {	
		if (strcmp(de.name, name)==0){
			check = 1;
		}
		else{
			check=0;
		}
	}
	if (check == 1) {
		return true;
	}
	return false;
}

/**
 * prints preceding spaces or lines, demonstrating files structure with indentation.
 *@param path: path which we are starting from
 *@param originalPath: path which we are stopping at, dont want to look further back than.(e.g /usr)
 */
void add_spaces(const char *path, const char *originalPath)
{
	char path_check[strlen(path)];
	char name[strlen(path)];
	char spaces[100];
	int count=0;
	for (int i = strlen(path); i >=strlen(originalPath); i--){
		if (path[i]=='/') {
			for(int x =0;x<strlen(path);x++){
				path_check[x]=NULL;
				name[x]=NULL;
			}
			
			for (int j = 0;j<i;j++){
				path_check[j]=path[j];
				
			}
			for (int q = i+1; q<=strlen(path);q++){
				if (path[q] == '/'){
					break;
				}
				else{
					name[q-(i+1)]=path[q];
				}
			}
			if (checkLastEntry(path_check,name)){
				for (int i =0;i<3;i++){
					spaces[count]=' ';
					count++;
				}
			}
			else
			{
				for (int i =0;i<2;i++){
					spaces[count]=' ';
					count++;
				}
				spaces[count]='|';
				count++;
			}
		}
	}
	for(int i =count-1;i>=0;i--){
			printf("%c",spaces[i]);
	}
}

/**
 * main recursive function for accessing and printing all required files and directories
 * @param path: current directory path which we are checking and exploring through
 * @param pattern: pattern of regex if applicable
 * @param originalPath: path from which this function is first called, and is called within the cmdline
 * @param countcheck: integer value to demonstrate when function has been called for the last time recursively.
 */
void print_directory (const char *path, const char *pattern, const char *originalPath,int countcheck)
{	
	
	HDIR dir = opendir(path, 0);
	if (is_error(dir)) {
		filecount++;
		//move back a recursive step as the file is unopenable hence not a directory
		return;
	}
	countcheck++;
	dircount++;
	struct dirent de;
    while(readdir(dir, &de)) {
		if (follows_rules(de.name, pattern) || pattern==""){
			
			add_spaces(path,originalPath);
			printf("|--- %s \n", de.name);
			//string comprehension for appending new filename onto current path for next recursive call
			char next_path[strlen(path)+strlen(de.name)+2];
			for (int i = 0; i < strlen(path); i++) {
				next_path[i]=path[i];
			}
			next_path[strlen(path)]='/';
			for (int j = strlen(path)+1; j < strlen(de.name)+strlen(path)+1; j++) {
				next_path[j]=de.name[j-strlen(path)-1];
			}
			next_path[strlen(path)+strlen(de.name)+2] ='\0';

			print_directory(next_path, pattern, originalPath,countcheck);
		}
	}
	countcheck--;
	if (countcheck==0){
		printf("%d directories, %d files \n",dircount-1,filecount);
	}
    return;
}

int main(const char *cmdline)
{	
    const char *path;
	if (!cmdline || strlen(cmdline) == 0) {
		path = "/usr";
	}

	else {
		path =  cmdline;
		char new_path[strlen(cmdline)];
		const char* patternstart = "-P";
		char printpath[strlen(patternstart)];

		for (int i = 0; i < strlen(path); i++) {
			printpath[0]=path[i];
			printpath[1]=path[i+1];
			//check if pattern has occured
			if (strcmp(printpath,patternstart)==0) {
				for (int q = 0; q < i-1; q++) {
					new_path[q] = cmdline[q]; //setting directory path to be character array before '-P' occurance
				}

				if (strlen(new_path)<1) {
					path="/usr";
				}
				else {
					path = new_path;
				}

				//setting regex pattern to be characters in array after '-P' occurance
				char pattern[strlen(cmdline) - strlen(patternstart) - i];
				for (int j = strlen(patternstart)+i; j <= strlen(cmdline); j++) {
					pattern[j-strlen(patternstart)-i]=cmdline[j+1];
				}

				HDIR dir = opendir(path, 0);
				if (is_error(dir)) {
					printf("Unable to open directory '%s' for reading.\n", path);
					return 1;
				}
				
				printf("Directory Listing of '%s':\n", path);
				printf("Using Pattern '%s':\n", pattern);
				print_directory(path, pattern,path,0);
				closedir(dir);
				return 0;
			}
			else {
				printpath[strlen(path)]={0};
			} 

		}
	}


	HDIR dir = opendir(path, 0);
	if (is_error(dir)) {
		printf("Unable to open directory '%s' for reading.\n", path);
		return 1;
	}
	
	printf("Directory Listing of '%s':\n", path);
	//no pattern so recursion must be called as such
    print_directory(path, "",path,0);
	closedir(dir);
    return 0;
}