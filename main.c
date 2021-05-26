#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>


#define BUFF_SIZE 20
#define NUMBER_SOLUTION 719
#define NB_MAX_TRANSITION 150

#define true 1
#define false 0

typedef int boolean;
typedef char State;


typedef struct {
    State pStateArr[3];
} LConfig;

typedef struct {
    LConfig lConfig;
    State result;
} Transition;

typedef struct {
    Transition *pTransitionArr;
    int size;
} Rule;

void showIntegerArray(State* pArray, int size) {
    if (size == 0)
        printf("size zero !!!");
    else {
        printf("%d", pArray[0]);
        for (int i = 1; i < size; i++)
            printf(" %d", pArray[i]);
    }
}

void showLConfig(LConfig* pLConfig) {
    showIntegerArray(pLConfig->pStateArr, 3);
}

void showTransition(Transition* pTransition) {
    showLConfig(&(pTransition->lConfig));
    printf(" -> %d", pTransition->result);
}

void showRule(Rule* pRule) {
    for (int i = 0; i < pRule->size; i++) {
        showTransition(&(pRule->pTransitionArr[i]));
        printf("\n");
    }
    
    printf("Rule size = %d\n", pRule->size);
}

Rule ruleFromFile(FILE* file) {
    Rule rule;
    rule.pTransitionArr = calloc(150, sizeof(Transition));
    rule.size = 0;

    char line[BUFF_SIZE];
    fgets(line, BUFF_SIZE, file);//premier ligne pour l'information de la solution

    while (fgets(line, BUFF_SIZE, file) != NULL) {
        char* token;
        rule.pTransitionArr[rule.size].lConfig.pStateArr[0] = strtol(line, &token, 10);
        rule.pTransitionArr[rule.size].lConfig.pStateArr[1] = strtol(token, &token, 10);
        rule.pTransitionArr[rule.size].lConfig.pStateArr[2] = strtol(token, &token, 10);
        rule.pTransitionArr[rule.size].result = strtol(token, NULL, 10);
        rule.size++;
    }

    return rule;
}

void initializeBorder(State* gc, int size) {
    gc[0] = 6;
    gc[size + 1] = 6;
}

void initializeGConfig(State *gc, int size) {
    initializeBorder(gc, size);
    gc[1] = 1;
    memset(gc + 2, 0, size - 1);
}

boolean equals(LConfig *plc1, LConfig *plc2) {
    for (int i = 0; i <= 2; i++) {
        if (plc1->pStateArr[i] != plc2->pStateArr[i])
            return false;
    }

    return true;
}

void swap(void **a, void **b) {
    void *c = *a;
    *a = *b;
    *b = c;
}

LConfig* getLConfig(State *pGConfig, int pos) {
    return (LConfig*)(pGConfig + pos);
}

State getResult(Rule rule, LConfig *plc) {
    for (int i = 0; i < rule.size; i++) {
        if (equals(plc, &(rule.pTransitionArr->lConfig)))//memcmp
            return rule.pTransitionArr->result;
        
        rule.pTransitionArr++;
    }

    return -1;
}

Transition newTransition(LConfig *pLConfig, State result) {
    return (Transition){lConfig : *pLConfig, result : result};
}

void addTransition(Rule *pRule, Transition *pTransition) {
    pRule->pTransitionArr[pRule->size] = *pTransition;
    pRule->size++;
}

void oneStep(Rule *pRule, State *pStateArrSrc, State *pStateArrDst, int size) {
    for (int pos = 1; pos <= size; pos++) {
        LConfig* pLConfig = getLConfig(pStateArrSrc, pos - 1);
        // showLConfig(pLConfig);
        // printf("\n");
        pStateArrDst[pos] = getResult(*pRule, pLConfig);
    }
}

boolean checkLocalMapping(Rule* ruleSrc, Rule* ruleDst, Rule* localMapping) {
    // printf("in\n");
    localMapping->size = 0;
    int MAX_SIZE = 500;
    State *gcSrc = calloc(MAX_SIZE + 2, sizeof(State));
    State *gcDst = calloc(MAX_SIZE + 2, sizeof(State));
    State *gcAux = calloc(MAX_SIZE + 2, sizeof(State));

    for (int size = 2; size <= MAX_SIZE; size++) {
        initializeGConfig(gcSrc, size);
        initializeBorder(gcDst, size);
        oneStep(ruleDst, gcSrc, gcDst, size);
        initializeBorder(gcAux, size);
        
        int finalTime = 2 * size - 2;
        for (int t = 0; t < finalTime; t++) {
            for (int p = 0; p < size; p++) {
                LConfig* lcSrc = getLConfig(gcSrc, p);
                State resDst = gcDst[p + 1];
                State oldRes = getResult(*localMapping, lcSrc);

                if (oldRes == -1) {
                    Transition transition = newTransition(lcSrc, resDst);
                    addTransition(localMapping, &transition);
                }
                else if (oldRes != resDst) {
                    printf("(%d) ", size);
                    return false;
                }
            }

            oneStep(ruleSrc, gcSrc, gcAux, size);
            swap((void**)&gcSrc, (void**)&gcAux);

            oneStep(ruleDst, gcDst, gcAux, size);
            swap((void**)&gcDst, (void**)&gcAux);
        }
    }

    printf("(%d) ", MAX_SIZE);

    free(gcSrc);
    free(gcDst);
    free(gcAux);

    return true;
}

int main(int argc, char *argv[]) {    
    FILE *fileArr[NUMBER_SOLUTION];
    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        char fileName[30];
        snprintf(fileName, 30, "./data/718/%d.txt", i);
        fileArr[i] = fopen(fileName, "r");
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

    //showRule(&ruleArr[718]);
    //TODO il faut vérifier dans l'article de 718 jusqu'à quelle taille pour la simulation et le noté
    //TODO argumenter si possible sur cette taille 
    Rule localMapping;
    localMapping.pTransitionArr = calloc(NB_MAX_TRANSITION, sizeof(Transition));
    for (int i = 0; i < NUMBER_SOLUTION - 1; i++) {
        for (int j = i + 1; j < NUMBER_SOLUTION; j++) {
            //if (i != j) {
                boolean isDet = checkLocalMapping(&ruleArr[i], &ruleArr[j], &localMapping);
                if (isDet == true)
                    printf("%d -> %d\n", i, j);
                else
                    printf("%d x> %d\n", i, j);

                isDet = checkLocalMapping(&ruleArr[j], &ruleArr[i], &localMapping);
                if (isDet == true)
                    printf("%d -> %d\n", j, i);
                else
                    printf("%d x> %d\n", j, i);
            //}
        }
    }
    free(localMapping.pTransitionArr);

    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        ruleArr[i].size = 0;
        free(ruleArr[i].pTransitionArr);
    }

    return 0;
}