//  main.c
//  Project
//
//  Created by Arun Prakash Themothy Prabu Vincent on 11/21/16.
//  Copyright © 2016 Arun Prakash Themothy Prabu Vincent. All rights reserved.
//
//  Teammates : Arun Prakash Themothy Prabu Vincet (axt161330) and Jaypreethi Palanisamy(jxp164030)
//  The project is implemented in XCODE (mac os)
//
//



#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* system Paramters and function */
void file_system_initialization (int N1, int N2);
void free_block_initialization ();
void create_directory() ;
void create_file();
void copy_file();
void free_memory_allocation();
void remove_file();
unsigned long free_block[100];
unsigned long inode [400];
char inode_info[32];
int freeBlockIndex = 99;
int inodeIndex = 1;
unsigned long FileSystemSize = 0;
int fd;
char p[100];

/* Beginning of Execution of Main Function */

int main(int argc, const char * argv[]) {
    
    char command[100] ,str[100];
    int n1,n2;
    while(1)  /* Begining of loop */
    {
        printf("\n Enter Command : ");
        gets(command); /* User Command */
        if(!strcasecmp(command,"q")) /* User Terminating program using q */
        {
            printf("\n Terminating !!!" ); /* Total Commands executed */
            exit(0);
        }
        strlcpy(str,command,7); /*Extracting initfs from user command */
        
        if(!strcasecmp (command,"mkdir v6-dir"))
           create_directory();  /* Function call to create directory */
        
        else if (!strcasecmp(str,"initfs")){
            sscanf(command, " %s %d %d" ,&str ,&n1 ,&n2); /* extracting system parameters from command */
            file_system_initialization(n1,n2); /* Function call to Initialize File System */
        }
        
        else if (!strcasecmp(command,"cpin externalfile v6-file"))
           create_file(); /* Function call to create v6-File and copy contents of external file to it */
        
        else if (!strcasecmp(command,"cpout v6-file externalfile"))
           copy_file(); /* Function call to create external file and set it contents to that of v6-file*/
        
        else if (!strcasecmp(command,"rm v6-file"))
           remove_file(); /* remove file and deallocate memory and Inodes */
        else
           printf("\n Invalid Command ") ;
    }
    return 1;
 
        
}

/* Function to create a v6 directory */

void create_directory() {
    
    /*creating new directory */
    
    char pathname[100];
    const char newpathname[]="/Users/ArunPrakash/v6filesystem"; /*pathname of new directory */
    
    getwd(p); /* current working directory before creating a new filesystem and directory */
    
    if(mkdir(newpathname,0777)==-1){ /* System call to create directory v6filesystem */
        printf("\n Error in mkdir ");
    }
    
    if(chdir(newpathname)==-1){  /* System call to change the current working directory to v6filesystem */
        printf("\n Error in mkdir ");
    }
    
    if (getwd(pathname) == NULL ){ /* System call to know current working directory */
        printf("Error getting path\n");
        exit(1);
    }
    
    /* Creating a external file for cpin operation */
    
    int fd_ext; /* file descriptor for external file */
    
    /*contents of the external file */
    char buf[] = " File Name : file2.txt Tpye : External text file Contents : This file is the external";
    const char externalpathname[] = "/Users/ArunPrakash/v6filesystem/externalfile.txt" ;/* Path of external file */
    
    size_t ret ;
    
    fd_ext = open(externalpathname,O_CREAT | O_RDWR,0777) ;/* system call to create external new external file or open existing file in Read and write mode */
    
    ret = write(fd_ext,buf,strlen(buf)); /* Writing to external file */
    
    if (ret==-1) {
        printf("\n Error in Seek 4 Function ");
    }
    
    close(fd_ext); /*closing file descriptor */

}

/* Function to initialize freeblock and inodes */


void file_system_initialization (int N1 ,int N2) {
    
    /* Intializing FreeBlocks of file System */
    
    FileSystemSize = N1 * 512 ; /* calculating File System Size */
    free_block[0] = 99*512 + 1024 ; /* Initializing free[0] to the pool of next free block address */
    int block_addr = 1024 ; /* Starts with 1024(block 3) since block 1(first 512B) and block2(second 512B) are used for system boot and superblck */
    
    for(int i=99 ; i>0 ; --i ){
        free_block[i] = block_addr ; /* Allocating the address of free memory to Free Blocks */
        block_addr +=512 ; /* address of next free memory*/
    }
    
    /* Initializing Inodes of file System */
    
    unsigned long addr = free_block[99] ; /* Allocating free blocks from block 3 to inodes */
    int numberOfBlocks = N2/(16); /* calculating total blocks required for inodes */
    
    /* Allocating blocks to inodes*/
    int j=1;
    for (int i =1 ; i<= numberOfBlocks; ++i){
        for (; j<=(i*16) ;) /* Single block could store 16 inodes */
        {
            inode[j] = addr; /* Allocating address location of each inode */
            addr +=32 ;
            ++j;
        }
    --freeBlockIndex ; /*Decrementing the total free block available */
    }
   
}

/* Function to update the Free Block on exhaustion */

void free_block_initialization () {
    
    /* Checking if the File System Memory has exhausted */
    
    if(free_block[0] == FileSystemSize) {
        printf(" \n \n Memory FUll ") ;
    }
    
    long file_index = free_block[0]; /* extracting the starting address location of pool of memory */
    long flag = file_index + 99*512 ; /* Initializing free[0] to the pool of next free block address */
    
    
    if(flag < FileSystemSize) {/* The available memory blocks it greater than that required by Free Block */
        free_block[0] = flag ;
        for (int i=99 ; i < 0 ; --i) {
            free_block [i] = file_index ; /* Allocating the address of free memory to block */
            file_index += 512 ;
            freeBlockIndex++; /*Incrementing the available free block */
        }
        free_block[0] = file_index ;
    }
    
    
    else { /* The available memory blocks is less than that required by Free Block */
        
        long j = (FileSystemSize - file_index )/512 ;
        free_block[0] = FileSystemSize; /* Calculating the total blocks of memory available */
        for( ; j>0; --j) {
            free_block[j] = file_index;/* Allocating the address of free memory to block */
            file_index += 512 ;
            freeBlockIndex++;/*Incrementing the available free block */
        }
        
    }
   
}


/* Function to create a v6-File */

void create_file() {
    
    size_t ret;
    int fd_ext;
    char buf[512] ;
    const char externalpathname[] = "/Users/ArunPrakash/v6filesystem/externalfile.txt" ;/* pathname of external file */
    
    fd_ext = open(externalpathname,O_RDONLY) ; /*system call to open external file in Read Only mode */
    
    ret = read(fd_ext,buf,sizeof(buf)); /* Reading from external file */
    if (ret==-1) {
        printf("\n Error in read Function ");
    }

    int noBlocksUsed = sizeof(buf)/512 ; /* calculation for block requirement */
    
 
    if ((freeBlockIndex - noBlocksUsed) <= 0){ /* checking it the reqired free block available */
    
        free_block_initialization () ; /* Block allocation to Free block from memory */
    }
   
 
    const char pathname[]="/Users/ArunPrakash/v6filesystem/v6-file.txt"; /* pathname of v6file*/
    
    /* Consructing Inode information */
    sprintf(inode_info,"%lu %lu %s",free_block[freeBlockIndex],strlen(buf),"v6-file.txt");
    
    /* Writing Inode to file System */
    
    if( (fd=open(pathname,O_CREAT | O_RDWR ,0777) ) < -1 ){ /* System call to open file system */
        printf("\n\n Cannot Open FIle");
        exit(1);
    }
    
    ret = lseek(fd ,inode[inodeIndex], SEEK_SET) ;
    if (ret==-1) {
        printf("\n Error in Seek 1 Function ");
    }
    
    inodeIndex ++ ;
    
    ret = write(fd, inode_info, strlen(inode_info)); /* writing inode in file system */

    if (ret==-1) {
        printf("\n \n %s " , inode_info);
        printf("\n Error in Seek 2 Function ");
    }
    
    /* Moving file pointer to the location of free block for Data */
    
    ret = lseek(fd, free_block[freeBlockIndex], SEEK_SET) ;
    if (ret==-1) {
        printf("\n Error in Seek 3 Function ");
    }
    
    /* Writing contents of external file to the V6-file */
    
    ret = write(fd,buf,strlen(buf));
    if (ret==-1) {
        printf("\n \n %s ", buf);
        printf("\n Error in Seek 4 Function ");
    }
    
    freeBlockIndex = freeBlockIndex - noBlocksUsed ;/* Decrementing the free blocks */
    close(fd);
    close(fd_ext);
}

/* Function to copy contents of v6-file to external file */
 
void copy_file () {
    
    
    char buffer[100] ;
    unsigned long index = inode[1];
    unsigned long addr = 0;
    unsigned long fileSize=0;
    char file[10] ;
    int flag;
    size_t ret;
    int fd_ext;
    
    
    const char extpathname[]="/Users/ArunPrakash/v6filesystem/externalfile.txt";
    const char pathname[]="/Users/ArunPrakash/v6filesystem/v6-file.txt";
    
    if( (fd=open(pathname,O_RDONLY) ) < -1 ){ /* Open v6 file for copying contents */
        printf("\n\n Cannot Open FIle");
        exit(1);
    }
    
    ret = lseek(fd, index , SEEK_SET); /* Moving the file pointer to the location of inode */
    if (ret==-1) {
        printf("\n Error in Seek Function ");
    }
    
    int n;
    
    while (read(fd, buffer, sizeof(buffer))) { /* Reading inodes */
        flag = sscanf(buffer, "%lu %lu %s" , &addr, &fileSize ,file) ;/* Extracting the inode information*/
        if(strcmp("file1.txt" , file)== 0){
            goto x ;
        }
    
    }
        x :
   
    ret = lseek(fd, addr, SEEK_SET); /* Moving file pointer to the location of File */
    if (ret==-1) {
        printf("\n Error in Seek Function ");
    }
    
    ret = read(fd ,buffer,fileSize); /* Reading the contents of v6 file */
    if (ret==-1) {
        printf("\n Error in read Function ");
    }
    
    /* Creating External file using system call */

    if( (fd_ext = open(extpathname,O_CREAT | O_RDWR ,0777) ) < -1 ){         printf("\n\n Cannot Open FIle");
        exit(1);
    }
    
    /* Copying the contents of V6 File to external file */
    ret = write(fd_ext,buffer,strlen(buffer));
    if (ret==-1) {
        printf("\n Error in Seek 4 Function ");
    }
    
    close(fd_ext);/* closing file descriptor of external file */
    close(fd); /*closing file descriptor of v6 file */
}


void remove_file() {
    
    const char filepathname[]="/Users/ArunPrakash/v6filesystem/v6-file.txt";
    const char extfilepathname[]="/Users/ArunPrakash/v6filesystem/externalfile.txt";
    const char path[]="/Users/ArunPrakash/v6filesystem";
    int flag;
    
    if(chdir(p)==-1){  /* System call to change the current working directory to v6filesystem */
        printf("\n Error in mkdir ");
    }
    flag = remove(filepathname);
    flag = remove(extfilepathname);
    
    if( !flag){
        printf("\n \n File Deleted Successfully...");
    }
    flag = remove (path);
    
    
}
