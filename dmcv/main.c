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

int buscar_tarefa(Tarefa * tarefas, int total_tarefas, char * nome_tarefa){

    for(int i = 0; i < total_tarefas; i++){
        if(strcmp(tarefas[i].nome,nome_tarefa) == 0){
            return i;
        }
    }
    return -1;
}


void run_sequencial(Tarefa * tarefas, int total_tarefas, char * nome_tarefa){

        pid_t pid = fork();
        if (pid < 0) { 
            fprintf(stderr, "Falha na criação do processo");
            return;
        }
        
        else if (pid == 0) { 

            
            int indice_tarefa = buscar_tarefa(tarefas,total_tarefas,nome_tarefa);

            if(indice_tarefa != -1){
                //execlp(tarefas[indice_tarefa].argumentos[indice_tarefa],tarefas[indice_tarefa].argumentos[1],NULL);
                //execvp = mais dinâmico
                execvp(tarefas[indice_tarefa].argumentos[0], tarefas[indice_tarefa].argumentos);
                exit(1);
            }

            else{
                printf("Tarefa não encontrada\n");
                exit(1);
            }
        }
        else { 
            int status;
            pid_t esperado = waitpid(pid, &status, 0);

            if(esperado > 0){
                    printf("Processo Filho concluido\n");
            }
        }
}

pid_t run_tasks_parallel(Tarefa * tarefas, int total_tarefas, char * nome_tarefa){

        pid_t pid = fork();
        if (pid < 0) { 
            fprintf(stderr, "Falha na criação do processo");
            return -1;
        }
        
        else if (pid == 0) { 

            
            int indice_tarefa = buscar_tarefa(tarefas,total_tarefas,nome_tarefa);

            if(indice_tarefa != -1){
                //execlp(tarefas[indice_tarefa].argumentos[indice_tarefa],tarefas[indice_tarefa].argumentos[1],NULL);
                //execvp = mais dinâmico
                execvp(tarefas[indice_tarefa].argumentos[0], tarefas[indice_tarefa].argumentos);
                exit(1);
            }

            else{
                printf("Tarefa não encontrada\n");
                exit(1);
            }
        }

        return pid;
}

void esperar_processos(pid_t * pids, int qtd_pids){

    for(int j = 0; j < qtd_pids; j++){
        int status;
        pid_t esperado = waitpid(pids[j], &status, 0);

        if(esperado > 0){
            printf("Processo Filho concluido\n");
        }
    }
}

void run_pipe(int num_pipes, int pipes[][num_pipes], Tarefa * tarefas, int tarefas_no_pipe, int * indices_tarefas_pipe){

    pid_t pids[32];

    for(int i = 0; i < tarefas_no_pipe; i++){

        pid_t p = fork();
        pids[i] = p;

        if(p < 0){
            printf("Falha ao criar processo!");
            return;
        }
        if(p == 0){
            //primeiro elemento
            if(i == 0){ 

                dup2(pipes[i][1],STDOUT_FILENO);

            }
            else if(i == tarefas_no_pipe - 1){

                dup2(pipes[i- 1][0],STDIN_FILENO);        

            }
            else{

                dup2(pipes[i - 1][0],STDIN_FILENO);
                dup2(pipes[i][1],STDOUT_FILENO);

            }

            for(int j = 0; j < num_pipes; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            int indice_tarefa = indices_tarefas_pipe[i];
            execvp(tarefas[indice_tarefa].argumentos[0], tarefas[indice_tarefa].argumentos);
            perror("Erro no execvp");
            exit(1);
        }
    }


    for(int i = 0; i < num_pipes; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for(int i = 0; i < tarefas_no_pipe; i++){
        waitpid(pids[i],NULL,0);
    }
}



int main(int argc, char *argv[]){


    switch(argc){
        case 1:
            printf("Modo Interativo\n");

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
                    //Cadastramento
                    if(strcmp(palavra,"task") == 0){
                        
                        char *nome = strtok(NULL, " \t\n");
                        
                        if(nome == NULL){
                            printf("Nome da tarefa faltando\n");
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

                    //Rodando
                    else if(strcmp(palavra,"run") == 0){
            
                            palavra = strtok(NULL, " \t\n");

                            //Sequencial

                            if(strcmp(palavra,"sequential") == 0){
                                palavra = strtok(NULL, " \t\n");

                                if(palavra == NULL){
                                    printf("Run oq? digite os argumentos");
                                    continue;
                                }
                                while(palavra != NULL){
                                    run_sequencial(tarefas,total_tarefas, palavra);
                                    palavra = strtok(NULL, " \t\n");
                                }
                            }

                            //Paralelo
                            else if(strcmp(palavra,"parallel") == 0){
                                palavra = strtok(NULL, " \t\n");
                                if(palavra == NULL){
                                    printf("Run oq? digite os argumentos");
                                    continue;
                                }

                                pid_t pids[32];
                                int i = 0;

                                while(palavra!=NULL){
                                    pids[i] = run_tasks_parallel(tarefas,total_tarefas, palavra);
                                    i++;
                                    palavra = strtok(NULL, " \t\n");
                                }


                                esperar_processos(pids,i);
                            }

                            //Pipe
                            
                            else if(strcmp(palavra,"pipe") == 0){

                                palavra = strtok(NULL, " \t\n");
                                if(palavra == NULL){
                                    printf("Run oq? digite os argumentos");
                                    continue;
                                }

                                
                                int indices_tarefas_pipe[100];
                                int numero_tarefas_pipe = 0;
                                
                                while(palavra != NULL){
                                    indices_tarefas_pipe[numero_tarefas_pipe] = buscar_tarefa(tarefas, total_tarefas, palavra);
                                    palavra = strtok(NULL, " \t\n");
                                    numero_tarefas_pipe++;
                                }
                                
                                int num_pipes = numero_tarefas_pipe - 1;
                                int pipes[num_pipes][2]; // Cada linha vai ser um pipe

                                for(int i = 0; i < num_pipes; i++){
                                    pipe(pipes[i]);
                                }
                                
                                run_pipe(num_pipes, pipes, tarefas, numero_tarefas_pipe,indices_tarefas_pipe);

                            }

                            else{

                                pid_t pid = fork();
                                if (pid < 0) { 
                                    fprintf(stderr, "Falha na criação do processo");
                                    return 1;
                                }
                                else if (pid == 0) { 

                                    int indice_tarefa = buscar_tarefa(tarefas,total_tarefas,palavra);

                                    if(indice_tarefa != -1){
                                        execlp(tarefas[indice_tarefa].argumentos[indice_tarefa],tarefas[total_tarefas].argumentos[1],NULL);
                                        exit(1);
                                    }

                                    else{
                                        printf("Tarefa não encontrada\n");
                                        return 1;
                                    }
                                }
                                else { 
                                    int status;
                                    pid_t esperado = waitpid(pid, &status, 0);

                                    if(esperado > 0){
                                        printf("Processo Filho concluido\n");
                                    }
                                }
                            }
                        }
                        
                    }

                
            }

            break;

        case 2:
            printf("Modo workflow, arquivo: %s\n", argv[1]);
            break;
        default:
            fprintf(stderr, "Erro: número incorreto de argumentos\n");
            exit(1);
    }
    
    return 0;

}
