#include "builtins.h"

void my_exit(){
	fflush(stdout);
	_exit(0);
}

int my_cp(char **argv){
	int i =0;
	int infile, outfile;
	ssize_t nread;
	char buffer[MAX_ARGS_LENGTH];
	
	while(argv[i] != NULL){
		i++;
	}
	if (i != 3){
		fprintf(stderr, "usage: my_cp input_file output_file\n");
		return (-1);
	}

	if ((infile = open(argv[1], O_RDONLY))== -1){ 
   	fprintf(stderr, "failed opening input file %s\n", argv[1]);
		return(-1); /* error on open infile */
  }
	
	if (access(argv[2], F_OK) == 0) { // file exists
		if (access(argv[2], W_OK) == -1) { //oh no, it's write only
			fprintf(stderr, "failed opening output file %s\n", argv[2]);
			close(infile);
			return -2;
		}
	}
	
	if ((outfile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, PERM))== -1){
   	close(infile);
   	return(-2); /* error on open outfile */
	}

  
	while ( (nread=read(infile, buffer, MAX_ARGS_LENGTH)) > 0){
		if (write(outfile, buffer, nread) < nread){   
  		close(infile);
    	close(outfile);
     	return(-3);  /* error on write */
   	}   
  	}

	close(infile);
	close(outfile);

	if (nread == -1) 
  		return(-4);     /* error on final read */
	else
  		return (0);
 	}

char get_file_type(int st_mode){
	if (S_ISREG(st_mode)){
		return '-';
	}
	else if (S_ISDIR(st_mode)){
		return 'd';
	}
	else if (S_ISLNK(st_mode)){
      return 'l';
   }      
	else if (S_ISCHR(st_mode)){
      return 'c';
   }      
	else if (S_ISBLK(st_mode)){
      return 'b';
   }      
	else if (S_ISFIFO(st_mode)){
      return 'p';
   }      
	else if (S_ISSOCK(st_mode)){
      return 's';
   }
	else{
		return '?';
	}
}

char set_perms(int st_mode, char perms_array[10]){
	// the permission are read, write, execute
	// for loop that's like "for perm in perms, if on print letter, else -"
	if ((st_mode & S_IRUSR) == 0){
		perms_array[0] = '-';
	}
	else{
		perms_array[0] = 'r';
	}
	if ((st_mode & S_IWUSR) == 0){
      perms_array[1] = '-';
   }
   else{
      perms_array[1] = 'w';
   } 
	if ((st_mode & S_IXUSR) == 0){
      perms_array[2] = '-';
   }
	else{
		perms_array[2] = 'x';
	}


   if ((st_mode & S_IRGRP) == 0){
      perms_array[3] = '-';
   }
   else{
      perms_array[3] = 'r';
   } 
   if ((st_mode & S_IWGRP) == 0){
      perms_array[4] = '-';
   }
   else{
      perms_array[4] = 'w';
   } 
   if ((st_mode & S_IXGRP) == 0){
      perms_array[5] = '-';
   }
   else{
      perms_array[5] = 'x';
   } 


   if ((st_mode & S_IROTH) == 0){
      perms_array[6] = '-';
   }
   else{
      perms_array[6] = 'r';
   } 
   if ((st_mode & S_IWOTH) == 0){
      perms_array[7] = '-';
   }
   else{
      perms_array[7] = 'w';
   } 
   if ((st_mode & S_IXOTH) == 0){
      perms_array[8] = '-';
   }
   else{
      perms_array[8] = 'x';
   } 
	//perms_array[9] is null
	perms_array[9] = '\0';
	return *perms_array;
}

int my_ls(char **argv){
	DIR *directory;
	struct dirent *entry;
	struct stat stat_info;
	int i=0;
	long int max_size;
	long int size;
	char longest_file [PATH_MAX] = {0};
	int max_len;
	char file_type;
	char perms_array [10] = "rwxrwxrwx";
	struct passwd *pwd;
	struct group *grp;
	struct tm *tmp;
	char time_string[80];
	//time_string[0] = '\0';
	char temp_string[80];
	int num_links;
	char name [PATH_MAX];

	while (argv[i] != NULL){
		i++;
	}

	if (i != 1){
		fprintf(stderr, "usage:my_ls, with no additional parameters\n");
		return (-1);
	}

	directory = opendir(".");
	while ((entry = readdir(directory)) != NULL){
		// skip the things without names
		if (strcmp(entry->d_name, ".") ==0 || strcmp(entry->d_name, "..") ==0){
			continue;
		}

		if (stat(entry->d_name, &stat_info) == 0){
			if (stat_info.st_size > max_size){
				max_size = stat_info.st_size;
				strncpy(longest_file, entry->d_name, PATH_MAX - 1);
			}
		}
	}

	max_len = 1 + snprintf(NULL, 0, "%li", max_size);

	rewinddir(directory);
	while ((entry = readdir(directory)) != NULL){
		if (strcmp(entry->d_name, ".") ==0 || strcmp(entry->d_name, "..") ==0){
			continue;
		}
		//character for file type
		if (stat(entry->d_name, &stat_info) == 0){
			file_type = get_file_type(stat_info.st_mode);
			
			// name of the file
			bzero(name, PATH_MAX);
			strcpy (name, entry->d_name);

			//the size of each file
			size = stat_info.st_size;

			// next need to find the permissions
			perms_array[10] = set_perms(stat_info.st_mode, perms_array);
			
			//next, need to use getpwuid and getgrgird
			pwd = getpwuid(stat_info.st_uid);
			
			grp = getgrgid(stat_info.st_gid);
			
			// time extraction
			time_t modified_time = stat_info.st_mtime;
			tmp = localtime(&stat_info.st_mtime);
			bzero(time_string, 80);	
			
			strftime(temp_string, sizeof(temp_string), "%b", tmp);
			strcat(time_string, temp_string);
			
			strftime(temp_string, sizeof(temp_string), " %-d", tmp);
			strcat(time_string, temp_string);
			
			strftime(temp_string, sizeof(temp_string), " %Y", tmp);
			if (strcmp(temp_string, "2025")== 0){
				strcat(time_string, temp_string);
			}
			
			strftime(temp_string, sizeof(temp_string), " %H:",tmp);
			strcat(time_string, temp_string);
			
			strftime(temp_string, sizeof(temp_string), "%m", tmp);
			strcat(time_string, temp_string);
			
			//num of links
			num_links = stat_info.st_nlink;		
		}
	
		// want to print the file_type, permission, nimber of links,
		// user and groups
			printf("%c%s %d %s %s", file_type, perms_array, num_links, pwd->pw_name
			, grp->gr_name);
		// now the size
			printf("%*ld ", max_len, size);
			printf("%s %s\n", time_string, name);
	}

	closedir(directory);
	return 0;
}

int my_chmod(char **argv){
   int j =0;
	int k = 0;
	int temp_mode[4] = {0};
	int input_argv[3];
   int input = atoi(argv[1]);
	unsigned int i;
	int int_mode = 0;
	char mode[12];
	//ok, how about modulus calc

	while(argv[j] != NULL){
      j++;
   }
   if (j != 3){
      fprintf(stderr, "usage: my_chmod octal_perms filename\n");
   	return (-1);
	}
	
	while (input >0){
		input_argv[k] = input%10;
		printf("input is: %d\n", input_argv[k]);
		input /= 10;
		k++;
	}
	printf("k is: %i\n",k);
	//reverse the order
	for (int i=0; i<=k; i++){
		temp_mode[i+1] = input_argv[k-i];
		printf("temp_mode is: %d\n", temp_mode[i+1]);
	}


	//now want to convert temp_mode into int
	for (int i=0; i<=4; i++){
		int_mode = int_mode*10 + temp_mode[i];
	}
	printf("int_mode is: %i\n", int_mode);

	//mode = itoa(int_mode);
	sprintf(mode, "%d", int_mode);
	i = strtoul(mode, 0, 8);
	printf("strtoul is now: %o\n", i);

	//ok, now invoke chmod
	if (chmod(argv[2], i) == -1){
		fprintf(stderr, "Error changing permissions for %s: %s\n", 
			argv[2], strerror(errno));
	}
	else{
		printf("Permissions changed successfully for %s to %o\n", 
			argv[2],i);
		return EXIT_SUCCESS;
	}
	return 0;
}
