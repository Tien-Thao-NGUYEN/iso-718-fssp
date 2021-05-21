#include <stdio.h>
#include <stdlib.h>
#include "rule.h"


LConfig* getLConfig(State *pGConfig, int pos) {
    return (LConfig*)(pGConfig + pos);
}

State getState(GConfig *pGConfig, int pos) {
    if (pos < 0 || pos >= pGConfig->size)
        return 6;

    return pGConfig->pStateArr[pos];
}

Transition newTransition(LConfig *pLConfig, int result) {
    return (Transition){pLConfig : *pLConfig, result : result};
}

boolean equals(LConfig *plc1, LConfig *plc2) {
    for (int i = 0; i < 3; i++) {
        if (plc1->pStateArr[i] != plc2->pStateArr[i])
            return false;
    }

    return true;
}

int getResult(Rule rule, LConfig *plc)
{
    for (int i = 0; i < rule.size; i++) {
        if (equals(plc, &(rule.pTransitionArr.pLConfig)))//memcmp
            return transition.result;
        
        rule.pTransitionArr++;
    }

    return -1;
}

void addTransition(Rule *pRule, Transition *pTransition) {
    pRule->pTransitionArr[pRule->size] = *pTransition;
    pRule->size++;
}

GConfig getInitGConfig(int size) {
    int *pStateArr = calloc(size, sizeof(int));
    pStateArr[0] = 1;
    for (int i = 1; i < size; i++)
        pStateArr[i] = 0;

    return (GConfig){pStateArr : pStateArr, size : size};
}

void oneStep(Rule *pRule, State *pStateArrSrc, State *pStateArrDst, int size) {
    for (int pos = 0; pos < size; pos++) {
        LConfig lConfig = getLConfig(pGConfig, pos);
        pStateArr[pos] = getResult(pRule, &lConfig);
    }
}

void getDiagram(Rule *pRule, GConfig *pInitGConfig) {
    int timeFin = 2 * pInitGConfig->size - 2;
    Diagram dgm;
    dgm.pGConfigArr = calloc(timeFin + 1, sizeof(GConfig)); //+1 pour initGConfig
    dgm.pGConfigArr[0] = *pInitGConfig;
    for (int t = 1; t <= timeFin; t++)
    {
        GConfig nextGConfig = oneStep(pRule, &(dgm.pGConfigArr[t - 1]));
        dgm.pGConfigArr[t] = nextGConfig;
    }

    dgm.timeFin = timeFin;

    return dgm;
}

void freeDiagram(Diagram *pDiagram) {    
    free(pDiagram->pGConfigArr);
}