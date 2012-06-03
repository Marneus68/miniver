#include "init.h"

int init(void)
{
    printf("Init command : ");
    
    errno = 0;
    /* creation of the tracking directory */
    if (mkdir(TRACK_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
    {
        switch (errno)
        {
            case EEXIST:
                printf("Tracking directory already present.\
Nothing to do here...\n");
                return 0;
                break;
            case EROFS:
                printf("The file system is read only.");
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
    
    /* creation of the SQLite database */
    sqlite3 *handle;
    char db_path[PATH_MAX];
    strcat(db_path, TRACK_DIR);
    strcat(db_path, "/");
    strcat(db_path, DB_NAME);
    
    if (sqlite3_open(db_path, &handle))
    {
        printf("Error while creating the database.\n");
        sqlite3_close(handle);
        return -1;
    }
    
    /* crate the db tables */
    if (sqlite3_exec(handle, CREATE_TABLE_COMMITS, 0, 0, 0) != SQLITE_OK)
    {
        printf("Error while creating the commit table.\n");
        sqlite3_close(handle);
        return -1;
    }
    
    if (sqlite3_exec(handle, CREATE_TABLE_FILES, 0, 0, 0) != SQLITE_OK)
    {
        printf("Error while creating the file table.\n");
        sqlite3_close(handle);
        return -1;
    }
    
    if (sqlite3_exec(handle, CREATE_TABLE_COMMITED_FILES, 0, 0, 0) != SQLITE_OK)
    {
        printf("Error while creating the association table.\n");
        sqlite3_close(handle);
        return -1;
    }
    
    time_t now;
    time(&now);
    
    struct tm *local;
    local = localtime(&now);
    
    /* do a "first commit" */
    char request[200];
    sprintf(request,
"INSERT INTO commits (timestamp, real_date, user, commit_message) values (\
%i, \"%s\", \"%s\", \"%s\")",
        (int)now, asctime(local), "Miniver", "Project creation.") != 0;
/*    printf("%s", request);*/
    
    if (sqlite3_exec(handle, request, 0, 0, 0) != SQLITE_OK)
    {
        printf("Error while creating the first commit.\n");
        sqlite3_close(handle);
        return -1;
    }
    
    sqlite3_close(handle);
    
    printf("Tracking directory successfully created.\n");
    return 0;
}

int uninit(void)
{
    printf("Uninit command : ");
    
    errno = 0;
    
    rm_r(TRACK_DIR);
    
    if (remove(TRACK_DIR) != 0 )
    {
        printf("Error while deleting the tracking directory : ");
        switch (errno)
        {
            case ENOENT:
                printf("No tracking directory found, move along...\n");
                return 0;
                break;
            case ENOTEMPTY:
                printf("Directory not empty... \
Hey, that's not supposed to happen !");
                break;
            case EROFS:
                printf("The file system is read only.");
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
    printf("Tracking directory successfully removed. Goodbye.\n");
    return 0;
}

int rm_r(const char* e_path)
{
    DIR *dir = opendir(e_path);
    struct dirent *ent;
    
    if (dir == NULL)
        /* error while opening the directory */
        return -1;
    
    /* DA MAGICKS */
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0)
        {
            char temp_path[PATH_MAX];
            strcpy(temp_path, e_path);
            strcat(temp_path, "/");
            strcat(temp_path, ent->d_name);
            
            /* let's test if that entry is a file or a directory */
            DIR *test_dir = opendir(temp_path);
            if(test_dir)
                /* it's a directory, let's remove all of it's content */
                if (rm_r(temp_path) != 0) /* that's a recursive call */
                    return -1;
            if (remove(temp_path) != 0)
                return -1;
        }
    }
    
    if (closedir(dir) != 0)
        return -1;
    return 0;
}

