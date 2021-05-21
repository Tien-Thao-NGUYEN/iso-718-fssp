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

typedef struct {
    State *pStateArr;
    int size;
} GConfig;

typedef struct {
    State *pStateArr;
    int timeFin;
    int sizeGConfig;
} Diagram;


LConfig* getLConfig(GConfig*, int pos);

GConfig getInitGConfig(int configSize);

boolean equals(LConfig*, LConfig*);
State getResult(Rule*, LConfig*);

Transition newTransition(LConfig*, State);
void addTransition(Rule*, Transition*);

GConfig oneStep(Rule* , GConfig*);
Diagram getDiagram(Rule* , GConfig*);
void freeDiagram(Diagram*);