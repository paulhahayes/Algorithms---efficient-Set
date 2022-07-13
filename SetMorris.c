// Implementation of the Set ADT using a balanced BST
// COMP2521 22T2 Assignment 1

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Set.h"
#include "SetStructs.h"

// my defines

#define UNION 1
#define INTERSECT 2



// my functions
static void showTreeR(tree t, int depth);
static void freeTree(tree t);
static tree AVLTreeInsert(tree t, int item, Set s);
static tree newBSTNode(int item, Set s);
static int BSTNodeHeight(tree root);
static int maximum(int a, int b);
static tree rotateLeft(tree t);
static tree rotateRight(tree t);
int TreeNumNodes(tree t);
static bool TreeSearch(tree t, int item);
static void inOrderPrintTree(tree t, int *size);
static void mergeSets(Set s1, tree t2);
//static bool treeEquals(tree t1, tree t2);
static tree findFloor(tree t, int value);
static tree findCeil(tree t, int value);
static tree insertBetween(tree t,tree new, int value );
////////////////////////////////////////////////////////////////////////
// Basic Operations

/**
 * Creates a new empty set
 */
Set SetNew(void) {
	Set s = malloc(sizeof(struct set));
	s->tree = NULL;
	s->size = 0;
	return s;
}

/**
 * Frees all memory associated with the given set
 */
void SetFree(Set s) {
	if (s->tree != NULL) {
		freeTree(s->tree);
	}
	free(s);
	return;
}

/**
 * Inserts an item into the set
 */
void SetInsert(Set s, int item) {

	s->tree = AVLTreeInsert(s->tree, item, s);
	
	// find good way to keep track of the set size
	// maybe store a variable inside the tree
	return;
}



/**
 * Returns the number of elements in the set
 */
int SetSize(Set s) {
	if (s != NULL) {
		return s->size;
	}
	return 0;
}

/**
 * Returns true if the set contains the given item, and false otherwise
 */
bool SetContains(Set s, int item) {
	return TreeSearch(s->tree, item);
}

/**
 * Prints the given set in the format
 * {elem1, elem2, elem3}
 */
void SetShow(Set s) {
	printf("Set: ");
	putchar('{');

	if (s != NULL && s->tree != NULL) {
		int size = s->size;
		inOrderPrintTree(s->tree, &size);
	}
	putchar('}');
	putchar('\n');
	// putchar('\n');
	// putchar('\n');
	showTreeR(s->tree, 0);
	// putchar('\n');
	// putchar('\n');
	// putchar('\n');
	putchar('\n');
	putchar('\n');
	printf("Inorder: ");
	tree curr = s->min;
	while(curr != NULL) {
		printf("%d, ", curr->item);
		curr = curr->next;
	}
	putchar('\n');
	return;
}

////////////////////////////////////////////////////////////////////////
// Further Operations

/**
 * Returns a new set representing the union of the two sets
 */
Set SetUnion(Set s1, Set s2) {

	Set s3 = SetNew();

	mergeSets(s3, s2->tree);
	mergeSets(s3, s1->tree);
	
	return s3;
}

/**
 * Returns a new set representing the intersection of the two sets
 */
Set SetIntersection(Set s1, Set s2) {

	//optimise with tree join?????

	Set s3 = SetNew();

	if (s1 == NULL || s2 == NULL) {
		return NULL;
	}
	tree currOne = s1->min;
	tree currTwo = s2->min;

	while(currOne != NULL && currTwo != NULL) {

		if (currOne->item == currTwo->item) {
			s3->tree = AVLTreeInsert(s3->tree, currOne->item, s3);
			currOne = currOne->next;
			currTwo = currTwo->next;
		} else if (currOne->item < currTwo->item) {
			currOne = currOne->next;
		} else {
			currTwo = currTwo->next;
		}
	}

	return s3;
}

/**
 * Returns a new set representing the set difference s1 - s2
 */

Set SetDifference(Set s1, Set s2) {

	Set s3 = SetNew();

	if (s1->tree == NULL) {
		return s3;
	}
	if (s2->tree == NULL) {
		mergeSets(s3, s1->tree);
		return s3;
	}
	tree curr = s1->min;
	while (curr != NULL) {
		if (SetContains(s2, curr->item)) {

		} else {
			s3->tree = AVLTreeInsert(s3->tree, curr->item, s3);
		}
		curr = curr->next;
	}

	return s3;

}

/**
 * Returns true if the two sets are equal, and false otherwise
 */


bool SetEquals(Set s1, Set s2) {
	//XOR if one tree is NULL and the other isn't return false
	if ((s1->tree == NULL) ^ (s2->tree == NULL )) {
		return false;
	}
	//Both empty sets 
	if (s1->tree == NULL && s2->tree == NULL) {
		return true;
	}
	if (s1->size != s2->size) {
		return false;
	}
	Set s3 = SetDifference(s1, s2);
	Set s4 = SetDifference(s2, s1);
	if (s3->tree != NULL || s4->tree != NULL) {
		SetFree(s3);
		SetFree(s4);
		return false;
	}
	SetFree(s3);
	SetFree(s4);
	return true;
}

/**
 * Returns true if set s1 is a subset of set s2, and false otherwise
 */
bool SetSubset(Set s1, Set s2) {
	Set s3 = SetDifference(s1, s2);
	if (s3->tree != NULL) {
		SetFree(s3);
		return false;
	}
	SetFree(s3);
	return true;
}

/**
 * Finds the floor of a given item in a set
 */
int SetFloor(Set s, int item) {
	tree floor = findFloor(s->tree, item);
	if (floor == NULL) {
		return UNDEFINED;
	}
	return floor->item;
}

/** 
 * Returns the ceiling of a given item in a set
 */
int SetCeiling(Set s, int item) {
	tree ceil = findCeil(s->tree, item);
	if (ceil == NULL) {
		return UNDEFINED;
	}
	return ceil->item;
}

////////////////////////////////////////////////////////////////////////
// Cursor Operations

/**
 * Creates a new cursor positioned at the smallest element of the set
 */
SetCursor SetCursorNew(Set s) {
	

	SetCursor cur = malloc(sizeof(struct cursor));
	cur->item = s->min->item;
	cur->currTree = s->min;
	return cur;
}

/**
 * Frees all memory associated with the given cursor
 */
void SetCursorFree(SetCursor cur) {
	free(cur);
}

/**
 * Returns the element at the cursor's current position, and then moves
 * the cursor to the next greatest element. If there is no next greatest
 * element, then all subsequent calls to SetCursorNext on this cursor
 * should return UNDEFINED.
 */
int SetCursorNext(SetCursor cur) {
	if (cur->currTree == NULL) {
		return UNDEFINED;
	}
	int item = cur->currTree->item;
	cur->currTree = cur->currTree->next;
	return item;
}

////////////////////////////////////////////////////////////////////////

static void freeTree(tree t) {
	//base case
	if (t->left == NULL && t->right == NULL) {
		free(t);
		return;
	}
	if (t->left != NULL) {
 		freeTree(t->left);
	}
	if (t->right != NULL) {
 		freeTree(t->right);
	}
	free(t);
	return;
}
tree AVLTreeInsert (tree t, int item, Set s) {

	if (t == NULL) {
		s->size++;
		return newBSTNode(item, s);
	} else if (item < t->item) {
		t->left = AVLTreeInsert(t->left, item, s);
	} else if (item > t->item) {
		t->right = AVLTreeInsert(t->right, item, s);
	} else { // (v == t->item)
		/* don't insert duplicates */;
		return t;
	}

	int leftHeight = BSTNodeHeight(t->left);
	int rightHeight = BSTNodeHeight(t->right);

	if (leftHeight - rightHeight > 1) {
		if (item > t->left->item) {
			t->left = rotateLeft(t->left);
		}
		t = rotateRight(t);
	} else	if (rightHeight - leftHeight > 1) {
		if (item < t->right->item) {
			t->right = rotateRight(t->right);
		} t = rotateLeft(t);
	}

	return t;
}


tree newBSTNode(int item, Set s) {
	tree new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "couldn't allocate BST node");
	}
	new->item = item;
	new->left = NULL;
	new->right = NULL;
	new->next = NULL;
	//find next()
	if (s->tree == NULL) {
		s->min = new;
		s->max = new;
		s->next = NULL;
	} else if ( s->min->item > item) {
		new->next = s->min;
		s->min = new;
	} else if (s->max->item < item) {
		s->max->next = new;
		s->max = new;
	} else {
		insertBetween(s->tree, new, item);
	}	
	
	return new;
}

int BSTNodeHeight(tree root) {
	if (root == NULL) {
		return -1;
	}
	int heightLeft = BSTNodeHeight(root->left);
	int heightRight = BSTNodeHeight(root->right);
	return 1 + maximum(heightLeft, heightRight);
}

int maximum(int a, int b) {
	return (a > b)? a : b;
}

tree rotateLeft(tree root) {
	
	if (root == NULL)  {
		return NULL;
	}
	tree newRoot = root->right;
	if (newRoot == NULL) {
		return root;
	}
	root->right = newRoot->left;
	newRoot->left = root;
	return newRoot;
}

tree rotateRight(tree root) {
	if (root == NULL)  {
		return NULL;
	}
	tree newRoot = root->left;
	if (newRoot == NULL) {
		return root;
	}
	root->left = newRoot->right;
	newRoot->right = root;
	return newRoot;
}

bool TreeSearch(tree t, int item) {
   if (t == NULL)
      return false;
   else if (item < t->item)
      return TreeSearch(t->left, item);
   else if (item > t->item)
      return TreeSearch(t->right, item);
   else                                 // it == data(t)
      return true;
}

void inOrderPrintTree(tree t, int *size) {
	if (t == NULL) {
		return;
	}

	inOrderPrintTree(t->left, size);
	printf("%d", t->item);
	if (*size != 1) {
		printf(", ");
	}
	*size -= 1;
	inOrderPrintTree(t->right, size);
}
int TreeNumNodes(tree t) {
   if (t == NULL)
      return 0;
   else
      return 1 + TreeNumNodes(t->left) + TreeNumNodes(t->right);
}

void mergeSets(Set s1, tree t2) {
	if (t2 == NULL) {
		return;
	}
	mergeSets(s1, t2->left);
	mergeSets(s1, t2->right);
	s1->tree = AVLTreeInsert(s1->tree, t2->item, s1);
	return;
}
void showTreeR(tree t, int depth) {
   if (t != NULL) {
		showTreeR(t->right, depth+1);
		int i;
		for (i = 0; i < depth; i++)
		putchar('\t');            // TAB character
		printf("%d\n", t->item);
		showTreeR(t->left, depth+1);
   }
}


tree insertBetween(tree t, tree new, int value) {

	tree floor = findFloor(t, value);
	tree ceil = findCeil(t, value);
	floor->next = new;
	new->next = ceil;
	return new;

}

tree findFloor(tree t, int value) {

    if (t == NULL) {
        return NULL;
    } 
	if (t->item == value) {
        return t;
	}
	if (t->item > value) {
        return findFloor(t->left, value);
	} else {
        tree temp = findFloor(t->right, value);
		if (temp != NULL && temp->item <= value) {
			return temp;
		}
        return t; 
    }
}

tree findCeil(tree t, int value) {
	if (t == NULL) {
		return NULL;
	} else if (t->item == value) {
		return t;
	} else if (t->item < value) {
		return findCeil(t->right, value); 
	} else {
		tree temp = findCeil(t->left, value);
		if (temp != NULL && temp->item >= value) {
			return temp;
		}
		return t; 
	}
}

// bool treeEquals(tree t1, tree t2) {
	
// 	// One tree is null and the other is not
// 	if ((t1 == NULL) ^ (t2 == NULL )) {
// 				putchar('a');
// 		return false;
// 	}
// 	// Both tree are NULL
// 	if ((t1 == NULL) && (t2 == NULL )) {
// 				putchar('b');
// 		return true;
// 	}
// 	// The items don't match
// 	if (t1->item != t2->item) {
// 		putchar('c');
// 		return false;
// 	}
// 	// Check left subtree
// 	else if (!treeEquals(t1->left, t2->left)) {
// 				putchar('d');
// 		return false;
// 	}
// 	// Check left subtree
// 	else if (!treeEquals(t1->right, t2->right)) {
// 				putchar('e');
// 		return false;
// 	}

// 	return true;
// }

