#include "present.h"

int present_commit(const char* e_commit_message, time_t* e_time)
{
    /* creation of the SQLite database handler */
    sqlite3 *handle;
    if (sqlite3_open(TRACK_DB_PATH, &handle))
    {
        printf("                  \
Error while opening the database for commit presentation.\n                  \
Have you used \"miniver init\" in that directory yet ? \n");
        sqlite3_close(handle);
        return -1;
    }
    
    /* turn the timestamp into a human readble date */
    struct tm *local;
    local = localtime(e_time);
    
    /* do a the commit information insertion */
    char request[200];
    sprintf(request,
"INSERT INTO commits (timestamp, real_date, user, commit_message) values (\
%i, \"%s\", \"%s\", \"%s\")",
        (int) *e_time, asctime(local), getenv("LOGNAME"),e_commit_message) != 0;
/*    printf("%s", request);*/
    
    if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
    {
        printf("Error while submiting the commits.\n");
        sqlite3_close(handle);
        return -1;
    }
    
    sqlite3_close(handle);
    
    return 0;
}


int present_file(const char* e_path, time_t* e_time)
{
    printf(" -> %s : ", e_path);
    
    errno = 0;
    
    /* cherck i the file exists */
    int nb_lines_old = 0,
        nb_lines_modif = 0,
        nb_lines_new = get_file_lines_nb(e_path);
    if (nb_lines_new == -1)
    {
        switch (errno)
        {
            case ENOENT:
                printf("\
This file dosen't exist. Are you sure you typed the name correctly ?");
                break;
            case EACCES:
                printf("You're not alowed to do that.");
                break;
            default:
                printf("%s", strerror(errno));
                break;
        }
        printf("\n");
        return -1;
    }
    
    /* creation of the SQLite database handler */
    sqlite3 *handle;
    sqlite3_stmt * stmt;
    
    if (sqlite3_open(TRACK_DB_PATH, &handle))
    {
        printf("                  \
Error while opening the database for insertion.\n                   \
Have you used \"miniver init\" in that directory yet ? \n");
        sqlite3_close(handle);
        return -1;
    }
    
    /* we get the number of lines of the previous entry IF the file was 
     * already tracked */
    char request[200];
    sprintf(request,
"SELECT nbLines FROM files WHERE filename = \"%s\";", e_path);
    
    sqlite3_prepare_v2(handle, request, strlen(request)+1, &stmt, NULL);
    while(1)
    {
        int s;
        s = sqlite3_step (stmt);
        if (s == SQLITE_ROW)
        {
            nb_lines_old = sqlite3_column_int(stmt, 0);
            nb_lines_modif = nb_lines_new - nb_lines_old;
        }
        else if (s == SQLITE_DONE)
        {
            break;
        }
        else
        {
            printf("Well duh...\n");
            return -1;
        }
    }
    sqlite3_finalize(stmt);
    
    printf("\
Good. This file is %d lines long.(%d lines)\n", nb_lines_new, nb_lines_modif);
    
    /* do the commit insertion */
    sprintf(request,
"INSERT OR IGNORE INTO files (fileName) values (\"%s\")",e_path);
    if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
    {
        printf("\
Error while submiting the commits informations : %s.\n",
                sqlite3_errmsg(handle));
        sqlite3_close(handle);
        return -1;
    }
    
    sprintf(request,
"UPDATE files SET nbLines = %d WHERE fileName = \"%s\" ;",
            nb_lines_new, e_path);
    if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
    {
        printf("\
Error updating the file entry in the database : : %s.\n",
                sqlite3_errmsg(handle));
        sqlite3_close(handle);
        return -1;
    }
    
    sprintf(request,
"INSERT INTO commited_files (timestamp, fileName, nbLinesModif) \
values (%i, \"%s\", %i)",
            (int) *e_time, e_path, nb_lines_modif);
    
    if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
    {
        printf("\
Error while submiting the commits informations : %s.\n",
                sqlite3_errmsg(handle));
        sqlite3_close(handle);
        return -1;
    }
    
    sqlite3_close(handle);
    
    /* else, we update the db file entry with the number of lines */
    /* TODO */
    
    return 0;
}

int present_all_files(const char* e_path, time_t* e_time)
{
    int valid_files = 0;
    
    DIR *dir = opendir(e_path);
    struct dirent *ent;
    
    if (dir == NULL)
        /* error while opening the directory */
        return -1;
    
    /* creation of the SQLite database handler */
    sqlite3 *handle;
    sqlite3_stmt * stmt;
    
    if (sqlite3_open(TRACK_DB_PATH, &handle))
    {
        printf("                  \
Error while opening the database for recursive insertion.\n                   \
Have you used \"miniver init\" in that directory yet ? \n");
        sqlite3_close(handle);
        return -1;
    }
    
    /* DA MAGICKS */
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0)
        {
            int nb_lines = 0;
            
            errno = 0;
            
            /* check if this is not a directory */
            
            DIR *check_dir = opendir(ent->d_name);
            if (check_dir == NULL)
            {
                printf(" -> %s : ", ent->d_name);
                
                /* this is not a dir, we can proceed */
                
                /* cherck if the file exists and we get the number of lines */
                int nb_lines_old = 0,
                    nb_lines_modif = 0,
                    nb_lines_new = get_file_lines_nb(ent->d_name);
                if (nb_lines_new == -1)
                {
                    switch (errno)
                    {
                        case ENOENT:
                            printf("\
This file dosen't exist. Dude, that's not supposed to happen !\n");
                            break;
                        case EACCES:
                            printf("You're not alowed to do that.");
                            break;
                        default:
                            printf("%s", strerror(errno));
                            break;
                    }
                    printf("\n");
                    return -1;
                }
                
                char request[200];
                sprintf(request,
"SELECT nbLines FROM files WHERE filename = \"%s\";", ent->d_name);
                
                sqlite3_prepare_v2(handle, request, strlen(request)+1, &stmt, NULL);
                while(1)
                {
                    int s;
                    s = sqlite3_step (stmt);
                    if (s == SQLITE_ROW)
                    {
                        nb_lines_old = sqlite3_column_int(stmt, 0);
                        nb_lines_modif = nb_lines_new - nb_lines_old;
                    }
                    else if (s == SQLITE_DONE)
                    {
                        break;
                    }
                    else
                    {
                        printf("Well duh...\n");
                        return -1;
                    }
                }
                
                sqlite3_finalize(stmt);
                
                printf("\
Good. This file is %d lines long.(%d lines)\n",
                        nb_lines_new, nb_lines_modif);
                
                sprintf(request,
"INSERT OR IGNORE INTO files (fileName) values (\"%s\")",ent->d_name);
                if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
                {
                    printf("\
Error while submiting the commits informations : %s.\n",
                            sqlite3_errmsg(handle));
                    sqlite3_close(handle);
                    return -1;
                }
                
                sprintf(request,
"UPDATE files SET nbLines = %d WHERE fileName = \"%s\" ;",
                        nb_lines_new, ent->d_name);
                if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
                {
                    printf("\
Error updating the file entry in the database : %s.\n", sqlite3_errmsg(handle));
                    sqlite3_close(handle);
                    return -1;
                }
                
                sprintf(request,
"INSERT INTO commited_files (timestamp, fileName, nbLinesModif) \
values (%i, \"%s\", %i)",
                (int) *e_time, ent->d_name, nb_lines_modif);
                
                if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
                {
                    printf("\
Error while submiting the commits informations : %s.\n",
                            sqlite3_errmsg(handle));
                    sqlite3_close(handle);
                    return -1;
                }
                
                valid_files++;
            }
/*            else*/
/*            {*/
                /* this is a directory, ignoring */
                /* printf("This is a directory. Ignoring.\n"); */
                /* TODO : find a way to handle different sub-diectories in a single tracking db */
/*            }*/
        }
    }
    
    sqlite3_close(handle);
    
    if (!valid_files)
        return -1;
    
    return 0;
}

int get_file_lines_nb(const char* e_path)
{
    FILE * file = fopen(e_path, "r");
    if (file == NULL)
        return -1;
    
    /* get the number of lines in that file */
    int lines_count = 0;
    char ch = '\0';
    while (ch != EOF)
    {
        ch = fgetc(file);
        if (ch == '\n') lines_count++;
    }
    fclose(file);
    
    return lines_count;
}

int file_md5sum(const char* out,const char* e_path)
{
    return 0;
}

