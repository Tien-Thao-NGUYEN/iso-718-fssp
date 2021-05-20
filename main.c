#include <stdio.h>
#include <stdlib.h>
#include "show.h"


void testAddTransition(Rule* rule);
boolean checkDgm(Diagram* dgmSrc, Diagram* dgmDst, Rule* localMapping);


int main(int argc, char *argv[]) {
    GConfig gc0 = getInitGConfig(10);
    //showGConfig(&gc0);
    //printf("\n");

    LConfig lc = getLConfig(&gc0, 0);
    //showLConfig(&lc);
    //printf("\n");

    Rule rule;
    rule.pTransitionArr = calloc(10, sizeof(Transition));
    rule.size = 0;

    Diagram dgmSrc;
    dgmSrc.pGConfigArr = calloc(3, sizeof(GConfig));
    dgmSrc.pGConfigArr[0] = gc0;
    dgmSrc.pGConfigArr[1] = gc0;
    dgmSrc.pGConfigArr[2] = gc0;
    dgmSrc.timeFin = 2;

    showDiagram(&dgmSrc);
    printf("\n");

    checkDgm(&dgmSrc, &dgmSrc, &rule);

    showRule(&rule);
    printf("\n");

    return 0;
}


boolean checkDgm(Diagram* dgmSrc, Diagram* dgmDst, Rule* localMapping) {
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
            }
            else if (oldRes != resDst)
                    return false;
        }
    }

    return true;
}

void testAddTransition(Rule* rule) {
    LConfig lc = newLConfig(1,2,3);
    Transition tr = newTransition(&lc, 4);
    addTransition(rule, &tr);
}