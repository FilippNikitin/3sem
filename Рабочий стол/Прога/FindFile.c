#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

const int MaxLengthOfPath = 255;

void FindFile(const char* NameDirrect, const char* NameFileToFind, const int DepthOfSearch);

int main(int argc, char* argv[])
{
    if(argc == 3)
    {
        FindFile(".",argv[1], atoi(argv[2]));
    }
    else
    {
        printf("Invalid number of arguments\n");
        exit(0);
    }
    return 0;
}

void FindFile(const char* NameDirrect, const char* NameToFind, const int DepthOfSearch)
{

    DIR* currentDir = opendir(NameDirrect);
    struct dirent *currentFile = readdir(currentDir);
    struct stat StatCurrentFile;
    char path[MaxLengthOfPath];

    while(currentFile)
    {
        stat(currentFile -> d_name, &StatCurrentFile);
        if(strcmp(NameToFind, currentFile -> d_name) == 0)
        {
               printf("File is located by this path: %s\n", NameDirrect);
        }

        if(S_ISDIR(StatCurrentFile.st_mode) &&
            DepthOfSearch &&
            (!!strcmp(currentFile -> d_name, ".")
            && !!strcmp(currentFile -> d_name, "..")))
        {
            strcpy(path, NameDirrect);
            strcat(path, "/");
            strcat(path, currentFile -> d_name);
            FindFile(path, NameToFind, DepthOfSearch - 1);
        }
        currentFile = readdir(currentDir);
    }
    closedir(currentDir);
}
