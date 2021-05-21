#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include "show.h"

#define MAX_CHARACTER 20
#define NUMBER_SOLUTION 11

boolean checkDgm(Diagram*, Diagram*, Rule*, int*);
Rule ruleFromFile(FILE*);


int main(int argc, char *argv[]) {
    int* tab1 = calloc(10, sizeof(int));
    int tab2[10];
    
    FILE *fileArr[NUMBER_SOLUTION];
    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        char buf[30];
        snprintf(buf, 30, "./data/718/%d.txt", i);
        fileArr[i] = fopen(buf, "r");
        if (fileArr[i] == NULL) {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            exit(0);
        }
    }

    Rule ruleArr[NUMBER_SOLUTION];
    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        Rule rule = ruleFromFile(fileArr[i]);
        ruleArr[i] = rule;
        fclose(fileArr[i]);
    }

    int maxSize = 100;
    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        for (int j = 0; j < NUMBER_SOLUTION; j++) {
            if (i != j) {
                Rule localMapping;
                localMapping.pTransitionArr = calloc(ruleArr[i].size, sizeof(Transition));
                localMapping.size = 0;

                boolean isDet = true;
                for (int size = 2; size < maxSize; size++) {
                    boolean check = checkDgm(&dgmSrc, &dgmDst, &localMapping, &maxSize);
                    if (check == false) {
                        isDet = false;
                        break;
                    }
                }

                if (isDet == true)
                    printf("%d -> %d (%d)\n", i, j, maxSize);

                localMapping.size = 0;
                free(localMapping.pTransitionArr);
            }
        }
    }

    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        ruleArr[i].size = 0;
        free(ruleArr[i].pTransitionArr);
    }

    return 0;
}

Rule ruleFromFile(FILE* file) {
    Rule rule;
    rule.pTransitionArr = calloc(150, sizeof(Transition));
    rule.size = 0;

    char buffer[MAX_CHARACTER];
    fgets(buffer, MAX_CHARACTER, file);//premier ligne pour l'information de la solution

    char* token;
    int stateArr[4];
    int index;
    while (fgets(buffer, MAX_CHARACTER, file) != NULL) {
        index = 0;
        token = strtok(buffer, " ");
        while(token != NULL) {
            stateArr[index] = atoi(token);
            index++;
            token = strtok(NULL, " ");
        }
        
        rule.pTransitionArr[size].pStateArr[0] = stateArr[0];
        rule.pTransitionArr[size].pStateArr[1] = stateArr[1];
        rule.pTransitionArr[size].pStateArr[2] = stateArr[2];
        rule.pTransitionArr[size].result = stateArr[3];
    }

    return rule;
}


boolean checkDgm(Rule* ruleSrc, Rule* ruleDst, Rule* localMapping, int currentSize, int* maxSize) {
    GConfig gc0 = getInitGConfig(size);
    Diagram dgmSrc = getDiagram(ruleSrc, &gc0);
    Diagram dgmDst = getDiagram(ruleDst, &gc0);

    boolean hasNewLConfig = false;
    for (int t = 0; t <= dgmSrc->timeFin - 1; t++) {
        GConfig gcSrc = dgmSrc->pGConfigArr[t];
        GConfig gcDst = dgmDst->pGConfigArr[t + 1];
        for (int p = 0; p < gcSrc.size; p++) {
            LConfig lcSrc = getLConfig(&gcSrc, p);
            int resDst = gcDst.pStateArr[p];
            int oldRes = getResult(localMapping, &lcSrc);
            if ( oldRes == -1) {
                Transition transition = newTransition(&lcSrc, resDst);
                addTransition(localMapping, &transition);
                hasNewLConfig = true;
            }
            else if (oldRes != resDst) {
                freeDiagram(&dgmSrc);
                freeDiagram(&dgmDst);
                return false;
            }
        }
    }

    if (hasNewLConfig == true) {
        int newMaxSize = currentSize * 2; 
        if (newMaxSize > *maxSize)
            *maxSize = newMaxSize;
    }

    return true;
}