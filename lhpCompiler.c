// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


// Final Year Project Program.

//File Creation Function.
FILE *file_opener(char *file_name, const char *type, FILE *lhp_log)
{

    FILE *in_file_ptr;
    
    in_file_ptr = fopen(file_name, type);
    if(in_file_ptr == NULL){
        fprintf(lhp_log, "Error opening %s, please try again...\n", file_name);
        exit(1);
    } else {
        return in_file_ptr;
    }
}



void remove_file_extension(char *file_name)
{
    char *end = file_name + strlen(file_name);

    while (end > file_name && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if ((end > file_name && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }  
}

char *get_current_date_and_time()
{
    time_t raw_time;
    struct tm * time_info;

    time ( &raw_time );
    time_info = localtime ( &raw_time );

    return asctime (time_info);
}

void analyse_preprocessor_directives(FILE *lhp_file, FILE *exe_file, char *line)
{
    
    //PreProcessor Directives will always start with '#'.
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, lhp_file)) != -1){

        //Identifying
        int i, j;
        for(i=0; i<strlen(line); i++){

            while(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r' || line[i] == '\f' || line[i] == '\v'){
                i++;
            }
            
            if(ispunct(line[i]) && line[i] == '#'){
                //Remove any leading or trailing whitespace.
                for(i=0; line[i]==' ' || line[i]=='\t'; i++) {}
                for(j=0; line[i]; i++){
                    line[j++]=line[i];
                }
                line[j]='\0';
                //Print to file (this function will be called first, therefore, print will be to top of the file).
                fprintf(exe_file, "%s", line);
            } else {
                break;
            }
        }
    }

    free(line);
                
}

// Function to analyse HTML in LHP File so that any HTML code will be inserted into printf() and all output will be sent to the EXE file.
void analyse_html(FILE *lhp_file, FILE *exe_file, char *line)
{

    char *head = "<£lhp";
    char *tail = "£>";
    int lhp_counter = 0;
  
    rewind(lhp_file);
  
    //Header HTML function.
    fprintf(exe_file, "\n%s\n", "// Header HTML Function.");
    fprintf(exe_file, "%s\n", "void header_html()\n{");
    fprintf(exe_file, "\t%s\n", "printf(\"Content-type: text/html\\n\\n\");");
    
    //Dynamic memory allocation.
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, lhp_file)) != -1){

        char *position;
        if((position = strchr(line, '\n')) != NULL){
            *position = '\0';
        }
        
        //count occurences of LHP start and end tags.
        if((strstr(line, head) != NULL) || strstr(line, tail) != NULL){
            lhp_counter++;
        }
        
        if(strstr(line, head) != NULL){
            fprintf(exe_file, "}\n");
        } else if (strstr(line, tail) != NULL){
            fprintf(exe_file, "\n%s\n", "// Footer HTML Function.");
            fprintf(exe_file, "%s\n", "void footer_html()\n{");
        }


        // C code sent straight to EXE file.
        if(lhp_counter % 2 == 0){
            if(strstr(line, tail) == NULL){
                if(line[0] != '\0'){
                    //Escape "" in HTML to avoid conflict with printf().
                    fprintf(exe_file, "\tprintf(\"%s\");\n", line);
                }
            }
        } 
    }
    fprintf(exe_file, "}\n");
    free(line);
    //Account for any inline HTML?
}


void analyse_c(FILE *lhp_file, FILE *exe_file, char *line)
{
    char *head = "<£lhp";
    char *tail = "£>";
    int lhp_counter = 0;
    
    rewind(lhp_file);

    //Dynamic memory allocation.

    size_t len = 0;
    ssize_t read;
    while((read = getline(&line, &len, lhp_file)) != -1){

        char *position;
        if((position = strchr(line, '\n')) != NULL){
            *position = '\0';
        }


        //count occurences of LHP start and end tags.
        if((strstr(line, head) != NULL) || strstr(line, tail) != NULL){
            lhp_counter++;
        }

        //Don't include Preprocessor Directives (already included at the top).
        // C code sent straight to EXE file.

        if(lhp_counter % 2 != 0){
            if((strchr(line, '#') == NULL)){
                if((strstr(line, head) == NULL)){
                    if((strstr(line, "return 0") == NULL)){
                       fprintf(exe_file, "%s\n", line);
                   }
                }
            } 
        }
            
        //Insert Function Calls of HTML back into C.
        if(strstr(line, " main(") != NULL){
            fprintf(exe_file, "\t\t%s\n", "header_html();");
        }

        //Insert Function Calls of HTML back into C.
        if(strstr(line, "return 0") != NULL){
            fprintf(exe_file, "\t\t%s\n", "footer_html();");
            fprintf(exe_file, "%s\n", line);
        }  
    }
    free(line);
}

// Function change EXE file to compiled version of EXE file.
void compile_exe_file(char *file_name, char *command, FILE *lhp_log)
{

    remove_file_extension(file_name);

    // 80 is the command length without file_names.
    size_t command_length = 89 + ((strlen(file_name)*2));
    command = realloc(command, command_length);

    #ifdef _WIN32
        fprintf(lhp_log, "%s\n", "Incorrect Operating System in use (OS: Windows).");
        fprintf(lhp_log, "%s\n", "This program cannot be run on this OS.");
        fprintf(lhp_log, "%s\n", "Please move to a Linux/Unix OS to use this.");
    #elif __linux__
        fprintf(lhp_log, "%s\n", "Correct Operating System in use (OS: Linux).");
        strcpy(command, "gcc -g -std=c99 -Wall -o ");
        command = strcat(command, file_name);
        command = strcat(command, ".exe ");
        command = strcat(command, file_name);
        command = strcat(command, ".c `mysql_config --cflags --libs` 2>> LHP.log");
    #elif __unix__
        fprintf(lhp_log, "%s\n", "Correct Operating System in use (OS: Unix).");
        strcpy(command, "gcc -g -std=c99 -Wall -o ");
        command = strcat(command, file_name);
        command = strcat(command, ".exe ");
        command = strcat(command, file_name);
        command = strcat(command, ".c `mysql_config --cflags --libs` 2>> LHP.log");
    #else
        fprintf(lhp_log, "%s\n", "Incorrect Operating System in use (OS: Other).");
        fprintf(lhp_log, "%s\n", "This program cannot be run on this OS.");
        fprintf(lhp_log, "%s\n", "Please move to a Linux/Unix OS to use this.");
    #endif
    
    //If errors mention that file couldn't be compiled and list.
    if(system(command) == 0){
        fprintf(lhp_log, "The file '%s.exe' has been successfully compiled.\n", file_name);
    }

    free(command);

}


//Main Function
int main (int argc, char* argv[])
{

    FILE *lhp_file;
    FILE *exe_file;
    FILE *lhp_log;

    //Dynamic Memory Allocation (Length of file_name will vary).
    char *command = NULL;
    char *line = NULL;
    char *date_and_time = get_current_date_and_time();

    lhp_log = fopen("LHP.log", "a+");
    //printf("lhp_log = %p\n", &lhp_log);
    if(lhp_log == NULL){
        fprintf(stderr, "Error opening LHP.log, please try again...\n");
        exit(1);
    } else {
        fputs("\n========================\n", lhp_log);
        fputs(date_and_time, lhp_log);
        fputs("========================\n", lhp_log);
        fflush(lhp_log);     
    
       if (argc != 2){
            fprintf(lhp_log, "%s\n", "Insufficient number of argument parameters supplied!");
            if (argc <= 1){
                fprintf(lhp_log, "%s\n", "No LHP file specified. Please specify the LHP file as an argument parameter.");
            }
            exit(1);
        } 

        size_t file_name_length = ((strlen(argv[1]))+1);
        //Dynamic Memory Allocation (Length of file_name will vary).
        char *lhp_file_name = (char *)malloc(file_name_length);
        char *exe_file_name = (char *)malloc(file_name_length);
        strcpy(lhp_file_name, argv[1]);
        strcpy(exe_file_name, lhp_file_name);


        // Take into account that the user may not enter file with extension, therefore, reallocate memory accordingly.
        if(strstr(lhp_file_name, ".lhp") == NULL){
            lhp_file_name = realloc(lhp_file_name, file_name_length+5);
            strcat(lhp_file_name, ".lhp");
        }

        lhp_file = file_opener(lhp_file_name, "r", lhp_log);
        

        // Take into account the file extension, therefore, reallocate memory accordingly.
        if(strstr(exe_file_name, ".lhp") != NULL){
            remove_file_extension(exe_file_name);
        }
        exe_file_name = realloc(exe_file_name, file_name_length+3);
        exe_file_name = strcat(exe_file_name, ".c");
        exe_file = file_opener(exe_file_name, "w", lhp_log);


        analyse_preprocessor_directives(lhp_file, exe_file, line);
        analyse_html(lhp_file, exe_file, line);
        analyse_c(lhp_file, exe_file, line);
        rewind(lhp_file);
        rewind(exe_file);

        compile_exe_file(exe_file_name, command, lhp_log);
        
        fclose(lhp_file);
        fclose(exe_file);
        fclose(lhp_log);
        free(lhp_file_name);
        free(exe_file_name);      
    }   

    return 0;
}
