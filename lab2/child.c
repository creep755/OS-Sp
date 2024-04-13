#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define MAX_LENGTH 256
#define ENV_COUNT 11
#define _GNU_SOURCE

void printStrings(char **strings, size_t rows);
char **allocateStringArray(size_t rows, size_t columns);
void populateChildEnvironment(char **childEnv, char *childEnvPath);

int main(int argc, char *argv[], char *envp[])
{   
    //получение переменных для дочернего процесса 
    pid_t pid = getpid();
    pid_t ppid = getppid();
    char *procName = argv[0];
    char *envPath = argv[1];

    //вывод информации про дочерний процесс 
    printf("Child process name: %s\nPID: %d\nPPID: %d\nEnvironment:\n", procName, pid, ppid);

    char **childEnv = allocateStringArray(ENV_COUNT, MAX_LENGTH); // выделение памяти под массив строк переменных среды 
    populateChildEnvironment(childEnv, envPath); // заполнение массива строк переменными среды по указанному пути envPath
    printStrings(childEnv, ENV_COUNT); // вывод строк переменныз окружения 

    sleep(2);

    return 0;
}

char **allocateStringArray(size_t rows, size_t columns)
{
    char **result = calloc(rows, sizeof(char *));
    if (!result)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < rows; i++)
    {
        result[i] = calloc(columns, sizeof(char));
        if (!result[i])
        {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
    }

    return result;
}

void printStrings(char **strings, size_t rows)
{
    for (size_t i = 0; i < rows; i++)
        printf("%s\n", strings[i]);
}

void populateChildEnvironment(char **childEnv, char *childEnvPath)
{
    FILE *file = fopen(childEnvPath, "rt");
    if (!file)
    {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    char tmp[MAX_LENGTH];
    for (size_t i = 0; fscanf(file, "%s ", tmp) != EOF; i++)
    {
        sprintf(childEnv[i], "%s=%s", tmp, getenv(tmp));
    }

    if (fclose(file) != 0)
    {
        perror("Unable to close file");
        exit(EXIT_FAILURE);
    }
}
