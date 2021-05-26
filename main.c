#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <search.h>


#define BUFF_SIZE 20
#define NUMBER_SOLUTION 719
#define NB_MAX_TRANSITION 150

#define true 1
#define false 0

typedef int boolean;
typedef char State;
typedef struct hsearch_data *Rule;


typedef struct {
    State pStateArr[3];
} LConfig;


Rule ruleFromFile(FILE* file) {
    Rule rule = malloc(sizeof(struct hsearch_data));
    hcreate_r(150 * 1.5, rule);

    char line[BUFF_SIZE];
    fgets(line, BUFF_SIZE, file);

    char lc[4];
    lc[3] = 0;

    ENTRY e;
    e.key = lc;
    while (fgets(line, BUFF_SIZE, file) != NULL) {
        char* token;
        lc[0] = strtol(line, &token, 10) + 1;
        lc[1] = strtol(token, &token, 10) + 1;
        lc[2] = strtol(token, &token, 10) + 1;

        e.data = (void*) (strtol(token, NULL, 10) + 1);
        hsearch_r(e, ENTER, NULL, rule);
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

void swap(void **a, void **b) {
    void *c = *a;
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

boolean checkLocalMapping(Rule ruleSrc, Rule ruleDst) {
    struct hsearch_data localmappingData;
    Rule localMapping = &localmappingData;
    hcreate_r(150 * 1.5, localMapping);
    
    int MAX_SIZE = 500;
    State *gcSrc = calloc(MAX_SIZE + 2, sizeof(State));
    State *gcDst = calloc(MAX_SIZE + 2, sizeof(State));
    State *gcAux = calloc(MAX_SIZE + 2, sizeof(State));

    char lc[4];
    lc[3] = 0;

    ENTRY e;
    e.key = lc;
    ENTRY *res;

    for (int size = 2; size <= MAX_SIZE; size++) {
        initializeGConfig(gcSrc, size);
        initializeBorder(gcDst, size);
        oneStep(ruleDst, gcSrc, gcDst, size);
        initializeBorder(gcAux, size);
        
        int finalTime = 2 * size - 2;
        for (int t = 0; t < finalTime; t++) {
            for (int p = 0; p < size; p++) {
                memcpy(lc, gcSrc + p, 3);

                e.data = (void*) (gcDst[p + 1]);
                hsearch_r(e, FIND, &res, localMapping);

                if (res == NULL) {
                    hsearch_r(e, ENTER, NULL, localMapping);
                }
                else if (res->data != e.data) {
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
            //if (i != j) {
                boolean isDet = checkLocalMapping(ruleArr[i], ruleArr[j]);
                if (isDet == true)
                    printf("%d -> %d\n", i, j);
                else
                    printf("%d x> %d\n", i, j);

                isDet = checkLocalMapping(ruleArr[j], ruleArr[i]);
                if (isDet == true)
                    printf("%d -> %d\n", j, i);
                else
                    printf("%d x> %d\n", j, i);
            //}
        }
    }

    for (int i = 0; i < NUMBER_SOLUTION; i++) {
        hdestroy_r(ruleArr[i]);
        free(ruleArr[i]);
    }

    return 0;
}