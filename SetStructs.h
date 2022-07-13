// COMP2521 22T2 Assignment 1
typedef struct node *tree;
typedef struct cursor *SetCursor;
// DO NOT CHANGE THE NAME OF THIS STRUCT
struct node {
	int item;           // DO NOT CHANGE/REMOVE THIS FIELD
	struct node *left;  // DO NOT CHANGE/REMOVE THIS FIELD
	struct node *right; // DO NOT CHANGE/REMOVE THIS FIELD
	struct node *next;
	int height;

};

// DO NOT CHANGE THE NAME OF THIS STRUCT
struct set {
	struct node *tree;  // DO NOT CHANGE/REMOVE THIS FIELD
	// You may add more fields here if needed
	int size;
	struct node *min;
	struct node *max;
	struct node *next;
};

// You may define more structs here if needed 

////////////////////////////////////////////////////////////////////////
// Cursors

struct cursor {
	struct node *currTree;
	int item;
	// You may add more fields here if needed
};

////////////////////////////////////////////////////////////////////////

