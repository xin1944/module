#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//execute shell command
int shexec(const char *cmd, char res[][512], int count)
{
    printf("shexec, cmd: %s\n", cmd);

    FILE *pp = popen(cmd, "r");
    if(!pp) {
        printf("error, cannot popen cmd: %s\n", cmd);
        return -1;
    }
    int i = 0;
    char tmp[512];
    while(fgets(tmp, sizeof(tmp), pp) != NULL) {
        if(tmp[strlen(tmp)-1] == '\n') {
            tmp[strlen(tmp)-1] = '\0';
        }
        printf("%d.get return results: %s\n", i, tmp);
        strcpy(res[i], tmp);
        i++;
        if(i >= count) {
            printf("get enough results, return\n");
            break;
        }
    }

    int rv = pclose(pp);
    printf("ifexited: %d\n", WIFEXITED(rv));

    if (WIFEXITED(rv))
    {
        printf("subprocess exited, exit code: %d\n", WEXITSTATUS(rv));
    }

    return i;
}

int main()
{
    char cmd[100] = "ls -l";
    char res[10][512];
    shexec(cmd, res, 10);
    return 0;
}
