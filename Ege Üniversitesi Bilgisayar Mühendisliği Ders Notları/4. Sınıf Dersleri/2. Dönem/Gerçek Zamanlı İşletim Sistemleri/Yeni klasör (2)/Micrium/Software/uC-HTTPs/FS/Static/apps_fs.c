/*
*********************************************************************************************************
*                                                uC/Apps
*                                      Network Application Modules
*
*                          (c) Copyright 2008-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               File System interface port files are provided in source form to registered
*               licensees ONLY.  It is illegal to distribute this source code to any third
*               party unless you receive written permission by an authorized Micrium
*               representative.  Knowledge of the source code may NOT be used to develop a
*               similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    uC/Apps FILE SYSTEM INTERFACE
*
*                                         STATIC FILE SYSTEM
*
* Filename      : apps_fs.c
* Version       : V1.02
* Programmer(s) : SR
*********************************************************************************************************
* Note(s)       : (1) All files MUST be added prior to the first file or directory access.
*
*                 (2) The application is responsible for validating file name compatibility with external
*                     code modules, if additional restrictions must be imposed (e.g., certain characters
*                     are not allowed).
*                     (a) Importantly, file names should use ONLY the selected path separator character,
*                         ASCII_CHAR_REVERSE_SOLIDUS (= '\\'), to separate path components.  This software
*                         does not check for the common alternative, ASCII_CHAR_SOLIDUS (= '/'), when
*                         parsing file names, so any ASCII_CHAR_SOLIDUS character will be considered part
*                         of the file name component in which it is embedded.  This may break external
*                         code modules that convert names from this module's convention to the other.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#define    APPS_FS_MODULE
#include  <apps_fs.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         FILE DATA DATA TYPE
*********************************************************************************************************
*/

typedef  struct  apps_fs_file_data {
    CPU_CHAR           *NamePtr;                                /* Ptr to file name.                                    */
    void               *DataPtr;                                /* Ptr to file data.                                    */
    CPU_INT32U          Size;                                   /* Size of file, in octets.                             */
    CPU_SIZE_T          NameSimilarity;                         /* Max similarity between name & prev file name.        */
} APPS_FS_FILE_DATA;

/*
*********************************************************************************************************
*                                           FILE DATA TYPE
*********************************************************************************************************
*/

typedef  struct  apps_fs_file {
    APPS_FS_FILE_DATA  *FileDataPtr;                            /* Ptr to file info.                                    */
    CPU_INT32U          Pos;                                    /* File pos, in octets.                                 */
} APPS_FS_FILE;

/*
*********************************************************************************************************
*                                         DIRECTORY DATA TYPE
*********************************************************************************************************
*/

typedef  struct  apps_fs_dir {
    CPU_INT16U          FileDataIxFirst;                        /* File info ix first.                                  */
    CPU_INT16U          FileDataIxNext;                         /* File info ix next.                                   */
    CPU_SIZE_T          DirNameLen;                             /* Len of dir name.                                     */
} APPS_FS_DIR;


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static  APPS_FS_FILE       Apps_FS_FileTbl[APPS_FS_CFG_NBR_FILES];          /* Info about open files.                   */

static  APPS_FS_FILE_DATA  Apps_FS_FileDataTbl[APPS_FS_CFG_NBR_FILES];      /* Data about files on file system.         */

static  APPS_FS_DIR        Apps_FS_DirTbl[APPS_FS_CFG_NBR_DIRS];            /* Info about open dirs.                    */

static  CPU_INT16U         Apps_FS_FileAddedCnt;                            /* Nbr of files added.                      */

static  APPS_FS_DATE_TIME  Apps_FS_Time;                                    /* Date/time of files and directories.      */


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_SIZE_T  Apps_FS_CalcSimilarity(CPU_CHAR  *pname_1,
                                           CPU_CHAR  *pname_2);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           Apps_FS_Init()
*
* Description : Initialize the static file system.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if file system initialized.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Notes       : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_Init (void)
{
    APPS_FS_DIR        *pdir;
    APPS_FS_FILE       *pfile;
    APPS_FS_FILE_DATA  *pfile_data;
    CPU_INT16U          i;


                                                                /* -------------------- INIT FILES -------------------- */
    for(i = 0u; i < APPS_FS_CFG_NBR_FILES; i++) {
        pfile = &Apps_FS_FileTbl[i];

        pfile->FileDataPtr = (APPS_FS_FILE_DATA *)0;
        pfile->Pos         =  0u;
    }

    Apps_FS_FileAddedCnt = 0u;



                                                                /* ------------------ INIT FILE INFO ------------------ */
    for(i = 0u; i < APPS_FS_CFG_NBR_FILES; i++) {
        pfile_data = &Apps_FS_FileDataTbl[i];

        pfile_data->NamePtr        = (CPU_CHAR *)0;
        pfile_data->DataPtr        = (void     *)0;
        pfile_data->Size           =  0u;
        pfile_data->NameSimilarity =  0u;
    }



                                                                /* --------------------- INIT DIRS -------------------- */
    for(i = 0u; i < APPS_FS_CFG_NBR_DIRS; i++) {
        pdir = &Apps_FS_DirTbl[i];

        pdir->FileDataIxFirst = DEF_INT_16U_MAX_VAL;
        pdir->FileDataIxNext  = DEF_INT_16U_MAX_VAL;
        pdir->DirNameLen      = 0u;
    }



                                                                /* --------------------- INIT TIME -------------------- */
    Apps_FS_Time.Second = 0u;
    Apps_FS_Time.Minute = 0u;
    Apps_FS_Time.Hour   = 0u;
    Apps_FS_Time.Day    = 0u;
    Apps_FS_Time.Month  = 0u;
    Apps_FS_Time.Year   = 0u;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                          Apps_FS_AddFile()
*
* Description : Add a file to the static file system.
*
* Argument(s) : pname       Name of the file.
*
*               pdata       Pointer to buffer holding file data.
*
*               size        Size of file, in octets.
*
* Return(s)   : DEF_OK,   if file added.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : none.
*
* Note(s)     : (1) The file name ...
*                   (a) MUST begin with a path separator character.
*                   (b) MUST be no longer than APPS_FS_MAX_PATH_NAME_LEN.
*                   (c) MUST not end with a path separator character.
*                   (d) MUST not duplicate the parent directory of a file already added.
*                   (e) MUST not duplicate a file already added.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_AddFile (CPU_CHAR    *pname,
                              void        *pdata,
                              CPU_INT32U   size)

{
    CPU_INT16S          cmp_val;
    APPS_FS_FILE_DATA  *pfile_data;
    CPU_INT16U          i;
    CPU_SIZE_T          len;
    CPU_SIZE_T          similarity;
    CPU_SIZE_T          similarity_max;


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ------------------ VALIDATE PTRS ------------------- */
    if (pname == (CPU_CHAR *)0) {                               /* Validate NULL name.                                  */
        return (DEF_FAIL);
    }
    if (pdata == (void *)0) {                                   /* Validate NULL buf.                                   */
        return (DEF_FAIL);
    }
#endif

    if (pname[0] != APPS_FS_PATH_SEP_CHAR) {                    /* Require init path sep char (see Note #1a).           */
        return (DEF_FAIL);
    }

    len = Str_Len_N(pname, APPS_FS_MAX_PATH_NAME_LEN + 1u);     /* Validate name len (see Note #1b).                    */
    if (len > APPS_FS_MAX_PATH_NAME_LEN) {
        return (DEF_FAIL);
    }

    if (pname[len - 1u] == APPS_FS_PATH_SEP_CHAR) {             /* Require final char NOT path sep char (see Note #1c). */
        return (DEF_FAIL);
    }



                                                                /* ----------------- CHK FOR DUP FILE ----------------- */
    similarity_max =  0u;
    pfile_data     = &Apps_FS_FileDataTbl[0];
    for (i = 0u; i < APPS_FS_CFG_NBR_FILES; i++) {              /* For each file ...                                    */
                                                                /*               ... chk if name begins with file name..*/
        cmp_val = Str_CmpIgnoreCase_N(pfile_data->NamePtr, pname, len);
        if (cmp_val == 0) {
                                                                /*               ... & following char is path sep char..*/
            if (pfile_data->NamePtr[len] == APPS_FS_PATH_SEP_CHAR) {
                return (DEF_FAIL);                              /* (see Note #1d)       ... file is name of parent dir. */

                                                                /*               ... if following char is NULL        ..*/
            } else if (pfile_data->NamePtr[len] == ASCII_CHAR_NULL) {
                return (DEF_FAIL);                              /* (see Note #1e)       ... file dup's file name.       */
            }
        }

        similarity = Apps_FS_CalcSimilarity(pname, pfile_data->NamePtr);
        if (similarity_max < similarity) {
            similarity_max = similarity;
        }

        pfile_data++;
    }



                                                                /* ---------------- FIND FREE FILE DATA --------------- */
    if (Apps_FS_FileAddedCnt >= APPS_FS_CFG_NBR_FILES) {
        return (DEF_FAIL);
    }

    pfile_data = &Apps_FS_FileDataTbl[Apps_FS_FileAddedCnt];
    Apps_FS_FileAddedCnt++;



                                                                /* --------------------- ADD FILE --------------------- */
    pfile_data->NamePtr        = pname;                         /* Populate file data.                                  */
    pfile_data->DataPtr        = pdata;
    pfile_data->Size           = size;
    pfile_data->NameSimilarity = similarity_max;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                          Apps_FS_SetTime()
*
* Description : Set date/time of files and directories.
*
* Argument(s) : ptime       Pointer to date/time to set.
*
* Return(s)   : DEF_OK,   if time set.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : none.
*
* Note(s)     : (1) This time will be returned in the directory entry for ALL files and directories.
*
*               (2) #### Validate date/time members.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_SetTime (APPS_FS_DATE_TIME  *ptime)
{
#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* -------------- VALIDATE DATE/TIME PTR -------------- */
    if (ptime == (APPS_FS_DATE_TIME *)0) {                      /* Validate NULL date/time.                             */
        return (DEF_FAIL);
    }
#endif



                                                                /* --------------------- SET TIME --------------------- */
    Apps_FS_Time.Second = ptime->Second;
    Apps_FS_Time.Minute = ptime->Minute;
    Apps_FS_Time.Hour   = ptime->Hour;
    Apps_FS_Time.Day    = ptime->Day;
    Apps_FS_Time.Month  = ptime->Month;
    Apps_FS_Time.Year   = ptime->Year;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         DIRECTORY FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          Apps_FS_DirOpen()
*
* Description : Open a directory.
*
* Argument(s) : pname       Name of the directory.
*
* Return(s)   : Pointer to a directory, if NO errors.
*               Pointer to NULL,        otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) The file search ends when :
*                   (a) A file lying in the directory 'pname' is found.
*                   (b) A file with name 'pname' is found.
*                   (c) All files have been examined.
*********************************************************************************************************
*/

void  *Apps_FS_DirOpen (CPU_CHAR  *pname)
{
    CPU_INT16S          cmp_val;
    APPS_FS_DIR        *pdir;
    CPU_SIZE_T          dir_name_len;
    APPS_FS_FILE_DATA  *pfile_data;
    CPU_INT16U          file_data_ix;
    CPU_BOOLEAN         found;
    CPU_INT16U          i;
    CPU_CHAR           *ppath_sep;
    CPU_SR_ALLOC();


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE DIR NAME ----------------- */
    if (pname == (CPU_CHAR *)0) {                               /* Validate NULL name.                                  */
        return ((void *)0);
    }
#endif

                                                                /* Validate name len.                                   */
    dir_name_len = Str_Len_N(pname, APPS_FS_MAX_PATH_NAME_LEN + 1u);
    if (dir_name_len > APPS_FS_MAX_PATH_NAME_LEN) {
        return (DEF_FAIL);
    }
    if (dir_name_len == 0u) {
        return (DEF_FAIL);
    }

    ppath_sep = Str_Char_Last(pname, APPS_FS_PATH_SEP_CHAR);
    if (ppath_sep != (CPU_CHAR *)0) {
        if (pname + dir_name_len - 1u == ppath_sep) {           /* If final dir name char is path sep char ...          */
            dir_name_len--;                                     /* ... dec dir name len to ignore path sep char.        */
        }
    }



                                                                /* --------------------- OPEN DIR --------------------- */
    found       =  DEF_NO;
    pfile_data  = &Apps_FS_FileDataTbl[0];
                                                                /* For each file ...                                    */
    for (file_data_ix = 0u; file_data_ix < Apps_FS_FileAddedCnt; file_data_ix++) {
                                                                /*               ... if name begins with dir name     ..*/
        cmp_val = Str_CmpIgnoreCase_N(pfile_data->NamePtr, pname, dir_name_len);
        if (cmp_val == 0) {
                                                                /*               ... & following char is path sep char..*/
            if (pfile_data->NamePtr[dir_name_len] == APPS_FS_PATH_SEP_CHAR) {
                found = DEF_YES;                                /* (see Note #1b)       ... file lies in dir.           */
                break;

                                                                /*              ... if following char is NULL ...       */
            } else if (pfile_data->NamePtr[dir_name_len] == ASCII_CHAR_NULL) {
                return ((void *)0);                             /* (see Note #1c)       ... dir name is file name.      */
            }
        }

        pfile_data++;
    }

    if (found == DEF_NO) {                                      /* If file data NOT found (see Note #1d) ...            */
        return ((void *)0);                                     /* ... rtn NULL ptr.                                    */
    }



                                                                /* ------------------- FIND FREE DIR ------------------ */
    found =  DEF_NO;
    pdir  = &Apps_FS_DirTbl[0];

    CPU_CRITICAL_ENTER();
    for (i = 0u; i < APPS_FS_CFG_NBR_FILES; i++) {
        if (pdir->FileDataIxFirst == DEF_INT_16U_MAX_VAL) {
            found = DEF_YES;
            break;
        }

        pdir++;
    }

    if (found == DEF_NO) {                                      /* If no free dir found ...                             */
        CPU_CRITICAL_EXIT();
        return ((void *)0);                                     /* ... rtn NULL ptr.                                    */
    }



    pdir->FileDataIxFirst = file_data_ix;                       /* Populate dir data.                                   */
    pdir->FileDataIxNext  = file_data_ix;
    pdir->DirNameLen      = dir_name_len;
    CPU_CRITICAL_EXIT();

    return ((void *)pdir);
}


/*
*********************************************************************************************************
*                                         Apps_FS_DirClose()
*
* Description : Close a directory.
*
* Argument(s) : pdir        Pointer to a directory.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Apps_FS_DirClose (void  *pdir)
{
    APPS_FS_DIR  *pdir_fs;
    CPU_SR_ALLOC();


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ------------------ VALIDATE DIR -------------------- */
    if (pdir == (void *)0) {                                    /* Validate NULL dir ptr.                               */
        return;
    }
#endif



                                                                /* --------------------- CLOSE DIR -------------------- */
    pdir_fs = (APPS_FS_DIR *)pdir;

    CPU_CRITICAL_ENTER();
    pdir_fs->FileDataIxFirst =  DEF_INT_16U_MAX_VAL;
    pdir_fs->FileDataIxNext  =  DEF_INT_16U_MAX_VAL;
    pdir_fs->DirNameLen      =  0u;
    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*                                           Apps_FS_DirRd()
*
* Description : Read a directory entry from a directory.
*
* Argument(s) : pdir        Pointer to a directory.
*
*               pentry      Pointer to variable that will receive directory entry information.
*
* Return(s)   : DEF_OK,   if directory entry read.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) If the entry name contains a path separator character after the path separator
*                   character immediately following directory name, the entry is a directory.
*
*               (2) The maximum similarity between the file name & previous file names is a measure of
*                   the number of shared path components.  If the similarity is greater than the
*                   directory name length, then either :
*                   (a) ... the next path component in the file name (a directory name) was returned
*                           in a previous directory read operation.
*                   (b) ... the portion of the file name shared with a previous file name was examined
*                           in a previous directory read operation, but failed to match the directory
8                           name.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_DirRd (void           *pdir,
                            APPS_FS_ENTRY  *pentry)
{
    CPU_INT16S          cmp_val;
    APPS_FS_DIR        *pdir_fs;
    APPS_FS_FILE_DATA  *pfile_data;
    CPU_INT16U          file_data_ix;
    CPU_BOOLEAN         found;
    CPU_SIZE_T          len;
    CPU_CHAR           *pname_entry;
    CPU_CHAR           *pname_first;
    CPU_CHAR           *ppath_sep;


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ------------------ VALIDATE PTRS ------------------- */
    if (pdir == (void *)0) {                                    /* Validate NULL dir   ptr.                             */
        return (DEF_FAIL);
    }
    if (pentry == (APPS_FS_ENTRY *)0) {                         /* Validate NULL entry ptr.                             */
        return (DEF_FAIL);
    }
#endif

    pdir_fs     = (APPS_FS_DIR *)pdir;
    pname_first =  Apps_FS_FileDataTbl[pdir_fs->FileDataIxFirst].NamePtr;



                                                                /* --------------- FIND NEXT FILE IN DIR -------------- */
    found      =  DEF_NO;
    pfile_data = &Apps_FS_FileDataTbl[pdir_fs->FileDataIxNext];
                                                                /* For each file ...                                    */
    for (file_data_ix = pdir_fs->FileDataIxNext; file_data_ix < Apps_FS_FileAddedCnt; file_data_ix++) {

        if (pfile_data->NameSimilarity <= pdir_fs->DirNameLen) {/*        ... w/ name NOT found prev'ly (see Note #2) ..*/

                                                                /*               ... if name begins with dir name     ..*/
            cmp_val = Str_CmpIgnoreCase_N(pfile_data->NamePtr, pname_first, pdir_fs->DirNameLen);
            if (cmp_val == 0) {
                                                                /*               ... & following char is path sep char..*/
                if (pfile_data->NamePtr[pdir_fs->DirNameLen] == APPS_FS_PATH_SEP_CHAR) {
                    found = DEF_YES;                            /*                      ... file lies in dir.           */
                    break;
                }
            }
        }

        pfile_data++;
    }

    if (found == DEF_NO) {                                      /* If file data NOT found ...                           */
        return (DEF_FAIL);                                      /* ... end of dir.                                      */
    }



                                                                /* ------------------ GET ENTRY INFO ------------------ */
    pname_entry = pfile_data->NamePtr + pdir_fs->DirNameLen + 1u;
    ppath_sep   = Str_Char_N(pname_entry, APPS_FS_MAX_PATH_NAME_LEN, APPS_FS_PATH_SEP_CHAR);
    if (ppath_sep == (CPU_CHAR *)0) {                           /* Chk if entry is file ...                             */
        Str_Copy_N(pentry->Name, pname_entry, APPS_FS_MAX_PATH_NAME_LEN);
        pentry->Attrib = APPS_FS_ENTRY_ATTRIB_RD;
        pentry->Size   = pfile_data->Size;

    } else {                                                    /*                      ... or dir (see Note #1).       */
        len = (CPU_SIZE_T)(ppath_sep - pname_entry);
        Str_Copy_N(pentry->Name, pname_entry, len);
        pentry->Attrib = APPS_FS_ENTRY_ATTRIB_RD | APPS_FS_ENTRY_ATTRIB_DIR;
        pentry->Size   = 0u;
    }

    pentry->DateTimeCreate.Second = Apps_FS_Time.Second;
    pentry->DateTimeCreate.Minute = Apps_FS_Time.Minute;
    pentry->DateTimeCreate.Hour   = Apps_FS_Time.Hour;
    pentry->DateTimeCreate.Day    = Apps_FS_Time.Day;
    pentry->DateTimeCreate.Month  = Apps_FS_Time.Month;
    pentry->DateTimeCreate.Year   = Apps_FS_Time.Year;



                                                                /* -------------------- UPDATE DIR -------------------- */
    pdir_fs->FileDataIxNext = file_data_ix + 1u;
    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           ENTRY FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        Apps_FS_EntryCreate()
*
* Description : Create a file or directory.
*
* Argument(s) : pname       Name of the entry.
*
*               dir         Indicates whether the new entry shall be a directory :
*
*                               DEF_YES, if the entry shall be a directory.
*                               DEF_NO,  if the entry shall be a file.
*
* Return(s)   : DEF_OK,   if entry created.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Read operations ONLY supported; entry CANNOT be created.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_EntryCreate (CPU_CHAR     *pname,
                                  CPU_BOOLEAN   dir)
{
   (void)&pname;                                                /* Prevent 'variable unused' compiler warning.          */
   (void)&dir;

    return (DEF_FAIL);                                          /* See Note #1.                                         */
}


/*
*********************************************************************************************************
*                                         Apps_FS_EntryDel()
*
* Description : Delete a file or directory.
*
* Argument(s) : pname       Name of the entry.
*
*               file        Indicates whether the entry MAY be a file :
*
*                               DEF_YES, if the entry may     be a file.
*                               DEF_NO,  if the entry may NOT be a file.
*
* Return(s)   : DEF_OK,   if entry created.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Read operations ONLY supported; entry CANNOT be deleted.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_EntryDel (CPU_CHAR     *pname,
                               CPU_BOOLEAN   file)
{
   (void)&pname;                                                /* Prevent 'variable unused' compiler warning.          */
   (void)&file;

    return (DEF_FAIL);                                          /* See Note #1.                                         */
}


/*
*********************************************************************************************************
*                                        Apps_FS_EntryRename()
*
* Description : Rename a file or directory.
*
* Argument(s) : pname_old   Old path of the entry.
*
*               pname_new   New path of the entry.
*
* Return(s)   : DEF_OK,   if entry renamed.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Read operations ONLY supported; entry CANNOT be renamed.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_EntryRename (CPU_CHAR  *pname_old,
                                  CPU_CHAR  *pname_new)
{
   (void)&pname_old;                                            /* Prevent 'variable unused' compiler warning.          */
   (void)&pname_new;

    return (DEF_FAIL);                                          /* See Note #1.                                         */
}


/*
*********************************************************************************************************
*                                       Apps_FS_EntryTimeSet()
*
* Description : Set a file or directory's date/time.
*
* Argument(s) : pname       Name of the entry.
*
*               ptime       Pointer to date/time.
*
* Return(s)   : DEF_OK,   if date/time set.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Read operations ONLY supported; entry date/time CANNOT be set.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_EntryTimeSet (CPU_CHAR           *pname,
                                   APPS_FS_DATE_TIME  *ptime)
{
   (void)&pname;                                                /* Prevent 'variable unused' compiler warning.          */
   (void)&ptime;

    return (DEF_FAIL);                                          /* See Note #1.                                         */
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FILE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         Apps_FS_FileOpen()
*
* Description : Open a file.
*
* Argument(s) : pname       Name of the file.
*
*               pmode_str   Access mode of the file :
*
*                              r or rb             Open existing file,
*                                                                   for reading.
*                              w or wb             Truncate existing file to zero length OR create new file,
*                                                                   for writing.
*                              a or ab             Open existing file OR create new file,
*                                                                   for writing at end-of-file.
*                              r+ or rb+ or r+b    Open existing file,
*                                                                   for reading & writing.
*                              w+ or wb+ or w+b    Truncate existing file to zero length OR create new file,
*                                                                   for reading & writing.
*                              a+ or ab+ or a+b    Open existing file OR create new file,
*                                                                   for reading & writing (writing at end-of-file).
*
* Return(s)   : Pointer to a file, if NO errors.
*               Pointer to NULL,   otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Read operations ONLY supported; file can ONLY be opened in a read mode.
*********************************************************************************************************
*/

void  *Apps_FS_FileOpen (CPU_CHAR  *pname,
                         CPU_CHAR  *pmode_str)
{
    CPU_INT16S          cmp_val;
    APPS_FS_FILE       *pfile;
    APPS_FS_FILE_DATA  *pfile_data;
    CPU_BOOLEAN         found;
    CPU_INT32U          i;
    CPU_SR_ALLOC();


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ------------------ VALIDATE PTRS ------------------- */
    if (pname     == (CPU_CHAR *)0) {                           /* Validate NULL name.                                  */
        return ((void *)0);
    }
    if (pmode_str == (CPU_CHAR *)0) {                           /* Validate NULL mode.                                  */
        return ((void *)0);
    }
#endif



                                                                /* ------------------- VALIDATE MODE ------------------ */
    cmp_val = Str_Cmp(pmode_str, "r");                          /* Validate mode (see Note #1).                         */
    if (cmp_val != 0) {
        cmp_val  = Str_Cmp(pmode_str, "rb");
        if (cmp_val != 0) {
            return ((void *)0);
        }
    }



                                                                /* ------------------- FIND FILE DATA ----------------- */
    found      =  DEF_NO;
    pfile_data = &Apps_FS_FileDataTbl[0];
    for (i = 0u; i < Apps_FS_FileAddedCnt; i++) {
        cmp_val = Str_CmpIgnoreCase(pfile_data->NamePtr, pname);
        if (cmp_val == 0) {
            found = DEF_YES;
            break;
        }

        pfile_data++;
    }

    if (found == DEF_NO) {                                      /* If file data NOT found ...                           */
        return ((void *)0);                                     /* ... rtn NULL ptr.                                    */
    }



                                                                /* ------------------ FIND FREE FILE ------------------ */
    found =  DEF_NO;
    pfile = &Apps_FS_FileTbl[0];

    CPU_CRITICAL_ENTER();
    for (i = 0u; i < APPS_FS_CFG_NBR_FILES; i++) {
        if (pfile->FileDataPtr == (APPS_FS_FILE_DATA *)0) {
            found = DEF_YES;
            break;
        }

        pfile++;
    }

    if (found == DEF_NO) {                                      /* If no free file found ...                            */
        CPU_CRITICAL_EXIT();
        return ((void *)0);                                     /* ... rtn NULL ptr.                                    */
    }



                                                                /* -------------------- OPEN FILE --------------------- */
    pfile->FileDataPtr = pfile_data;
    pfile->Pos         = 0u;
    CPU_CRITICAL_EXIT();

    return ((void *)pfile);
}


/*
*********************************************************************************************************
*                                         Apps_FS_FileClose()
*
* Description : Close a file.
*
* Argument(s) : pfile       Pointer to a file.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Apps_FS_FileClose (void  *pfile)
{
    APPS_FS_FILE  *pfile_fs;
    CPU_SR_ALLOC();


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE FILE PTR ----------------- */
    if (pfile == (void *)0) {                                   /* Validate NULL file ptr.                              */
        return;
    }
#endif

    pfile_fs = (APPS_FS_FILE *)pfile;



                                                                /* -------------------- CLOSE FILE -------------------- */
    CPU_CRITICAL_ENTER();
    pfile_fs->FileDataPtr = (APPS_FS_FILE_DATA *)0;
    pfile_fs->Pos         =  0u;
    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*                                          Apps_FS_FileRd()
*
* Description : Read from a file.
*
* Argument(s) : pfile       Pointer to a file.
*
*               pdest       Pointer to destination buffer.
*
*               size        Number of octets to read.
*
*               psize_rd    Pointer to variable that will receive the number of octets read.
*
* Return(s)   : DEF_OK,   if no error occurred during read (see Note #2).
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Pointers to variables that return values MUST be initialized PRIOR to all other
*                   validation or function handling in case of any error(s).
*
*               (2) If the read request could not be fulfilled because the EOF was reached, the return
*                   value should be 'DEF_OK'.  The application should compare the value in 'psize_rd' to
*                   the value passed to 'size' to detect an EOF reached condition.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_FileRd (void        *pfile,
                             void        *pdest,
                             CPU_SIZE_T   size,
                             CPU_SIZE_T  *psize_rd)
{
    CPU_INT08U         *pdata_cur;
    APPS_FS_FILE_DATA  *pfile_data;
    APPS_FS_FILE       *pfile_fs;
    CPU_SIZE_T          size_rd;


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SIZE PTR ----------------- */
    if (psize_rd == (CPU_SIZE_T *)0) {                          /* Validate NULL size ptr.                              */
        return (DEF_FAIL);
    }
#endif

   *psize_rd = 0u;                                              /* Init to dflt size for err (see Note #1).             */

#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ------------------ VALIDATE PTRS ------------------- */
    if (pfile == (void *)0) {                                   /* Validate NULL file ptr.                              */
        return (DEF_FAIL);
    }
    if (pdest == (void *)0) {                                   /* Validate NULL dest ptr.                              */
        return (DEF_FAIL);
    }
#endif

    pfile_fs   = (APPS_FS_FILE *)pfile;
    pfile_data =  pfile_fs->FileDataPtr;



                                                                /* --------------------- RD FILE ---------------------- */
    if (pfile_fs->Pos >= pfile_data->Size) {                    /* If pos beyond EOF ...                                */
        return (DEF_OK);                                        /* ... NO data rd.                                      */
    }

    size_rd = size;                                             /* Calculate rd size.                                   */
    if (size_rd > pfile_data->Size - pfile_fs->Pos) {
        size_rd = pfile_data->Size - pfile_fs->Pos;
    }

    pdata_cur = (CPU_INT08U *)pfile_data->DataPtr + pfile_fs->Pos;
    Mem_Copy(pdest, (void *)pdata_cur, size_rd);                /* 'Rd' data.                                           */

    pfile_fs->Pos += size_rd;                                   /* Advance file pos.                                    */
   *psize_rd       = size_rd;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                          Apps_FS_FileWr()
*
* Description : Write to a file.
*
* Argument(s) : pfile       Pointer to a file.
*
*               psrc        Pointer to source buffer.
*
*               size        Number of octets to write.
*
*               psize_wr    Pointer to variable that will receive the number of octets written.
*
* Return(s)   : DEF_OK,   if no error occurred during write.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Write operations are NOT permitted.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_FileWr (void        *pfile,
                             void        *psrc,
                             CPU_SIZE_T   size,
                             CPU_SIZE_T  *psize_wr)
{
#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SIZE PTR ----------------- */
    if (psize_wr == (CPU_SIZE_T *)0) {                          /* Validate NULL size ptr.                              */
        return (DEF_FAIL);
    }
#endif

   *psize_wr = 0u;                                              /* Init to dflt size (see Note #1).                     */

   (void)&pfile;                                                /* Prevent 'variable unused' compiler warning.          */
   (void)&psrc;
   (void)&size;

    return (DEF_FAIL);
}


/*
*********************************************************************************************************
*                                        Apps_FS_FilePosSet()
*
* Description : Set file position indicator.
*
* Argument(s) : pfile       Pointer to a file.
*
*               offset      Offset from the file position specified by 'origin'.
*
*               origin      Reference position for offset :
*
*                               APPS_FS_SEEK_ORIGIN_START    Offset is from the beginning of the file.
*                               APPS_FS_SEEK_ORIGIN_CUR      Offset is from current file position.
*                               APPS_FS_SEEK_ORIGIN_END      Offset is from the end       of the file.
*
* Return(s)   : DEF_OK,   if file position set.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) #### File position NOT checked for overflow.
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_FilePosSet (void        *pfile,
                                 CPU_INT32S   offset,
                                 CPU_INT08U   origin)
{
    APPS_FS_FILE_DATA  *pfile_data;
    APPS_FS_FILE       *pfile_fs;
    CPU_BOOLEAN         ok;
    CPU_INT32U          pos_new;


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE FILE PTR ----------------- */
    if (pfile == (void *)0) {                                   /* Validate NULL file ptr.                              */
        return (DEF_FAIL);
    }
#endif

    pfile_fs   = (APPS_FS_FILE *)pfile;
    pfile_data =  pfile_fs->FileDataPtr;



                                                                /* ------------------- SET FILE POS ------------------- */
    ok = DEF_FAIL;
    switch (origin) {
        case APPS_FS_SEEK_ORIGIN_START:
             if (offset >= 0) {
                 pos_new = (CPU_INT32U)offset;
                 ok      =  DEF_OK;
             }
             break;

        case APPS_FS_SEEK_ORIGIN_CUR:
             if (offset < 0) {
                 if (-offset <= pfile_fs->Pos) {
                     pos_new  = pfile_fs->Pos - (CPU_INT32U)-offset;
                     ok       = DEF_OK;
                 }
             } else {
                 pos_new = pfile_fs->Pos + (CPU_INT32U)offset;
                 ok      = DEF_OK;
             }
             break;

        case APPS_FS_SEEK_ORIGIN_END:
             if (offset < 0) {
                 if (-offset <= pfile_data->Size) {
                     pos_new  = pfile_data->Size - (CPU_INT32U)-offset;
                     ok       = DEF_OK;
                 }
             } else {
                 pos_new = pfile_data->Size + (CPU_INT32U)offset;
                 ok      = DEF_OK;
             }
             break;

        default:
             break;
    }
    if (ok != DEF_OK) {
        return (DEF_FAIL);
    }

    if (pos_new > pfile_data->Size) {                           /* Chk for pos beyond EOF.                              */
        return (DEF_FAIL);
    }

    pfile_fs->Pos = pos_new;                                    /* Set pos.                                             */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        Apps_FS_FileSizeGet()
*
* Description : Get file size.
*
* Argument(s) : pfile       Pointer to a file.
*
*               psize       Pointer to variable that will receive the file size.
*
* Return(s)   : DEF_OK,   if file size gotten.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) Pointers to variables that return values MUST be initialized PRIOR to all other
*                   validation or function handling in case of any error(s).
*********************************************************************************************************
*/

CPU_BOOLEAN  Apps_FS_FileSizeGet (void        *pfile,
                                  CPU_INT32U  *psize)
{
    APPS_FS_FILE_DATA  *pfile_data;
    APPS_FS_FILE       *pfile_fs;


#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE SIZE PTR ----------------- */
    if (psize == (CPU_INT32U *)0) {                             /* Validate NULL size ptr.                              */
        return (DEF_FAIL);
    }
#endif

   *psize = 0u;                                                 /* Init to dflt size for err (see Note #1).             */

#if (APPS_FS_CFG_ARG_CHK_EXT_EN == DEF_ENABLED)                 /* ---------------- VALIDATE FILE PTR ----------------- */
    if (pfile == (void *)0) {                                   /* Validate NULL file ptr.                              */
        return (DEF_FAIL);
    }
#endif

                                                                /* ------------------ GET FILE SIZE ------------------- */
    pfile_fs   = (APPS_FS_FILE *)pfile;
    pfile_data =  pfile_fs->FileDataPtr;
   *psize      =  pfile_data->Size;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       Apps_FS_CalcSimilarity()
*
* Description : Calculate 'similarity' between two file names.
*
* Argument(s) : pname_1     Pointer to first  file name.
*
*               pname_2     Pointer to second file name.
*
* Return(s)   : Similarity, in characters.
*
* Caller(s)   : Apps_FS_AddFile().
*
* Note(s)     : (1) The similarity between two file names is the length of the initial path components
*                   (including leading & embedded path separator characters) shared by the file names.
*********************************************************************************************************
*/

static  CPU_SIZE_T  Apps_FS_CalcSimilarity (CPU_CHAR  *pname_1,
                                            CPU_CHAR  *pname_2)
{
    CPU_SIZE_T  similarity;
    CPU_SIZE_T  cmp_len;

                                                                /* ---------------- VALIDATE STR PTRS ----------------- */
    if ((pname_1 == (CPU_CHAR *)0) ||
        (pname_2 == (CPU_CHAR *)0)) {
         return (0u);
    }


    similarity = 0u;
    cmp_len    = 0u;
    while ((*pname_1 == *pname_2) &&                            /* Cmp strs until non-matching chars ...                */
           (*pname_1 !=  ASCII_CHAR_NULL)) {                    /* ... or NULL chars found.                             */
        if (*pname_1 == APPS_FS_PATH_SEP_CHAR) {
            similarity = cmp_len;
        }
        cmp_len++;
        pname_1++;
        pname_2++;
    }

                                                                /* Chk whether one file is ancestor of other.           */
    if (((*pname_1 == ASCII_CHAR_NULL) && (*pname_2 == APPS_FS_PATH_SEP_CHAR)) ||
        ((*pname_2 == ASCII_CHAR_NULL) && (*pname_1 == APPS_FS_PATH_SEP_CHAR))) {
        similarity = cmp_len;
    }

    return (similarity);
}

