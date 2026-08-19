#define _POSIX_C_SOURCE 200809L


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/wait.h>   
#include <sys/types.h>  
#include <fcntl.h>      

typedef struct {
    char nome[64];
    char *argumentos[32];
    int quantidade;
}Tarefa;

int main(){
    
    Tarefa tarefas[100];
    int total_tarefas = 0;

    char linha[1000];
    
    while(1){
        
        printf("procesflow> ");
        fflush(stdout);
        
        
        if(fgets(linha,sizeof(linha),stdin) == NULL){
            printf("\n");
            break;
        }
        
        //pega a primeira palavra
        char *palavra = strtok(linha, " \t\n");
        
        if (palavra != NULL) {
            
            if(strcmp(palavra,"exit") == 0){
                printf("Programa encerrado\n");
                break;
            }
            
            if(strcmp(palavra,"task") == 0){
                
                char *nome = strtok(NULL, " \t\n");
                
                if(nome == NULL){
                    printf("Nome da tarefa faltando");
                }
                else{
                    
                    strcpy(tarefas[total_tarefas].nome,nome);
                    
                    int i = 0;
                    
                    //Cada nova chamada com NULL o strtok vai para onde ele parou na última vez
                    char *argumento = strtok(NULL, " \t\n");
                    
                    while(argumento != NULL){
                        tarefas[total_tarefas].argumentos[i] = strdup(argumento);
                        i++;
                        argumento = strtok(NULL, " \t\n");
                    }
                    
                    //Como o execvp exige que o último elemento seja NULL
                    
                    tarefas[total_tarefas].argumentos[i] = NULL;
                    tarefas[total_tarefas].quantidade = i;
                    total_tarefas++;
                    
                    printf("Tarefa Cadastrada\n");
                }

            }

        }
    }

    printf("Tarefas cadastradas: \n");

    for(int i = 0; i < total_tarefas; i++){
        printf("%s\n",tarefas[i].nome);
        
        for(int j = 0; tarefas[i].argumentos[j] != NULL; j++){
            printf("%s\n",tarefas[i].argumentos[j]);
        }
    }

    printf("Escolha a forma de Rodar: \n");

    printf("procesflow> ");
        
    if(fgets(linha,sizeof(linha),stdin) == NULL){
        printf("\n");
    }
        
    //pega a primeira palavra
    char *palavra = strtok(linha, " \t\n");
    
    if(strcmp(palavra,"run") == 0){
        palavra = strtok(NULL, " \t\n");

        
        pid_t pid = fork();
        if (pid < 0) { 
            fprintf(stderr, "Fork Failed");
            return 1;
        }
        else if (pid == 0) { 
            execlp(tarefas[0].argumentos[0],tarefas[total_tarefas].argumentos[1],NULL);
        }
        else { 
    
            wait(NULL);
            printf("Child Complete");
        }

    }


    return 0;
}