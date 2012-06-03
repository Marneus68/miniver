#ifndef __MINIVER_PRESENT_H__
#define __MINIVER_PRESENT_H__

#include <stdlib.h> /* getenv() */
#include <stdio.h> /* printf() */
#include <errno.h> /* errno */
#include <time.h> /* time(), time_t */
#include <dirent.h> /* PATH_MAX */
#include <string.h> /* strcat() */

#include <sqlite3.h>

#include <openssl/md5.h>

#include "constants.h"

/* Inserts the commit informations (commit message: e_commit_message 
 * and timestamp: e_time) */
int present_commit(const char* e_commit_message, time_t* e_time);
/* Check if the file at e_path exists, get his number of lines and insert it 
 * into the database (in the files and commited_files tables) */
int present_file(const char* e_path, time_t* e_time);
/* Get a list of all the files in e_path exists, get their number of lines 
 * and insert them into the database (in the files and commited_files tables) */
int present_all_files(const char* e_path, time_t* e_time);

/* Return the number of lines in the file at e_path */
int get_file_lines_nb(const char* e_path);
/* TODO: Replace the value of the string pointed out by out by the value ot the 
 * md5 sum of the file at e_path */
int file_md5sum(const char* out,const char* e_path);

#endif /* __MINIVER_PRESENT_H__ */

