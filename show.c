#include <stdio.h>
#include "show.h"


void showIntegerArray(int* pArray, int size) {
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
    showLConfig(&(pTransition->pLConfig));
    printf(" -> %d", pTransition->result);
}

void showRule(Rule* pRule) {
    for (int i = 0; i < pRule->size; i++) {
        showTransition(&(pRule->pTransitionArr[i]));
        printf("\n");
    }
}

void showGConfig(GConfig* pGConfig) {
    showIntegerArray(pGConfig->pStateArr, pGConfig->size);
}

void showDiagram(Diagram* pDiagram) {
    for (int t = 0; t <= pDiagram->timeFin; t++) {
        showGConfig(&(pDiagram->pGConfigArr[t]));
        printf("\n");
    }
}