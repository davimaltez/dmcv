#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/wait.h>   
#include <sys/types.h>  
#include <fcntl.h>      

typedef struct {
    char nome[64];
}Tarefas;

int main(){
  
    char linha[1000];
    
    while(1){
        
        printf("procesflow>");
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

            while(palavra != NULL){
                printf("%s\n",palavra);
                palavra = strtok(NULL, " \t\n");
            }
        }
    }
    
    return 0;
}