#include "history.h" 

int print_history(const char *file_name[], const int file_nb)
{
    /* TODO: check the that every filename is valid */
    /* reduce valid_file_nb by 1 for every invalid file */
    
    printf("Printing the history for the inputed files : \n\n");
    
    int x, valid_file_nb = 0;
    
    /* we check if the all the files are valid and we draw them in a stair */
    for(x = 0; x < file_nb; x++)
    {
        /* test if the fil exists */
        if (file_exists(file_name[x+2]))
        {
            int i;
            for(i = 0; i < x; i++)
                printf("│");
            printf("┍%s\n", file_name[x+2]);
            valid_file_nb++;
        }
    }
    
    /* we draw a separation */
    for(x = 0; x < valid_file_nb; x++)
    {
        printf("┷");
    }
    printf("\n");
    
    /* creation of the SQLite database handler */
    sqlite3 *handle;
    sqlite3_stmt * stmt;
    /* creation and population of the SQLite request */
    char sqlrequest[100000];
    strcat(sqlrequest, "SELECT ");
    for(x = 0; x < valid_file_nb; x++)
    {
        char temp_request[1000];
        sprintf(temp_request, FILE_SELECT_NBMODIF_SELECT,
                file_name[x+2], "a_field", file_name[x+2], "b_field");
        strcat(sqlrequest, temp_request);
    }
    strcat(sqlrequest, "real_date, commit_message FROM commits;");
    
    if (sqlite3_open(TRACK_DB_PATH, &handle))
    {
        printf("                  \
Error while opening the database.\n                  \
Have you used \"miniver init\" in that directory yet ? \n");
        sqlite3_close(handle);
        return -1;
    }
    sqlite3_prepare_v2(handle, sqlrequest, strlen(sqlrequest)+1, &stmt, NULL);
    
    int * nb_modif = malloc(sizeof(int)*valid_file_nb);
    for(x = 0; x < valid_file_nb; x++)
        nb_modif[x] = 0;
    
    while(1)
    {
        int s;
        s = sqlite3_step (stmt);
        if (s == SQLITE_ROW)
        {
            const unsigned char * real_date;
            const unsigned char * commit_message;
            
            real_date = sqlite3_column_text(stmt, valid_file_nb*2);
            commit_message = sqlite3_column_text(stmt, (valid_file_nb*2)+1);
            
            for(x = 0; x < valid_file_nb; x++)
            {
                int file_state, /* 1 = modified, 0 unmodified */
                    nb_lines_modif;
                file_state = sqlite3_column_int(stmt, x*2);
                nb_lines_modif = sqlite3_column_int(stmt, x*2+1);
                switch (file_state)
                {
                    case 0:
                        if (nb_modif[x] == 0)
                        {
                            printf("┊");
                            break;
                        }
                        else
                        {
                            printf("│");
                            break;
                        }
/*                        printf("%d", nb_modif[x]);*/
                        break;
                    case 1:
                        nb_modif[x]++;
                        if (nb_lines_modif > 0)
                        {
                            printf("+");
                            break;
                        }
                        else if (nb_lines_modif < 0)
                        {
                            printf("-");
                            break;
                        }
                        else
                        {
                            printf("=");
                            break;
                        }
                        break;
                }
            }
/*            free(nb_modif);*/
            
            /* printf (" - %s : %s\n", real_date, commit_message); */
            printf (" : %s\n", commit_message);
        }
        else if (s == SQLITE_DONE)
        {
            break;
        }
        else
        {
            printf("\
Error while happening the results. What have you done ?\n");
            return -1;
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(handle);
    
    printf("\n");
    
    return 0;
}

int file_exists(const char * filename)
{
    FILE * file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

