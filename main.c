#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <search.h>


#define BUFF_SIZE 20
#define NUMBER_SOLUTION 719
#define NB_MAX_TRANSITION 150

typedef char State;
typedef struct hsearch_data *Rule;

typedef struct {
    State pStateArr[3];
} LConfig;


Rule ruleFromFile(FILE* file) {
    Rule rule = malloc(sizeof(struct hsearch_data));
    *rule = (struct hsearch_data){0};
    hcreate_r(150 * 1.5, rule);

    char line[BUFF_SIZE];
    fgets(line, BUFF_SIZE, file);

    // TODO: add the corresponding free of the following calloc ?
    char *lcs = calloc(150, 4 * sizeof(char));

    for (int i = 0; fgets(line, BUFF_SIZE, file) != NULL; ) {
        char* token;
        lcs[i++] = strtol(line, &token, 10) + 1;
        lcs[i++] = strtol(token, &token, 10) + 1;
        lcs[i++] = strtol(token, &token, 10) + 1;
        lcs[i++] = 0;
        ENTRY e, *res;
        e.key = lcs + (i - 4);
        e.data = (void*) (strtol(token, NULL, 10) + 1);
        hsearch_r(e, ENTER, &res, rule);
    }

    return rule;
}

void initializeBorder(State* gc, int size) {
    gc[0] = 6 + 1;
    gc[size + 1] = 6 + 1;
}

void initializeGConfig(State *gc, int size) {
    initializeBorder(gc, size);
    gc[1] = 2;
    memset(gc + 2, 1, size - 1);
}

void swap(State **a, State **b) {
    State *c = *a;
    *a = *b;
    *b = c;
}

void oneStep(Rule rule, State *pStateArrSrc, State *pStateArrDst, int size) {
    State lc[4];
    lc[3] = 0;

    ENTRY e;
    e.key = lc;
    ENTRY *res;

    for (int pos = 1; pos <= size; pos++) {
        memcpy(lc, pStateArrSrc + pos -1, 3);
        hsearch_r(e, FIND, &res, rule);
        pStateArrDst[pos] = (State)(res->data);
    }
}

bool checkLocalMapping(Rule ruleSrc, Rule ruleDst) {
    struct hsearch_data localmappingData = {0};
    Rule localMapping = &localmappingData;
    hcreate_r(150 * 1.5, localMapping);
    
    int MAX_SIZE = 500;
    State *gcSrc = calloc(MAX_SIZE + 2, sizeof(State));
    State *gcDst = calloc(MAX_SIZE + 2, sizeof(State));
    State *gcAux = calloc(MAX_SIZE + 2, sizeof(State));

    char lc[4];
    lc[3] = 0;

    for (int size = 2; size <= MAX_SIZE; size++) {
        initializeGConfig(gcSrc, size);
        initializeBorder(gcDst, size);
        oneStep(ruleDst, gcSrc, gcDst, size);
        initializeBorder(gcAux, size);
        
        int finalTime = 2 * size - 2;
        int t = 0;
        while (true) {
            for (int p = 0; p < size; p++) {
                ENTRY e;
                ENTRY *res;
                memcpy(lc, gcSrc + p, 3);
                e.key = lc;
                e.data = (void*) (gcDst[p + 1]);
                hsearch_r(e, FIND, &res, localMapping);

                if (res == NULL) {
                    hsearch_r(e, FIND, &res, ruleSrc);
                    e.key = res->key;
                    hsearch_r(e, ENTER, &res, localMapping);
                }
                else if (res->data != e.data) {
                    printf("(%d) ", size);
                    return false;
                }
            }

            t++;
            if (t < finalTime) {
                oneStep(ruleSrc, gcSrc, gcAux, size);
                swap(&gcSrc, &gcAux);

                oneStep(ruleDst, gcDst, gcAux, size);
                swap(&gcDst, &gcAux);
            }
            else
                break;
        }
    }

    printf("(%d) ", MAX_SIZE);

    free(gcSrc);
    free(gcDst);
    free(gcAux);

    hdestroy_r(localMapping);

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

    

    for (int i = 0; i < NUMBER_SOLUTION - 1; i++) {
        for (int j = i + 1; j < NUMBER_SOLUTION; j++) {
            bool isDet = checkLocalMapping(ruleArr[i], ruleArr[j]);
            if (isDet)
                printf("%d -> %d\n", i, j);
            else
                printf("%d x> %d\n", i, j);

            isDet = checkLocalMapping(ruleArr[j], ruleArr[i]);
            if (isDet)
                printf("%d -> %d\n", j, i);
            else
                printf("%d x> %d\n", j, i);
        }
    }

    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        hdestroy_r(ruleArr[i]);
        free(ruleArr[i]);
    }

    return 0;
}