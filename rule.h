#define true 1
#define false 0
typedef int boolean;

typedef struct {
    int pStateArr[3];
} LConfig;

typedef struct {
    LConfig pLConfig;
    int result;
} Transition;

typedef struct {
    Transition* pTransitionArr;
    int size;
} Rule;

typedef struct {
    int* pStateArr;
    int size;
} GConfig;

typedef struct {
    GConfig* pGConfigArr;
    int timeFin;
} Diagram;


LConfig newLConfig(int, int, int);
LConfig getLConfig(GConfig*, int);

GConfig getInitGConfig(int);

boolean equals(LConfig*, LConfig*);
int getResult(Rule*, LConfig*);

Transition newTransition(LConfig*, int);
void addTransition(Rule*, Transition*);

GConfig oneStep(Rule* , GConfig*);
Diagram getDiagram(Rule* , GConfig*);