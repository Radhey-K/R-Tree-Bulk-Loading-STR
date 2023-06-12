#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define M 4 // max number of entries in a node


// -----------------STRUCTURES-------------------------
typedef struct pair {
    int a;
    int b;
} pair;

typedef struct rectangle {
    pair p1;
    pair p2;
} rectangle;

typedef struct node node;
typedef struct entry {
    rectangle r;
    node *n;
} entry;

struct node {
    int isleaf;
    int count;
    entry arr[M];
};

typedef struct STRtree {
    node *root;
} STRtree;
// -----------------------------------------------------


// -----------------GLOBAL VARIABLES-------------------
pair *inp;
int pt_count;
int leafStatus = 1; // 1 if leaf, 0 if not
int P;
int S;
node** nodeAddresses; // Stores the addresses of all the nodes
node *root; 
// ----------------------------------------------------


// returns an STRTree with the given root node
STRtree* createSTRtree(node *rootNode) {
    STRtree *retTree = malloc(sizeof(STRtree));
    retTree->root = rootNode;
    return retTree;
}


// reads data from the file and stores it in the inp array
void readData(FILE *fp, pair *inp, int pt_count) {
    // reset the file pointer to the beginning of the file
    fseek(fp, 0, SEEK_SET);

    int p,q;
    for (int i = 0; i < pt_count; i++)
    {
        fscanf(fp, "%d %d\n", &p, &q);
        inp[i].a = p;
        inp[i].b = q;
    }
}


// sorts the array of entries according to the x coordinate of the centre of the rectangle
void sortByX(entry *arr, int dataPoints) {
    for(int j = 1; j < dataPoints; j++){
        int i = j - 1;
        entry v = arr[j];
        while(i >= 0 && (v.r.p1.a + v.r.p1.b) / 2.0 < (arr[i].r.p1.a + arr[i].r.p1.b) / 2.0){
            arr[i + 1] = arr[i];
            i--;
        }
        arr[i + 1] = v;
    }
}


// sorts the sliced array of entries according to the y coordinate of the centre of the rectangle
void sortByY(entry *arr, int sliceSize, int dataPoints) {

    float slices = ((float)dataPoints) / sliceSize;
    int intSlice = (int)slices;

    for(int k = 0; k < intSlice; k++){   
        for(int j = k * sliceSize + 1; j < (k + 1) * sliceSize; j++){
            int i = j - 1;
            entry v = arr[j];
            while(i >= k * sliceSize && (v.r.p2.a + v.r.p2.b) / 2.0 < (arr[i].r.p2.a + arr[i].r.p2.b) / 2.0){
                arr[i + 1] = arr[i];
                i--;
            }
            arr[i + 1] = v;
        }  
    }

    if(slices != intSlice){
        for(int j = sliceSize * intSlice + 1; j < dataPoints; j++){
            int i = j - 1;
            entry v = arr[j];
            while(i >= sliceSize * intSlice && v.r.p2.a < arr[i].r.p2.a){
                arr[i + 1] = arr[i];
                i--;
            }
            arr[i + 1] =v;
        }
    }
}


// creates a node with the given array of entries and returns the node
node *createNode(entry *e_arr, int arrSize) {
    node *retNode = malloc(sizeof(node));

    if (leafStatus == 1) {
        retNode->isleaf = 1;
    }
    else {
        retNode->isleaf = 0;
    }

    retNode->count = arrSize;
    for (int i = 0; i < arrSize; i++)
    {

        retNode->arr[i] = e_arr[i];
    }
    return retNode;
}


// -----------------HELPER FUNCTIONS-------------------
int mini(int a, int b) {
    return a < b ? a : b;
}

int maxi(int a, int b) {
    return a > b ? a : b;
}
// -----------------------------------------------------


// finds the MBR of the entries in the given node
rectangle findMBR(node *nodePtr) {
    int x_low = nodePtr->arr[0].r.p1.a;
    int x_high = nodePtr->arr[0].r.p1.b;
    int y_low = nodePtr->arr[0].r.p2.a;
    int y_high = nodePtr->arr[0].r.p2.b;

    for (int i = 0; i < nodePtr->count; i++)
    {   
        x_low = mini(x_low, nodePtr->arr[i].r.p1.a);
        x_high = maxi(x_high, nodePtr->arr[i].r.p1.b);
        y_low = mini(y_low, nodePtr->arr[i].r.p2.a);
        y_high = maxi(y_high, nodePtr->arr[i].r.p2.b);
    }
    
    rectangle retRect;
    retRect.p1.a = x_low;
    retRect.p1.b = x_high;
    retRect.p2.a = y_low;
    retRect.p2.b = y_high;

    return retRect;
}


// used to build STR tree to convert the array of nodes to array of entries
entry *convertNodesToEntry(int entrySize) {
    entry *retEntry = malloc(entrySize * sizeof(entry));

    for (int i = 0; i < entrySize; i++)
    {
        rectangle newRect = findMBR(nodeAddresses[i]);
        retEntry[i].r = newRect;
        retEntry[i].n = nodeAddresses[i];
    }
    
    return retEntry;
}


// Main function to build the STR tree recursively starting from leaf nodes upto the root node in bottom up fashion
void buildSTRtree(entry *arr, int noPoints) {
    if (noPoints == 1)
        return;

    int P = ceil(noPoints / (float)M);
    int S = ceil(sqrt(P));

    sortByX(arr, noPoints);
    int sliceSize = S * M;
    sortByY(arr, sliceSize, noPoints);


    // Bulk loading the nodes with the sorted entries
    for (int i = 0; i < P - 1; i++)
    {
        entry *e_arr = malloc(M * sizeof(entry));
        for (int j = 0; j < M; j++)
        {
            e_arr[j] = arr[M * i + j];
        }
        node *retNode = createNode(e_arr, M);
        // storing the node address in the array of node addresses
        nodeAddresses[i] = retNode; 
    }
    
    int remPoints = noPoints - ((P - 1) * M);  // last node may have less than M entries

    entry *e_arr = malloc(remPoints * sizeof(entry));
    for (int i = 0; i < remPoints; i++)
    {
        e_arr[i] = arr[M * (P - 1) + i];
    }
    nodeAddresses[P - 1] = createNode(e_arr, remPoints);

    if (leafStatus == 1) leafStatus = 0;  // to make the next level as internal nodes

    buildSTRtree(convertNodesToEntry(P), P);  // recursive call to build the upper level
}


// converts the input points to entries
entry *convertToEntry(pair *inp) {
    entry *arr = malloc(pt_count * sizeof(entry));
    for (int i = 0; i < pt_count; i++)
    {
        arr[i].r.p1.a = inp[i].a;
        arr[i].r.p1.b = inp[i].a;
        arr[i].r.p2.a = inp[i].b;
        arr[i].r.p2.b = inp[i].b;
        arr[i].n = NULL;
    }

    return arr;
}


// prints the STR tree in preorder traversal
void preorderTraversal(node* rootPtr){
    rectangle rect = findMBR(rootPtr);
    if(rootPtr->isleaf == 1){
        printf("Printing leaf node : Bottom left = (%d,%d) and Top right = (%d,%d)\n",rect.p1.a,rect.p2.a,rect.p1.b,rect.p2.b);
        for(int i = 0; i < rootPtr->count; i++){
            printf("Printing data points: (%d,%d)\n",rootPtr->arr[i].r.p1.a,rootPtr->arr[i].r.p2.a);
        }
    }
    else{
        printf("Printing internal node : Bottom left = (%d,%d) and Top right = (%d,%d)\n",rect.p1.a,rect.p2.a,rect.p1.b,rect.p2.b);
        for(int i = 0; i < rootPtr->count; i++){
            preorderTraversal(rootPtr->arr[i].n);
        }
    }
}


int main(int argc, char *argv[]) {
    // Provide input file name as command line argument like ./a.out data.txt
    if (argc != 2) {
        printf("Usage: ./a.out <input_file>\n");
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");

    // Check if file exists
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    // Read number of points in data.txt into pt_count
    int p,q;
    while(fscanf(fp, "%d %d\n", &p, &q) != EOF) {
        pt_count++;
    }

    // inp stores the points
    inp = malloc(sizeof(pair) * pt_count);

    // Read data.txt into inp 
    readData(fp, inp, pt_count);

    entry *arr = convertToEntry(inp);

    nodeAddresses  = malloc(((pt_count / M) + 10) * sizeof(node *));

    buildSTRtree(arr, pt_count);
    STRtree *tree = createSTRtree(*nodeAddresses);

    root = tree->root;

    preorderTraversal(root);
    
    return 0;
}
