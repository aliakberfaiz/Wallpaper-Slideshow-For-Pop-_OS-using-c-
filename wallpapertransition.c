#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>
#include<limits.h>
#include <sys/types.h>
#include <sys/wait.h>
const char *getExt (const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        e = ""; // fast method, could also use &(fspec[strlen(fspec)]).
    return e;
}
// Maximum length of string after modifications.
const int MAX = 1000;
// Replaces spaces with %20 in-place and returns
// new length of modified string. It returns -1
// if modified string cannot be stored in str[]
int replaceSpaces(char str[])
{
    // count spaces and find current length
    int space_count = 0, i;
    for (i = 0; str[i]; i++)
        if (str[i] == ' ')
            space_count++;
 
    // Remove trailing spaces
    while (str[i-1] == ' ')
    {
       space_count--;
       i--;
    }
 
    // Find new length.
    int new_length = i + space_count * 2 + 1;
 
    // New length must be smaller than length
    // of string provided.
    if (new_length > MAX)
        return -1;
 
    // Start filling character from end
    int index = new_length - 1;
 
    // Fill string termination.
    str[index--] = '\0';
 
    // Fill rest of the string from end
    for (int j=i-1; j>=0; j--)
    {
        // inserts %20 in place of space
        if (str[j] == ' ')
        {
            str[index] = '0';
            str[index - 1] = '2';
            str[index - 2] = '%';
            index = index - 3;
        }
        else
        {
            str[index] = str[j];
            index--;
        }
    }
 
    return new_length;
}

int main(int argc, char const *argv[])
{
    char cwd[PATH_MAX];
    char fileUri[MAX];
    struct dirent *de;
    pid_t pid,child_id;
    int ret = 1;
    int status;
    int minutes = 2;
    if(getcwd(cwd,sizeof(cwd))!=NULL){
        while(1){
            fprintf(stdout,"Current working directory : %s \n",cwd);
            DIR *dr = opendir(cwd);
            if(dr==NULL){
                printf("Couldn't open the directory");
            }
            while((de=readdir(dr))!=NULL){
                if((strcmp(".png",getExt(de->d_name))==0) || (strcmp(".jpg",getExt(de->d_name))==0)|| (strcmp(".jpeg",getExt(de->d_name))==0)){
                sprintf(fileUri,"file://%s/%s",cwd,de->d_name);
                if(replaceSpaces(fileUri)!=-1){
                    fprintf(stdout,"\n%s\n",fileUri);
                    pid =fork();
                    if(pid ==-1){
                        fprintf(stderr,"can't fork, some error occured\n");
                        exit(-1);
                    }
                    else if(pid==0){
                    //child process
                    //gsettings set org.gnome.desktop.background picture-uri 'file:///home/aliakberfaiz/Pictures/Wallpapers/Screenshot%20from%202020-12-15%2015-24-32.png'
                    char *arr[]={"gsettings","set" ,"org.gnome.desktop.background","picture-uri"};
                    fprintf(stdout,"Setting new wallpaper\n");
                    execlp(arr[0], arr[0],arr[1],arr[2],arr[3],fileUri,NULL);
                    exit(0);// never comes here 3:)
                    }
                    else{
                        //pareNt process 
                        if(waitpid(pid,&status,0)>0){
                           // fprintf(stdout,"\nChild Process with id: %u  terminated with status %d\n",pid,status);
                           // fprintf(stdout,"Done!\n");
                            fprintf(stdout,"New wallpaper set!\n");
                            sleep(minutes*60);
                        }
                        else{
                            printf("wait() failed\n");
                        }

                    }


                }
            }
        }
    }
}
    else{
        perror("getcwd() error");
        return 1;
    }
    return 0;
}
