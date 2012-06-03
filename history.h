#ifndef __MINIVER_HISTORY_H__
#define __MINIVER_HISTORY_H__

#include <stdio.h> /* printf() */
#include <string.h>

#include <sqlite3.h>

#include "constants.h"

//#define FILE_SELECT "( SELECT COUNT(*) FROM commited_files WHERE commited_files.timestamp = commits.timestamp AND fileName = \"%s\" ) AS %s , "
//#define NB_MODIF_SELECT "( SELECT nbLinesModif FROM commited_files WHERE commited_files.timestamp = commits.timestamp AND fileName = \"%s\" ) AS %s , "

#define FILE_SELECT_NBMODIF_SELECT "\
( SELECT COUNT(*) FROM commited_files WHERE commited_files.timestamp = commits.timestamp AND fileName = \"%s\" ) AS %s , \
( SELECT nbLinesModif FROM commited_files WHERE commited_files.timestamp = commits.timestamp AND fileName = \"%s\" ) AS %s , "

/* Print the history of all the files in file_name[], file_nd describing the 
 * numer of files in file_name[] */
int print_history(const char *file_name[], const int file_nb);

/* Check if the filename corresponds to an existing file */
int file_exists(const char * filename);

#endif /* __MINIVER_HISTORY_H__ */

