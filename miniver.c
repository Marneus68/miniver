#ifndef __MINIVER_MAIN_H__
#define __MINIVER_MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcat(), strcmp() */
#include <time.h> /* time(), time_t */
#include <libintl.h>

#include "init.h"
#include "present.h"

void print_present_usage(void)
{
    printf("usage:  miniver present [<filename>]... <message>\n");
}

void print_usage(void)
{
    print_present_usage();
    printf("\
                past [<filename>]...\n\
                history [<filename>]...\n\
                init\n\
                uninit\n\n");
}

/* Asks a "yes or no" question described by question_str and return 1 if the 
 * user typed in an afirmative response, 0 otherwise */
int askyn(const char* question_str)
{
    printf("%s (y, n) : ", question_str);
    char answer[100];
    
    while (strcmp(answer, "yes\n") || strcmp(answer, "no\n") || strcmp(answer, "y\n") || strcmp(answer, "n\n"))
    {
        fgets(answer,100,stdin);
        
        if (!strcmp(answer, "yes\n") || !strcmp(answer, "y\n"))
            return 1;
        else if (!strcmp(answer, "no\n") || !strcmp(answer, "n\n"))
            return 0;
    }
}

int main (int argc, char *argv[])
{
    /* command definition */
    typedef enum {PRESENT, PAST, HISTORY, INIT, UNINIT} mode;
    const char* commands[] = {"present", "past", "history", "init", "uninit"};
    const int len_com_arr = sizeof(commands)/sizeof(char*);
    
    int argc_left = argc;
    
    switch (argc)
    {
        case 1:
            printf("Not enough argruments provided.\n\n");
            print_usage();
            exit(EXIT_SUCCESS);
        default:
            {
                unsigned int    i,
                                _mode = -1;
                for (i = 0; i < len_com_arr; i++)
                {
                    if (argv[1][0] == '-')
                    {
                        switch (argv[1][1])
                        {
                            case 'h':
                                print_usage();
                                exit(EXIT_SUCCESS);
                            case 'v':
                                printf("%s - v%s\n", PROGRAM_NAME, VERSION);
                                exit(EXIT_SUCCESS);
                            default:
                                break;
                        }
                    }
                    if (!strcmp(argv[1], commands[i]))
                    {
                        _mode = i;
                        argc_left = argc_left-2;
                        break;
                    }
                }
                switch (_mode)
                {
                    case PRESENT:
                        printf("Present command : ");
                        if (argc < 3)
                        {
                            printf("not enough parameters provided.\n\n");
                            print_present_usage();
                            printf("\n");
                            exit(EXIT_SUCCESS);
                        }
                        printf("commit message : %s\n", argv[argc-1]);
                        argc_left--;
                        
                        time_t now;
                        time(&now); /* we get the unix epoch time */
                        
                        printf("\
                  unix epoch time : %i\n", (int)now);
                        
                        if (argc > 3)
                        {
                            int files_count = 0;
                            
                            for (argc_left; argc_left > 0; argc_left--)
                            {
                                if (present_file(argv[argc_left+1], &now) == 0)
                                    files_count++;
                            }
                            
                            if (files_count)
                            {
                                if (present_commit(argv[argc-1], &now) != 0)
                                    exit(EXIT_FAILURE);
                                
                                printf("Done.\n");
                                exit(EXIT_SUCCESS);
                            }
                            
                            if (!askyn("No valid files specified. Do you want to check all the directory ?"))
                                exit(EXIT_SUCCESS);
                        }
                        
                        printf("\
Checking for changes in every file in the directory.\n");
                        
                        if (present_all_files("./", &now) == 0)
                        {
                            if (present_commit(argv[argc-1], &now) != 0)
                                exit(EXIT_FAILURE);
                            printf("Done.\n");
                        }
                        exit(EXIT_SUCCESS);
                        
                    case PAST:
                        printf("Past command : "); 
                        if (argc > 2)
                        {
                            
                            
                            exit(EXIT_SUCCESS);
                        }
                        printf("\
No files specified. Reverting every file in the directory to its previous version.\n");
                        
                        /* TODO: add recursive checking and call */
                        
                        exit(EXIT_SUCCESS);
                    case HISTORY:
                        printf("History command : "); 
                        if (argc > 2)
                        {
/*                            printf("You entered %d file names.\n", argc-2);*/
                            
                            print_history(argv ,argc-2);
                            
                            exit(EXIT_SUCCESS);
                        }
                        
                        /* TODO: add recursive check and call */
                        
                        exit(EXIT_SUCCESS);
                    case INIT:
                        if (init() != 0)
                            exit(EXIT_FAILURE);
                        exit(EXIT_SUCCESS);
                    case UNINIT:
                        if (uninit() != 0)
                            exit(EXIT_FAILURE);
                        exit(EXIT_SUCCESS);
                }
                if (i == len_com_arr)
                {
                    printf("%s: unrecognized command.\n\n", argv[1]);
                    print_usage();
                    exit(EXIT_SUCCESS);
                }
            }
            break;
    }
    return EXIT_FAILURE;
}

#endif /* __MINIVER_MAIN_H__ */

