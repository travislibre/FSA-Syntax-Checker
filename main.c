/*
Written by Travis Libre, Raj Raman, and Jonathan Ginter
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define maxVertices 10
#define LINELIMIT 500
int globalIsValid = 0;
int globalStartState;
int globalFinalState; //PLACEHOLDER, TO BE SET DURING PROGRAM INITIALIZATION

// Data structure to store a graph object
typedef struct Graph {
    // An array of pointers to Node to represent an adjacency list
    struct Node *head[maxVertices];
} graph;

// Data structure to store adjacency list nodes of the graph
typedef struct Node {
    char key;
    int myName;
    int dest;
    struct Node *next;
} node;

// Data structure to store a graph edge
typedef struct Edge {
    int src; // the int equivalent of "q0" in (q0,1,q1)
    char key; // the "1" in (q0,1,q1)
    int dest;// the int equivalent of "q1" in (q0,1,q1)
} edge;

// Function to create an adjacency list from specified edges
graph *createGraph(struct Edge edges[], int n) {
    // allocate storage for the pGraph data structure
    graph *pGraph = (graph *) malloc(sizeof(graph));
    // initialize head pointer for all vertices
    for (int i = 0; i < maxVertices; i++) {
        pGraph->head[i] = NULL;
    }
    // add edges to the directed pGraph one by one
    for (int i = 0; i < n; i++) {
        // get the source and destination vertex
        int src = edges[i].src;
        char key = edges[i].key;
        int dest = edges[i].dest;

        // allocate a new node of adjacency list from src to dest
        node *newNode = (node *) malloc(sizeof(node));
        newNode->dest = dest;
        newNode->myName = src;
        newNode->key = key;
        newNode->next = pGraph->head[src];
        pGraph->head[src] = newNode;
    }
    return pGraph;
}

void processString(const char *string, int index, graph *pGraph, node *currentNode) {
    while (currentNode != NULL) //while node exists
    {
        if (currentNode->key == string[index]) { // if path key matches string[index]
            node *nextNode = pGraph->head[currentNode->dest]; // navigate to next node from path
            if (currentNode->dest == globalFinalState &&
                string[index + 1] == '\0') { // if you are about to reach the end, flag=1
                globalIsValid = 1;
            } else if (nextNode != NULL) { //if path is valid
                processString(string, (index + 1), pGraph, nextNode); //recursively navigate graph
            }
        }
        currentNode = currentNode->next; // try next path if this connection didn't match
    }
}

node *findStart(graph *pGraph) { //Assumes globalStartState has been initialized
    node *tempNode = pGraph->head[0]; //Start at graph head
    int i = 1;
    while (tempNode != NULL && tempNode->myName != globalStartState) { // While (not start)
        tempNode = pGraph->head[i];  // Go to next node
        i++;
    }
    if (tempNode == NULL) {
        printf("Starting node not found!\n");
    }
    return tempNode;
}

void removeChar(char *string, char c) {
    int length = strlen(string);
    int index;
    for (int i = index = 0; i < length; i++) { // Loop until at end
        if (string[i] != c) { // If not c
            string[index++] = string[i]; // Add to string
        }
    }
    string[index] = '\0';
}

void clean(char *string) { //Removes following characters from a string
    removeChar(string, '(');
    removeChar(string, ')');
    removeChar(string, ' ');
    removeChar(string, '\n');
}

int countCommas(const char *s) { // Counts & returns count of commas in a string
    int count = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] == ',') {
            count++;
        }
    }
    return count;
}

void userProcessString(graph *graph) {
    int firstPass = 1;
    int leave = 1;
    while (leave) {
        if (firstPass) { // If first ask
            printf("Please input a string:\t");
            firstPass = 0;
        } else
            printf("Please input another string:\t");
        char userInput[50];
        scanf("%49s", userInput); // Read user input from stdin
        if (userInput[0] == '\0' || userInput[0] == '~') { // If no input or tilde (special escape), exit loop
            leave = 0;
        }
        processString(userInput, 0, graph, findStart(graph)); // processes user input
        if (globalIsValid) {
            printf("Accepted.\n");
        } else {
            printf("Rejected.\n");
        }
        globalIsValid = 0; // Reset for next string
    }
    printf("Bye bye.");
}

void readFile(FILE *infile) {
    char buffer[LINELIMIT];
    char initialState[10];
    char finalState[10];
    char connections[1500];
    char testStrings[1500];
    //Skip first 4 lines
    fgets(buffer, LINELIMIT, infile);
    fgets(buffer, LINELIMIT, infile);
    fgets(buffer, LINELIMIT, infile);
    fgets(buffer, LINELIMIT, infile);
    //Get initial state, final state, edges
    fgets(initialState, LINELIMIT, infile);
    fgets(finalState, LINELIMIT, infile);
    fgets(connections, 1500, infile);
    //Skips empty line, gets test strings
    fgets(buffer, LINELIMIT, infile);
    fgets(testStrings, 1500, infile);
    clean(testStrings);
    int boolStringsProvided; // Test if we need user input strings
    if (testStrings[0] == '\0')
        boolStringsProvided = 0;
    else {
        boolStringsProvided = 1;
    }
    int stringCount = countCommas(testStrings) + 1; // Count strings
    char *allTestStrings[stringCount]; // String array of test strings
    int i = 0;
    if (boolStringsProvided) {
        char *p = strtok(testStrings, ","); // Split into substrings
        while (p != NULL) {
            allTestStrings[i++] = p; // Store to string array
            p = strtok(NULL, ",");
        }
    }
    //Cleans the inital and final state strings
    clean(initialState);
    clean(finalState);
    removeChar(initialState, ',');
    removeChar(finalState, ',');
    removeChar(initialState, 'q');
    removeChar(finalState, 'q');
    //Sets globalStart to initial and globalFinal to final
    globalStartState = atoi(initialState);
    globalFinalState = atoi(finalState);

    //Clean the string containing edges
    clean(connections);
    char originState[10];
    char key[10];
    char destinationState[10];

    //Counts the number of commas in "connections" in order to determine edge count
    int edgeCount = countCommas(connections);
    edgeCount = (edgeCount + 1) / 3; // Adds 1 bc of offset then divides by 3 to group the edges into (q0,key,q1)
    edge edges[edgeCount]; // Initialize edge array of size edgeCount
    int count = 0; // Used to index edge creation
    char *token = strtok(connections, ","); // Gets first substring deliminated by ','
    while (token != NULL) {
        strcpy(originState, token); // Save first substring (origin state)
        token = strtok(NULL, ",");  // Get next substring
        strcpy(key, token);          // Save second substring (key)
        token = strtok(NULL, ","); // Get next substring
        strcpy(destinationState, token); // Save third substring (destination state)
        token = strtok(NULL, ","); // Get next substring
        //Clean the string then convert it into an integer so we can create an edge out of it
        removeChar(originState, 'q');
        removeChar(destinationState, 'q');
        int src = atoi(originState); // Convert string to int
        int dest = atoi(destinationState);
        edge temp = {src, key[0], dest}; //Initialize and save edge
        edges[count] = temp; //Assign to edge array
        count++;
    }
    // construct a graph from the given edges
    graph *graph = createGraph(edges, edgeCount);
    if (boolStringsProvided) {
        printf("(");
        for (int j = 0; j < stringCount; j++) { // For every input string, run processString()
            processString(allTestStrings[j], 0, graph, findStart(graph));
            if (globalIsValid)
                printf("accepted");
            else
                printf("rejected");
            globalIsValid = 0;
            if (j < stringCount - 1) {
                printf(",");
            }
        }
        printf(")\n");
    } else { // Else run userProcessString
        userProcessString(graph);
    }

}

int main(void) {

    printf("Type file name:\t");
    char userInput[40];
    scanf("%39s", userInput);
    FILE *infile = fopen(userInput, "r");
    if (infile == NULL) {
        printf("Invalid file name.\n");
    } else {
        readFile(infile);
    }

    return 0;
}