#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LENGTH 256
#define CHILD_NAME_LENGTH 9
#define ENV_COUNT 11
#define _GNU_SOURCE

extern char **environ;

void sortStrings(char ***strings, size_t rows);
void printStrings(char **strings, size_t rows);
char *findEnv(char **env, char *key, size_t rows);

int main(int argc, char *argv[], char *envp[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Error! File is not provided\n");
        exit(EXIT_FAILURE);
    }
    //подсчет и сортировка переменных окружения 
    size_t sizeOfEnv = 0;
    while (environ[sizeOfEnv])
        sizeOfEnv++;

    sortStrings(&environ, sizeOfEnv);
    printStrings(environ, sizeOfEnv);

    char *childEnvPath = argv[1];
    size_t childIndex = 0;

    while (1)
    {
        setenv("LC_COLLATE", "C", 1);
        printf("Input char: + * & q.\n>");
        rewind(stdin);
        //выбор спосооба получения адреса дочернего процесса 
        char opt;
        while (1)
        {
            scanf("%c", &opt);
            if (opt == '+' || opt == '*' || opt == '&' || opt == 'q')
            {
                break;
            }
        }

        //создание переменной, хранящей имя дочернего процесса
        char childName[CHILD_NAME_LENGTH];
        snprintf(childName, CHILD_NAME_LENGTH, "%s%02d", "child_", (int)childIndex);
        //массив для передачи агрументов при создании дочернего процесса 
        char *childArgs[] = {childName, childEnvPath, NULL};
        int statusChild = 0;

        pid_t pid; // идентификатор процесса 
        switch (opt)
        {
            case '+': // получение пути к дочернему процессу через функцию getenv()
                pid = fork();
                if (pid == -1)
                {
                    printf("Error! Error occurred, error code - %d\n", errno);
                    exit(errno);
                }
                if (pid == 0)
                {
                    char *CHILD_PATH = getenv("CHILD_PATH"); // путь к исполняемому файлу, который запущен в дочернем процессе 
                    printf("%s\n", CHILD_PATH);
                    printf("Child process created. Please, wait...\n");
                    execve(CHILD_PATH, childArgs, environ); // замена текущего процесса на новый с аргументами и переменными окружения 
                }
                wait(&statusChild); // ожидание завершения дочернего процесса, сохранение статуса завершения 
                break;

            case '*':   // получение пути к дочернему процессу путем сканирования массива параметров среды, переданного в main
                pid = fork();
                if (pid == -1)
                {
                    printf("Error! Error occurred, error code - %d\n", errno);
                    exit(errno);
                }
                if (pid == 0)
                {
                    char *CHILD_PATH = findEnv(envp, "CHILD_PATH", sizeOfEnv); // поиск значения переменной окружения с именем ChildPath в массиве envp
                    printf("Child process created. Please, wait...\n");
                    execve(CHILD_PATH, childArgs, environ); // замена текущего процесса на новый с аргументами и переменными окружения
                }
                wait(&statusChild); // ожидание завершения дочернего процесса, сохренение статуса завершения
                break;

            case '&': // получение пути к дочернему процессу путем сканирования массива параметров среды, указанной во вешней переменной, установленной при запуске
                pid = fork();
                if (pid == -1)
                {
                    printf("Error! Error occurred, error code - %d\n", errno);
                    exit(errno);
                }
                if (pid == 0)
                {
                    char *CHILD_PATH = findEnv(environ, "CHILD_PATH", sizeOfEnv); // поиск значения переменной окружения с именем ChildPath в массиве environ
                    printf("Child process created. Please, wait...\n");
                    execve(CHILD_PATH, childArgs, environ); // замена текущего процесса на новый с аргументами и переменными окружения
                }
                wait(&statusChild); // ожидание завершения дочернего процесса, сохренение статуса завершения
                break;

            case 'q':
                exit(EXIT_SUCCESS);

            default:
                break;
        }

        childIndex++;
        if (childIndex > 99)
            childIndex = 0;
    }
}

void printStrings(char **strings, size_t rows)
{
    for (size_t i = 0; i < rows; i++)
        fprintf(stdout, "%s\n", strings[i]);
}

void swap(char **s1, char **s2)
{
    char *tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}

void sortStrings(char ***strings, size_t rows)
{
    for (size_t i = 0; i < rows - 1; i++)
    {
        for (size_t j = 0; j < rows - i - 1; j++)
        {
            if (strcoll((*strings)[j], (*strings)[j + 1]) > 0)
                swap(&((*strings)[j]), &((*strings)[j + 1]));
        }
    }
}

char *findEnv(char **env, char *key, size_t rows)
{
    char *result = calloc(MAX_LENGTH, sizeof(char));
    for (size_t i = 0; i < rows; i++)
    {
        if (strstr(env[i], key))
            strncpy(result, env[i] + strlen(key) + 1, MAX_LENGTH);
    }
    return result;
}
