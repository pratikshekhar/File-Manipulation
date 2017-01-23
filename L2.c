
//Header files
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ftw.h>
#include <fcntl.h>
#include <sys/stat.h>


char folderA[200];
char folderB[200];

// Copy function 
int Copy(const char *source, const char *destination){
    
    char buf[1024];
    int n;

    int file1 = open(source, O_RDONLY, 0);
    int file2 = creat(destination, 0777);

    while((n = read(file1, buf, 1024)) > 0){
        
        write(file2, buf, n);
    }

    return 0;
}

// If a file in b does not exist in a, it should be deleted from b.

int Remove(const char *path, const struct stat *status, int type, struct FTW *pfwt){
    
    if(type == FTW_D || type == FTW_DP){
        
        char subpath[600];
        strcpy(subpath, path + strlen(folderB)); // copies contents of one string into another string. 

        char apath[600];
        strcpy(apath, folderA);
        strcat(apath, subpath);

        if(access(apath, F_OK) == -1){
            
            rmdir(path);
            printf("%s was not found at %s. \t Directory deleted.\n", path, apath);
        }

        return 0;
    }

    if(type == FTW_F){
        
        char subpath[600];
        strcpy(subpath, path + strlen(folderB)); // copies contents of one string into another string. 

        char apath[600];
        strcpy(apath, folderA);
        strcat(apath, subpath);

        if(access(apath, F_OK) == -1){
            
            remove(path);
            printf("%s was not found at %s. \t Deleted.\n", path, apath);
        }
    }

    return 0;
}

// If a file in a does not exist in b, it is replicated in b.
// If a file exists in both a and b, the file with the most recent modified date / time should be copied from one directory to the other.

int Replace(const char *path, const struct stat *status, int type){
    
    if(type == FTW_D || type == FTW_DP){
        
        char subpath[600];
        strcpy(subpath, path + strlen(folderA)); // copies contents of one string into another string. 

        char bpath[600];
        strcpy(bpath, folderB);
        strcat(bpath, subpath);

        int exists = access(bpath, F_OK);

        if(exists == -1){
            
            mkdir(bpath, 0777);
            printf("%s was not found at %s. \t Copied to new location.\n", path, bpath);
        }

        return 0;
    }

    if(type == FTW_F){
        
        char subpath[600];
        strcpy(subpath, path + strlen(folderA)); // copies contents of one string into another string. 

        char bpath[600];
        strcpy(bpath, folderB);
        strcat(bpath, subpath);

        int exists = access(bpath, F_OK);

        if(exists == -1){
            
            Copy(path, bpath);
            printf("%s was not found at %s. \t The File has been created.\n", path, bpath);
        }
        
        else{
            
            struct stat bstatus;
            stat(bpath, &bstatus);

            double seconds = difftime(status->st_mtime, bstatus.st_mtime);

            if(seconds > 0){
                
                remove(bpath);
                Copy(path, bpath);
                printf("%s has a more recent modification date than %s. \t File has been updated.\n", path, bpath);
            }
            
            else if(seconds < 0){
                
                remove(path);
                Copy(bpath, path);
                printf("%s has a more recent modification date than %s. \t File has been updated.\n", bpath, path);
            }
        }
    }

    return 0;
}


//Main function

int main(int argc, char *argv[]){

    strcpy(folderA, argv[1]); // copies the string pointed to, by argv[1] to folderA.
    strcpy(folderB, argv[2]); // copies the string pointed to, by argv[2] to folderB.

    printf("\n");

    int flags = FTW_DEPTH;
    
    /* nftw() walks through the directory tree that is located under the
       directory dirpath, and calls fn() once for each entry in the tree. */
    
    nftw(folderB, Remove, 20, flags);
    
    /* ftw() is an older function that offers a subset of the functionality
       of nftw().  ftw() has no flags argument.  It behaves the same as when nftw()
          is called with flags specified as zero. */
    
    ftw(folderA, Replace, 20);

    printf("\n");

    return 0;
}

