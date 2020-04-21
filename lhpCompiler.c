/****************************************************************************
 *                                                                          *
 * Copyright (C) 2020 by Iqra Haq                                           *
 *                                                                          *
 * This file is the source code for the prototype software of               *
 * my university project: the lhpCompiler.                                  *
 *                                                                          *
 * lhpCompiler is software that you can redistribute                        *
 * and/or modify under the terms of the MIT License.                        *
 *                                                                          *
 * lhpCompiler is distributed in the hope that it will be useful            *
 * for my university project and/or further research purposes               *
 * but WITHOUT ANY WARRANTY AND LIABILITY.                                  *
 * See the following link for more details:                                 *
 * https://github.com/Iqrahaq/lhpCompiler/blob/master/LICENSE               *
 *                                                                          *
 ****************************************************************************/

/**
 * @file lhpCompiler.c
 * @author Iqra Haq
 * @date 01/01/2020
 * @brief This file contains all of the source code for the lhpCompiler.
 */

// These are the necessary pre-processor directives required for the library functions used by the prototype.
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <string.h>


/**
 * This function gets the current local date and time based on when the function is run. This will be vital for the LHP Log File.
 * 
 * @author Iqra Haq
 * \return A pointer to the Date&Time
 * @date 01/01/2020
 */
char *get_current_date_and_time()
{
    // Variables that support storing date & time variables.
    time_t raw_time;
    struct tm * time_info;

    // Gets the time and date in Unix Timestamp format, i.e. 00:00:00 TTT, MMMM D YYYY.
    time ( &raw_time );
    // Puts the time retrieved into a structure of information so that the certain information is easy to extract.
    time_info = localtime ( &raw_time );

    // Returns the struct time_info as a char * so that all information is returned in a string format ready to print.
    return asctime (time_info);
}

/**
 * This function removes the file extension from a file, i.e. the file "test.php" will become "test.".
 * This function is required to keep the integrity of the file name whilst the program processes its contents.
 * This function doesn't require any output as it involve manipulation of the pointer of the file name that the program has stored.
 * 
 * @author Iqra Haq
 * @param[in] file_name - The name of the file.
 * @date 01/01/2020
 */
void remove_file_extension(char *file_name)
{
	// As file extensions are expected at the end of the file name; "end" is assigned as the pointer to the end of the file name.
    char *end = file_name + strlen(file_name);

    // Checks to see that only the file extension is removed, i.e. anything that isn't a ". \ /" to consider full file paths. Keep checking until unsatisfied.
    while (end > file_name && *end != '.' && *end != '\\' && *end != '/') {
    	// Move the pointer to the left. (Remove will be done from right to left as the pointer is positioned at the very end of the file name)
        --end;
    }
    // Check to make sure that the pointer hasn't exceeded the file name length and that the pointer is pointing to the final ".".
    if ((end > file_name && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
    	// Null terminate the string (signals that the end of the string has been reached).
        *end = '\0';
    } 
}

/**
 * This function opens a file and outputs this as a FILE parameter by using the file name and the type specified as input parameters. 
 * If the file doesn't exist, the file will be created. 
 * The type specified will be the action used by the program to analyse the file, e.g. read, write or append.
 * If there is an issue opening the file, a log of this will be noted to the LHP Log File.
 *
 * @author Iqra Haq
 * @param[in] file_name - The name of the file.
 * @param[in] type - The type of behaviour when opening the file (i.e. read, write, append).
 * @param[out] lhp_log - The log file to store any errors in.
 * \return FILE - Opened File
 * @date 01/01/2020
 */
FILE *file_opener(char *file_name, const char *type, FILE *lhp_log)
{   
    // Local file parameter used to open the file based on the file name and file type specified.
    FILE *in_file_ptr = fopen(file_name, type);
    // Validation to check if there were any issues opening the file.
    if(in_file_ptr == NULL){
    	// Log to be added to the LHP Log file if there were any issues.
        fprintf(lhp_log, "Error opening %s, please try again...\n", file_name);
    	printf("There was an error processing this file. Please check LHP.log for further details!\n");
        exit(1);
    } else {
    	// Successful file open, therefore return the opened FILE pointer.
        return in_file_ptr;
    }
}

/**
 * This function opens the input file and checks to see that the content structure of the file are consistent with an expected LHP file. 
 * If any errors are detected, sufficient output will be given to the user and to the LHP Log file.
 *
 * @author Iqra Haq
 * @param[in] file_name - The name of the input file.
 * @param[in] line - The data of the file to be read.
 * @param[out] lhp_log - The log file to store any errors in.
 * \return FILE - Opened File
 * @date 01/01/2020
 */
void file_checker(FILE *lhp_file, FILE *lhp_log, char *line, char *file_name)
{
	// LHP Tags and the counter for the tags have been initialised to begin with.
    char *head = "<£lhp";
    char *tail = "£>";
    int lhp_counter = 0;
    int head_counter = 0;
    int tail_counter = 0;
  
  	// As the file has already been read line by line in the previous function, rewind it so that the pointer position is put back to the beginning.
    rewind(lhp_file);

    // Variables required for dynamic memory allocation.
    size_t len = 0;
    ssize_t read;

    // Loop through the file, line by line. This includes the use of dynamic memory manipulation with the getline() function.
    while((read = getline(&line, &len, lhp_file)) != -1){

    	// If the line ends with a new line character, replace this with a null terminator (as strstr() searches require null termination).
        char *position;
        if((position = strchr(line, '\n')) != NULL){
            *position = '\0';
        }
        
        // Count the occurences of LHP tags separately and store the count in separate counters.
        if(strstr(line, head) != NULL){
        	head_counter++;
        } else if(strstr(line, tail) != NULL){
            tail_counter++;
        }

        // Check that HTML doesn't contain any double quotations to avoid conflict with printf() encapsulation.
        if ((head_counter + tail_counter) % 2 == 0){
        	if(strstr(line, "\"")){
        		// Log to be added to the LHP Log file detailing this issue.
		    	fprintf(lhp_log, "Error with %s.lhp! This program does not permit double quotation marks in HTML code. Please replace these with single quotation marks!\n", file_name);
		    	printf("There was an error processing this file. Please check LHP.log for further details!\n");
		    	exit(1);
        	}
        }
    }

    // Calculate the total amount of LHP tags in file.
    lhp_counter = head_counter + tail_counter;

    //Validation for too many LHP tags.
    if(lhp_counter > 2){
    	// Log to be added to the LHP Log file detailing this issue.
    	fprintf(lhp_log, "Error with %s.lhp! Too many LHP tags detected in this file.\n", file_name);
    	printf("There was an error processing this file. Please check LHP.log for further details!\n");
    	exit(1);
    // Validation for too little LHP tags.
    } else if (lhp_counter < 1) {
    	// Log to be added to the LHP Log file detailing this issue.
    	fprintf(lhp_log, "Error with %s.lhp! No LHP tags detected in this file.\n", file_name);
    	printf("There was an error processing this file. Please check LHP.log for further details!\n");
    	exit(1);
    // Validation for inconsistent LHP tag pairings.
    } else if (head_counter != tail_counter){
    	// Log to be added to the LHP Log file detailing this issue.
    	fprintf(lhp_log, "Error with %s.lhp! LHP tag pairs are inconsistent. Either a start tag or an end tag is missing!\n", file_name);
    	printf("There was an error processing this file. Please check LHP.log for further details!\n");
    	exit(1);
    }
        
    // As dynamic memory manipulation was involved, freeing the memory once the analysis has succeeded is required.
    free(line);
    
}

/**
 * This function is 1st of the 3 major analysis functions of the program 
 * with the main aim of copying any pre-processor directives 
 * from the LHP File (lhp_file) to the top of the C File (linker_file).
 * This is the first user defined function called in the main function, 
 * therefore, any prints from this function will be at the top of the file.
 * 
 * @author Iqra Haq
 * @param[in] lhp_file - The input file where data will be read from.
 * @param[out] linker_file - The output file where the data will be written to.
 * @param[in] line - The data of the file to be read and written.
 * @date 01/01/2020
 */
void analyse_preprocessor_directives(FILE *lhp_file, FILE *linker_file, char *line)
{
	// Insert the FastCGI Standard I/O Header Library into the linker file to enable FastCGI Functionality.
    fprintf(linker_file, "%s\n", "#include \"fcgi_stdio.h\"");

  	// As the file has already been read line by line in the previous function, rewind it so that the pointer position is put back to the beginning.
    rewind(lhp_file);
    
    // Variables required for dynamic memory allocation.
    size_t len = 0;
    ssize_t read;

    // Loop through the file, line by line. This includes the use of dynamic memory manipulation with the getline() function.
    while((read = getline(&line, &len, lhp_file)) != -1){

    	// Variables for the loops have been made here for clarity (easier to identify that there are multiple "for" loops not just one).
        int i, j;

        // Loop through the line, letter by letter.
        for(i=0; i<strlen(line); i++){

        	// Skip any whitespace.
            while(line[i] == ' ' || line[i] == '\t' || line[i] == '\n' || line[i] == '\r' || line[i] == '\f' || line[i] == '\v'){
                i++;
            }

            // Check to see if the line is a relevant pre-processor directive (i.e. that it starts with a "#include...")
            if((ispunct(line[i])) && (line[i] == '#') && (strstr(line, "#include") != NULL)){
                // Remove any leading or trailing whitespace that the previous check may have omitted by looping through the line twice and comparing contents.
                for(i=0; line[i]==' ' || line[i]=='\t'; i++) {}
                for(j=0; line[i]; i++){
                    line[j++]=line[i];
                }
    			// Null terminate the string (signals that the end of the string has been reached).
                line[j]='\0';
                // Print to file.
                fprintf(linker_file, "%s", line);
            } else {
            	// Stop the loop once the whole line has been analysed so that the next line can be processed.
                break;
            }
        }
    }

    // As dynamic memory manipulation was involved, freeing the memory once the analysis has succeeded is required.
    free(line);
                
}

/**
 * This function is 2nd of the 3 major analysis functions of the program with the main aim
 * of copying any HTML from the LHP File (lhp_file) to the C File (linker_file).
 * 
 * @author Iqra Haq
 * @param[in] lhp_file - The input file where data will be read from.
 * @param[out] linker_file - The output file where the data will be written to.
 * @param[in] line - The data of the file to be read and written.
 * @date 01/01/2020
 */
void analyse_html(FILE *lhp_file, FILE *linker_file, char *line)
{

	// LHP Tags and the counter for the tags have been initialised to begin with.
    char *head = "<£lhp";
    char *tail = "£>";
    int lhp_counter = 0;
  
  	// As the file has already been read line by line in the previous function, rewind it so that the pointer position is put back to the beginning.
    rewind(lhp_file);
  

    // Start the header function for the first portion of HTML.
    fprintf(linker_file, "\n%s\n", "// Header HTML Function.");
    fprintf(linker_file, "%s\n", "void header_html()\n{");
    // Follwiing line allows for the code to be compatible with a web server and therefore viewable via a web browser.
    fprintf(linker_file, "\t%s\n", "printf(\"Content-type: text/html\\n\\n\");");
    
    // Variables required for dynamic memory allocation.
    size_t len = 0;
    ssize_t read;

    // Loop through the file, line by line. This includes the use of dynamic memory manipulation with the getline() function.
    while((read = getline(&line, &len, lhp_file)) != -1){

    	// If the line ends with a new line character, replace this with a null terminator (as strstr() searches require null termination).
        char *position;
        if((position = strchr(line, '\n')) != NULL){
            *position = '\0';
        }
        
        // Count the occurences of LHP tags and store the count in lhp_counter.
        if((strstr(line, head) != NULL) || strstr(line, tail) != NULL){
            lhp_counter++;
        }
        
        // Find the occurence of the start LHP tag (as this indicates the end of the first (header) portion of HTML code.
        if(strstr(line, head) != NULL){
        	// Insert end bracket to end the header HTML function.
            fprintf(linker_file, "}\n");
            // Find the occurence of the corresponding end LHP tag.(as this indicates the start of the final (footer) portion of HTML code.)
        } else if (strstr(line, tail) != NULL){
    		// Start the footer function for the final portion of HTML.
            fprintf(linker_file, "\n%s\n", "// Footer HTML Function.");
            fprintf(linker_file, "%s\n", "void footer_html()\n{");
        }


        // Only target the HTML code by checking the LHP Counter to make sure it is an even number.
        if(lhp_counter % 2 == 0){
        	// Check to see that the "£>" (end/tail tag) isn't included with the HTML code (as the counter changes to even once this line is reached).
            if(strstr(line, tail) == NULL){
            	// Check to not include empty lines as these will also be wrapped around by printf() statements (therefore, printing empty lines).
                if(line[0] != '\0'){
                    // Print HTML code wrapped in "printf()" statements to the linker_file (the C File).
                    // Printing will ensure HTML is sent straight to output, i.e. straight to the browser.
                    fprintf(linker_file, "\tprintf(\"%s\");\n", line);
                }
            }
        } 
    }
    // End bracket for the footer HTML function.
    fprintf(linker_file, "}\n");
    // As dynamic memory manipulation was involved, freeing the memory once the analysis has succeeded is required.
    free(line);
}

/**
 * This function is 3rd of the 3 major analysis functions of the program with the main aim
 * of copying any C code from the LHP File (lhp_file) to the C File (linker_file).
 * 
 * @author Iqra Haq
 * @param[in] lhp_file - The input file where data will be read from.
 * @param[out] linker_file - The output file where the data will be written to.
 * @param[in] line - The data of the file to be read and written.
 * @date 01/01/2020
 */
void analyse_c(FILE *lhp_file, FILE *linker_file, char *line)
{
	// LHP Tags and the counter for the tags have been initialised to begin with.
    char *head = "<£lhp";
    char *tail = "£>";
    int lhp_counter = 0;
    int indent_counter = 0;
    char *cLine = " ";    
  	// As the file has already been read line by line in the previous function, rewind it so that the pointer position is put back to the beginning.
    rewind(lhp_file);

    // Variables required for dynamic memory allocation.
    size_t len = 0;
    ssize_t read;

    // Loop through the file, line by line. This includes the use of dynamic memory manipulation with the getline() function.
    while((read = getline(&line, &len, lhp_file)) != -1){

    	// If the line ends with a new line character, replace this with a null terminator (as strstr() searches require null termination).
        char *position;
        if((position = strchr(line, '\n')) != NULL){
            *position = '\0';
        }

        // Count the occurences of LHP tags and store the count in lhp_counter.
        if((strstr(line, head) != NULL) || strstr(line, tail) != NULL){
            lhp_counter++;
        }

    	// Variables for the loops have been made here for clarity (easier to identify that there are multiple "for" loops not just one).
        int i, j;

        if(strstr(line, "main(") != NULL){
	        // Loop through the line, letter by letter.
	        for(i=0; i<=strlen(line); i++){
                // Check for and count occurences of leading whitespace.
	        	if(line[i] == ' ' || line[i] == '\t'){
	        		indent_counter++;  
	        	}
	    	}
            // Remove certain amount of leading whitespace to clean up C code block for better readability.
            if(lhp_counter % 2 != 0){
                
                cLine = &line[indent_counter-1];
            }           
            printf("%i\n", indent_counter);
    	}

        // Only target the C code by checking the LHP Counter to make sure it is an odd number.
        if(lhp_counter % 2 != 0){
        	// Relevant Pre-Processor Directives have already been processed, therefore, they have been omitted here to avoid duplication and other errors.
            if((strstr(line, "#include") == NULL)){
	        	// Check to see that the "<£lhp" (start/head tag) isn't included with the HTML code (as the counter changes to odd once this line is reached).
                if((strstr(line, head) == NULL)){
                	// Make sure to omit the return statement as footer function call needs to be included first.
            		if(strstr(line, "return") == NULL && strstr(line, "main(") == NULL ){
                        // Print C code statements to the linker_file (the C File). 
                    	// Doesn't need to be wrapped in printf() as C code is the primary language in use.
                    	fprintf(linker_file, "\t\t%s\n", cLine);
                	}         
                }
            } 
        }

        // Locate start of main function.
        if(strstr(line, " main(") != NULL){
            fprintf(linker_file, "%s\n", cLine);
            // Insert relevant FastCGI while statement to allow for C code to be FastCGI compatible.
            fprintf(linker_file, "\t%s\n", "while (FCGI_Accept() >= 0){");
            // Insert header HTML function call into C code block before main.
            fprintf(linker_file, "\t\t%s\n", "header_html();");
        }
            
        // Locate the return statement of the main function.
        if(strstr(line, "return ") != NULL){
        	// Insert footer function call into C code block before main's return statement.
            fprintf(linker_file, "\t\t%s\n", "footer_html();");
            // Insert corresponding end bracket to the finish FastCGI while code block.
            fprintf(linker_file, "\t%s\n", "}");
            // Print remainder C code (expected to be the main's return statement).
            fprintf(linker_file, "%s\n", cLine);
        }

    }
    // As dynamic memory manipulation was involved, freeing the memory once the analysis has succeeded is required.
    free(line);
}

/**
 * This function is the final major function of the program with the main aim
 * of compiling the C File (linker_file) with enough compatibility for interaction with
 * SQL statements for database functionality and FastCGI for web server hosting.
 * 
 * @author Iqra Haq
 * @param[in] file_name - The name of the file to be used.
 * @param[in] command - The pointer to the command.
 * @param[out] lhp_log - The log file to store any errors in. 
 * @date 01/01/2020
 */
void compilation(char *file_name, char *command, FILE *lhp_log)
{
    // Remove file extension so that the same name can be used for the outputted EXE file.
    remove_file_extension(file_name);

    // Calculate enough space for the static aspects of the command and the variable file name and store the value.
    size_t command_length = 96 + ((strlen(file_name)*2));
    // Change memory allocation from NULL (as specified in main) to the calculated value.
    command = realloc(command, command_length);

    // Conditional macro statements to check relevant Operating System is in use.
    #ifdef _WIN32
        // Output relevant log to lhp_log for Windows Operating System as being unusable for program.
        fprintf(lhp_log, "%s\n", "Incorrect Operating System in use (OS: Windows).");
        fprintf(lhp_log, "%s\n", "This program cannot be run on this OS.");
        fprintf(lhp_log, "%s\n", "Please move to a Linux/Unix OS to use this.");
    #elif __linux__
        // Output relevant log to lhp_log for Linux Operating System in use.
        fprintf(lhp_log, "%s\n", "Correct Operating System in use (OS: Linux).");
        // Construct the correct command by inserting file name with correctly appended file extensions and static command aspects.
        strcpy(command, "gcc -g -std=c99 -Wall -o ");
        command = strcat(command, file_name);
        command = strcat(command, ".exe ");
        command = strcat(command, file_name);
        // Include relevant compatibility and append any errors to LHP Log.
        command = strcat(command, ".c -lfcgi `mysql_config --cflags --libs` 2>> LHP.log");
    #elif __unix__
        // Output relevant log to lhp_log for Unix Operating System in use.
        fprintf(lhp_log, "%s\n", "Correct Operating System in use (OS: Unix).");
        // Construct the correct command by inserting file name with correctly appended file extensions and static command aspects.
        strcpy(command, "gcc -g -std=c99 -Wall -lfcgi -o ");
        command = strcat(command, file_name);
        command = strcat(command, ".exe ");
        command = strcat(command, file_name);
        // Include relevant compatibility and append any errors to LHP Log.
        command = strcat(command, ".c -lfcgi `mysql_config --cflags --libs` 2>> LHP.log");
    #else
        // Output relevant log to lhp_log for Other Operating System as being unusable for program.
        fprintf(lhp_log, "%s\n", "Incorrect Operating System in use (OS: Other).");
        fprintf(lhp_log, "%s\n", "This program cannot be run on this OS.");
        fprintf(lhp_log, "%s\n", "Please move to a Linux/Unix OS to use this.");
    #endif
    
    // Issue the command with system() and wrap with a check to make sure that the command was successfully executed.
    if(system(command) == 0){
        // Output relevant log for successful execution to the LHP Log.
        fprintf(lhp_log, "The file '%s.exe' has been successfully compiled.\n", file_name);
    }

    // Free the allocated memory used for the command.
    free(command);

}


/**
* This is main function of the program. This is where all the functions will be called.
* The order of the statements within the main function matter as all statements are 
* essentially global statements and have effects on the user-defined functions.
* Argument variable parameters passed at run-time have been used rather than continuous
* user interaction for ease of use and essential interaction only.
*
* @author Iqra Haq
* @param[in] argc - Number of argument parameters supplied by user.
* @param[in] argv - List of arguments supplied by user.
* \return int - Error status.
* @date 01/01/2020
*/
int main (int argc, char* argv[])
{

    // Relevant FILE variables created. (Note: EXE File is created during compilation function's system() call)
    FILE *lhp_file = NULL;
    FILE *linker_file = NULL;
    FILE *lhp_log = NULL;

    //Dynamic Memory Allocation (Length of file_name will vary).
    char *command = NULL;
    char *line = NULL;
    char *date_and_time = get_current_date_and_time();

    // lhp_log is not opened with file_opener function as the function requires lhp_log to independantly exist.
    lhp_log = fopen("LHP.log", "a+");
    if(lhp_log == NULL){
        fprintf(stderr, "Error opening LHP.log, please try again...\n");
        // Independant existence means the program will exit with an error if there is an issue opening LHP.Log.
        exit(1);
    } else {
        // Insert the Date & Time at the top of the LHP.Log file.
        fprintf(lhp_log,"\n%s\n", "========================");
        fprintf(lhp_log, "%s", date_and_time);
        fprintf(lhp_log, "%s\n", "========================");
         
    
        // Check to see if wrong number of argument parameters were specified for validation purposes.
        if (argc != 2){
            // Inform the user via LHP.Log accordingly if wrong number of parameters specified.
            fprintf(lhp_log, "%s\n", "Insufficient number of argument parameters supplied!");
            if (argc <= 1){
                // Inform the user via LHP.Log if LHP File (Necessary Input File) hasn't been included.
                fprintf(lhp_log, "%s\n", "No LHP file specified. Please specify the LHP file as an argument parameter.");
            }
            // End the program noting an error occured for incorrect number of argument parameters.
            exit(1);
        } 

        // Calculate and store the length of the file name.
        size_t file_name_length = ((strlen(argv[1]))+1);
        // Dynamic memory allocation used as the file name will always vary.
        char *lhp_file_name = (char *)malloc(file_name_length);
        char *linker_file_name = (char *)malloc(file_name_length);
        // Store the file name into variables for both the LHP file (lhp_file) and the C File (linker_file) (keep file name consistent throughout to avoid confusion.)
        strcpy(lhp_file_name, argv[1]);
        strcpy(linker_file_name, lhp_file_name);


        // Check to see if extension is not included within the file name.
        if(strstr(lhp_file_name, ".lhp") == NULL){
            // Allocate memory for the extension and null terminator also.
            lhp_file_name = realloc(lhp_file_name, file_name_length+5);
            // Append the file extension to the file name if not included.
            strcat(lhp_file_name, ".lhp");
        }

        // Use file_opener function to open the input file as a read only (avoid accidental file modifications)
        lhp_file = file_opener(lhp_file_name, "r", lhp_log);
        

        // Remove the LHP file extension if the C File (linker_file) variable includes it so that the correct file extension can be applied.
        if(strstr(linker_file_name, ".lhp") != NULL){
            remove_file_extension(linker_file_name);
        }
        // Allocate memory for the extension and null terminator also.
        linker_file_name = realloc(linker_file_name, file_name_length+3);
        linker_file_name = strcat(linker_file_name, ".c");
        // Use file_opener function to open the linker file with write permission (as this file needs modification abilities enabled)
        linker_file = file_opener(linker_file_name, "w", lhp_log);


        // Call file_checker function to make sure that the content structure of the input file is suitable for the program.
        file_checker(lhp_file, lhp_log, line, lhp_file_name);

        // Call analyse_preprocessor_directives function to analyse relevant Pre-Processor Directives in LHP File line by line and copy them to the C File (linker_file).
        analyse_preprocessor_directives(lhp_file, linker_file, line);
        // Call analyse_html functon to analyse any HTML in the LHP File line by line and copy them to the C File (linker_file).
        analyse_html(lhp_file, linker_file, line);
        // Call analyse_c to analyse any C in the LHP File line by line and copy them to the C File (linker_file).
        analyse_c(lhp_file, linker_file, line);

        //Rewind the files that were analysed or modified so that the compilation command can process these accordingly as well as future program reruns.
        rewind(lhp_file);
        rewind(linker_file);

        // Call the final user-defined function: compilation to compile the C File (linker_file).
        compilation(linker_file_name, command, lhp_log);
        
        // Close any opened files and free any allocated memory as the program has completed.
        fclose(lhp_file);
        fclose(linker_file);
        fclose(lhp_log);
        free(lhp_file_name);
        free(linker_file_name);      
    }   

    // Main's return function to signify end of program.
    return 0;
}
