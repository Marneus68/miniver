#ifndef __MINIVER_INIT_H__
#define __MINIVER_INIT_H__

#include <unistd.h> /* that shit won't work on windoze */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <dirent.h> /* DIR, dirent, */
#include <time.h> /* time(), time_t */

#include <sys/types.h>
#include <sys/stat.h>

#include <sqlite3.h>

#include "constants.h"

/* SQLite statement used to create the "commits" table */
#define CREATE_TABLE_COMMITS "CREATE TABLE commits \
(timestamp INT PRIMARY KEY NOT NULL, real_date TEXT, user TEXT,\
commit_message TEXT NOT NULL)"

/* SQLite statement used to create the "files" table */
#define CREATE_TABLE_FILES "CREATE TABLE files\
(fileName TEXT PRIMARY KEY NOT NULL, nbLines INT, md5sum TEXT)"

/* SQLite statement used to create the "commited_files" table */
#define CREATE_TABLE_COMMITED_FILES "CREATE TABLE commited_files \
(timestamp INT, fileName TEXT, nbLinesModif INT, \
PRIMARY KEY(timestamp, fileName),\
FOREIGN KEY (timestamp) REFERENCES commits(timestamp),\
FOREIGN KEY (fileName) REFERENCES files(fileName))"

/* Create a tracking directory in the current directory if there's none */
int init(void);
/* Deletes the tracking directory of the current directory if it exists */
int uninit(void);

/* Delete all entries in e_path recursively */
int rm_r(const char* e_path);

#endif /* __MINIVER_INIT_H__ */

