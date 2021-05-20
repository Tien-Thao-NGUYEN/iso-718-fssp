#include <stdio.h>
#include <stdlib.h>
#include "rule.h"

LConfig newLConfig(int l, int c, int r)
{
    LConfig lConfig;
    lConfig.pStateArr[0] = l;
    lConfig.pStateArr[1] = c;
    lConfig.pStateArr[2] = r;

    return lConfig;
}

int getState(GConfig *pGConfig, int pos)
{
    if (pos < 0 || pos >= pGConfig->size)
        return 6;

    return pGConfig->pStateArr[pos];
}

LConfig getLConfig(GConfig *pGConfig, int pos)
{
    int l = getState(pGConfig, pos - 1);
    int c = pGConfig->pStateArr[pos];
    int r = getState(pGConfig, pos + 1);

    return newLConfig(l, c, r);
}

Transition newTransition(LConfig *pLConfig, int result)
{
    return (Transition){pLConfig : *pLConfig, result : result};
}

boolean equals(LConfig *plc1, LConfig *plc2)
{
    for (int i = 0; i < 3; i++)
    {
        if (plc1->pStateArr[i] != plc2->pStateArr[i])
            return false;
    }

    return true;
}

int getResult(Rule *pRule, LConfig *plc)
{
    for (int i = 0; i < pRule->size; i++)
    {
        Transition transition = pRule->pTransitionArr[i];
        if (equals(plc, &(transition.pLConfig)))
            return transition.result;
    }

    return -1;
}

void addTransition(Rule *pRule, Transition *pTransition)
{
    pRule->pTransitionArr[pRule->size] = *pTransition;
    pRule->size++;
}

GConfig getInitGConfig(int size)
{
    int *pStateArr = calloc(size, sizeof(int));
    pStateArr[0] = 1;
    for (int i = 1; i < size; i++)
        pStateArr[i] = 0;

    return (GConfig){pStateArr : pStateArr, size : size};
}

GConfig oneStep(Rule *pRule, GConfig *pGConfig)
{
    GConfig nextGConfig;
    nextGConfig.pStateArr = calloc(pGConfig->size, sizeof(int));
    for (int pos = 0; pos < pGConfig->size; pos++)
    {
        LConfig lConfig = getLConfig(pGConfig, pos);
        nextGConfig.pStateArr[pos] = getResult(pRule, &lConfig);
    }

    nextGConfig.size = pGConfig->size;

    return nextGConfig;
}

Diagram getDiagram(Rule *pRule, GConfig *pInitGConfig)
{
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