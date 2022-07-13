// Implementation of the Set ADT using a balanced BST
// COMP2521 22T2 Assignment 1

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Set.h"
#include "SetStructs.h"


// my functions               
static void freeTree(tree t);							 
static tree AVLTreeInsert(tree t, int item, Set s);		
static tree newBSTNode(int item, Set s);
static int BSTNodeHeight(tree root);
static int maximum(int a, int b);
static tree rotateLeft(tree t);
static tree rotateRight(tree t);
int TreeNumNodes(tree t);
static bool TreeSearch(tree t, int item);
static void inOrderPrintTree(tree t, Set s);
static void mergeSets(Set s1, tree t2);
int findBalance(tree root);
static tree findFloor(tree t, int value);
static tree findCeil(tree t, int value);
static tree insertBetween(tree t,tree new, int value );



////////////////////////////////////////////////////////////////////////
// Basic Operations

/**
 * Creates a new empty set
 */
Set SetNew(void) {

	// Assign memory and set default values
	Set s = malloc(sizeof(struct set));
	s->tree = NULL;
	s->size = 0;
	return s;

}

/**
 * Frees all memory associated with the given set
 */
void SetFree(Set s) {

	// free the nodes of the set first
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

	if (s == NULL) {
		return;
	}
	// Use a AVL tree to insert he node to maintain a Olog(n) timecomplexity
	s->tree = AVLTreeInsert(s->tree, item, s);
	
}



/**
 * Returns the number of elements in the set
 */
int SetSize(Set s) {

	if (s != NULL) {

		// Set size is stored a variable inside the set
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

	// check that the set is not empty
	if (s != NULL && s->tree != NULL) {
		inOrderPrintTree(s->tree, s);
	}

	putchar('}');

	return;
}

////////////////////////////////////////////////////////////////////////
// Further Operations

/**
 * Returns a new set representing the union of the two sets 
 */
Set SetUnion(Set s1, Set s2) {

	Set s3 = SetNew();

	// MergeSets() inserts a given tree into another set by calling the AVLTreeInsert  
	// AVLTreeInsert can handle repeated elements and NULL trees
	mergeSets(s3, s2->tree);
	mergeSets(s3, s1->tree);
	
	return s3;
}

/**
 * Returns a new set representing the intersection of the two sets 
 * uses the cursor implmentation for simplicity
 */
Set SetIntersection(Set s1, Set s2) {

	Set s3 = SetNew();

	// if either Set is empty there cannot be any intersect
	if (s1 == NULL || s2 == NULL) {
		return NULL;
	}

	// create a pointer to the start of each set using 
	tree currOne = s1->min;
	while (currOne != NULL) {

		// search the other set for the item and insert it if found
		if (SetContains(s2, currOne->item)) {
			s3->tree = AVLTreeInsert(s3->tree, currOne->item, s3);
		}
		currOne = currOne->next;
	}
	return s3;
}

/**
 * Returns a new set representing the set difference s1 - s2
 */

Set SetDifference(Set s1, Set s2) {

	Set s3 = SetNew();

	// if Set one is then the difference must be NULL
	if (s1->tree == NULL) {
		return s3;
	}

	// if Set two is NULL add all the elements of Set one
	if (s2->tree == NULL) {
		mergeSets(s3, s1->tree);
		return s3;
	}

	// create a pointer to the start of set One
	tree curr = s1->min;
	while (curr != NULL) {

		// Only add elements to the return set if they are not in set 2
		if  (!SetContains(s2, curr->item)) {
			s3->tree = AVLTreeInsert(s3->tree, curr->item, s3);
		}
		// increment
		curr = curr->next;
	}

	return s3;

}

/**
 * Returns true if the two sets are equal, and false otherwise 
 */

bool SetEquals(Set s1, Set s2) {

	// check using an XOR if one tree is NULL and the other isn't
	if ( (s1->tree == NULL) ^ (s2->tree == NULL ) ) {
		return false;
	}

	// Check if both sets are emtpy 
	if (s1->tree == NULL && s2->tree == NULL) {
		return true;
	}

	// Check if the sizes match
	if (s1->size != s2->size) {
		return false;
	}

	// Use Set difference generate a lists of items that are in one set but not the other
	Set s3 = SetDifference(s1, s2);
	Set s4 = SetDifference(s2, s1);

	// if either of the temporary sets are not empty then the Set One and Two cannot be equal
	if (s3->tree != NULL || s4->tree != NULL) {
		// free the temporary sets
		SetFree(s3);
		SetFree(s4);
		return false;
	}
	// free the temporary sets
	SetFree(s3);
	SetFree(s4);

	// Sets must be equal
	return true;
}

/**
 * Returns true if set s1 is a subset of set s2, and false otherwise
 */

bool SetSubset(Set s1, Set s2) {

	// Use Set difference generate a lists of items that are in one set but not the other
	Set s3 = SetDifference(s1, s2);

	// if the temporary set is not empty then set s1 contains elements not in set 2, thus not a subset
	if (s3->tree != NULL) {

	// free the temporary set
		SetFree(s3);
		return false;
	}
	// free the temporary sets
	SetFree(s3);
	
	// set must be a subset
	return true;
}

/**
 * Finds the floor of a given item in a set
 */
int SetFloor(Set s, int item) {

	// calls a helper function that returns a pointer to the floor node
	tree floor = findFloor(s->tree, item);

	// if the floor could not be found return undefined
	if (floor == NULL) {
		return UNDEFINED;
	}

	// return the integer value of the found floor tree
	return floor->item;
}

/** 
 * Returns the ceiling of a given item in a set
 */
int SetCeiling(Set s, int item) {

	// calls a helper function that returns a pointer to the celing node
	tree ceil = findCeil(s->tree, item);

	// if the ceiling could not be found return undefined
	if (ceil == NULL) {
		return UNDEFINED;
	}

	// return the integer value of the found ceiling tree
	return ceil->item;
}

////////////////////////////////////////////////////////////////////////
// Cursor Operations

/**
 * Creates a new cursor positioned at the smallest element of the set
 */
SetCursor SetCursorNew(Set s) {
	
	// create a new Setcursor and assign memory
	SetCursor cur = malloc(sizeof(struct cursor));

	// within the new setcursor pass the smallest node to its current Node
	cur->currTree = s->min;

	// set the value of the item of the new setcursor to be the smallest node in the set
	cur->item = s->min->item;

	return cur;
}

/**
 * Frees all memory associated with the given cursor
 */
void SetCursorFree(SetCursor cur) {
	
	// Setcursor only has an item value and twos pointers so it can be free instantly
	free(cur);

}

/**
 * Returns the element at the cursor's current position, and then moves
 * the cursor to the next greatest element. If there is no next greatest
 * element, then all subsequent calls to SetCursorNext on this cursor
 * should return UNDEFINED.
 */
int SetCursorNext(SetCursor cur) {

	// Check if the cursor has reached the end of the set
	if (cur->currTree == NULL) {
		return UNDEFINED;
	}

	// Assign the item of the Setcursor as the item from the Node is it currently on
	int item = cur->currTree->item;

	// Travel to the next node in order
	// **NOTE: the 'next' is a set up in AVLTreeInsert **
	cur->currTree = cur->currTree->next;

	return item;

}

////////////////////////////////////////////////////////////////////////

/*
	Post-order traversal to free each node in a given tree - O(n)
*/
static void freeTree(tree t) {

	// base case
	if (t->left == NULL && t->right == NULL) {
		free(t);
		return;
	}
	// traverse left
	if (t->left != NULL) {
 		freeTree(t->left);
	}
	// traverse right
	if (t->right != NULL) {
 		freeTree(t->right);
	}
	// free current node
	free(t);

}

/*
* inserts items into a AVL tree. AVL trees are height self balancing
* thus have O(log(n)) for their operations. To achieve this, it rotates when
* the absolute height balance is between two subtrees is greater than 1
* insertion and rotation are both O(log(n)) operations and calulating the height is O(1)
*/

tree AVLTreeInsert (tree t, int item, Set s) {

	// base case or empty tree
	if (t == NULL) {
		s->size++;
		t = newBSTNode(item, s);

		// insert left
	} else if (item < t->item) {
		t->left = AVLTreeInsert(t->left, item, s);

		// insert right
	} else if (item > t->item) {
		t->right = AVLTreeInsert(t->right, item, s);

		// element is a duplicate thus don't insert it
	} else { // (v == t->item)

		return t;
	}

	// Once the node has been inserted calulcate the current balance of subtree
	int balance = findBalance(t);
	
	// tree is unbalanced on the left
	if (balance > 1) {
		// check the node below as it might need to rotate too
		if (item > t->left->item) {
			t->left = rotateLeft(t->left);
		}
		t = rotateRight(t);

	// tree is unbalanced on the right
	} else	if (balance < -1) {
		if (item < t->right->item) {
			// check the node below as it might need to rotate too
			t->right = rotateRight(t->right);
		} t = rotateLeft(t);
	}
	// Assign a height to the newly added node
	t->height = BSTNodeHeight(t);
	return t;
}

/*
* Creates a new node in the tree with a given item
* updates the set's minimum and/or maximum values if the node qualifies
* assigns a inorder next pointer for the node using the floor and ceil functions - O(log(n))
*/
tree newBSTNode(int item, Set s) {

	// allocate memory for the new node
	tree new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "couldn't allocate BST node");
	}
	// Assign default values
	new->item = item;
	new->left = NULL;
	new->right = NULL;
	new->next = NULL;

	// empty tree the new node will be the max and min
	if (s->tree == NULL) {
		s->min = new;
		s->max = new;
		s->next = NULL;

		// new node becomes the minimum in the set
	} else if ( s->min->item > item) {
		new->next = s->min;
		s->min = new;
		// new node becomes the maximum in the set
	} else if (s->max->item < item) {
		s->max->next = new;
		s->max = new;

		// new node is neither the minimum nor maximum in the set
	} else {

		// find what the new node's next pointer will be
		insertBetween(s->tree, new, item);
	}
	return new;
}



/*
* Calculates the height of a given tree by finding the maximum height
* between the left and right subtrees - O(1)
*/
int BSTNodeHeight(tree root) {

	// an empty tree has a height of -1
	if (root == NULL) {
		return -1;
	}

	int leftHeight, rightHeight;

	// Either the left tree is empty or already has a height
	if (root->left == NULL) {
		leftHeight = -1;
	} else {
		leftHeight = root->left->height;
	}

	// Either the right tree is empty or already has a height
	if (root->right == NULL) {
		rightHeight = -1;
	} else {
		rightHeight = root->right->height;
	}

	// the height of the parent tree will be one larger than its biggest subtree
	return 1 + maximum(leftHeight, rightHeight);
}

/*
* helper funcion that returns the maximum value of two given integers 
*/
int maximum(int a, int b) {
	return (a > b)? a : b;
}
/*
* calculates the height balance of a given root by finding the height difference 
* between the left and right subtrees - O(1)
*/
int findBalance(tree root) {

	// empty tree
	if (root == NULL ) {
		return 0;
	}

	int leftHeight, rightHeight;
	
	// Either the left tree is empty or already has a height
	if (root->left == NULL) {
		leftHeight = -1;
	} else {
		leftHeight = 1 + root->left->height;
 	}

	// Either the left tree is empty or already has a height
	if(root->right == NULL ) {
		rightHeight = -1;
	} else {
		rightHeight = 1 + root->right->height;
	}

	// balance is calcluated by finding the difference of the trees
	return leftHeight - rightHeight;

}

/*
* rotate left tree around the root
* update the heights - O(1)
*/
tree rotateLeft(tree root) {
	
	// base case
	if (root == NULL)  {
		return NULL;
	}

	tree newRoot = root->right;
	// check there is a right subtree
	if (newRoot == NULL) {
		return root;
	}

	root->right = newRoot->left;
	newRoot->left = root;

	// since the nodes have changed update their heights
	root->height = BSTNodeHeight(root);
	newRoot->height = BSTNodeHeight(newRoot);
	return newRoot;
}





/*
* rotate right tree around the root
* update the heights - O(1)
*/
tree rotateRight(tree root) {
		// base case
	if (root == NULL)  {
		return NULL;
	}

	tree newRoot = root->left;
	// check there is a left sub tree
	if (newRoot == NULL) {
		return root;
	}
	root->left = newRoot->right;
	newRoot->right = root;
	
	// since the nodes have changed update their heights
	root->height = BSTNodeHeight(root);
	newRoot->height = BSTNodeHeight(newRoot);

	return newRoot;
}



/*
* searches a tree and returns true or false if the tree contains an input integer - O(log(n))
*/
bool TreeSearch(tree t, int item) {

	// base case - item not found
	if (t == NULL)
		return false;

		// search left
	else if (item < t->item)
		return TreeSearch(t->left, item);
		
		// search right
	else if (item > t->item)
		return TreeSearch(t->right, item);
	else
		// item and tree value must be equal  
		return true;
}
/*
* Prints the values of a tree using a recursive inorder traversal - O(n)
*/
void inOrderPrintTree(tree t, Set s) {

	// base case
	if (t == NULL) {
		return;
	}
	// recursive call left
	inOrderPrintTree(t->left, s);
	printf("%d", t->item);
	// does not print a ', ' on the largest value 
	if (t->item != s->max->item) {
		printf(", ");
	}
	// recursive call right
	inOrderPrintTree(t->right, s);
}

/*
* Recursively counts the number of nodes in a tree - O(n)
*/
int TreeNumNodes(tree t) {
	if (t == NULL)
		return 0;
	else
		return 1 + TreeNumNodes(t->left) + TreeNumNodes(t->right);
}

/*
* Adds all the elements from given tree into a given set using a post order traversal - O(n)
*/
void mergeSets(Set s1, tree t2) {
	// base case
	if (t2 == NULL) {
		return;
	}
	// add the left tree's elements
	mergeSets(s1, t2->left);
	// add the right tree's elements
	mergeSets(s1, t2->right);
	// add the current element
	s1->tree = AVLTreeInsert(s1->tree, t2->item, s1);
	return;
}

/*
* Using an input value and tree recursively finds the floor node - O(log(n))
*/
tree findFloor(tree t, int value) {

	// base case
    if (t == NULL) {
        return NULL;
    }

	// elements are equal - stop searching
	if (t->item == value) {
        return t;
	}
	// search left
	if (t->item > value) {
        return findFloor(t->left, value);
	} else {
		// continue searching right
        tree rightTree = findFloor(t->right, value);

		// rightTree is the floor
		if (rightTree != NULL && rightTree->item <= value) {
			return rightTree;
		}
		// current tree is the floor or the floor was not found 
        return t; 
    }
}
/*
* Using an input value and tree recursively finds the ceiling node - O(log(n))
*/
tree findCeil(tree t, int value) {
	// base case
	if (t == NULL) {
		return NULL;
	}
	
	// elements are equal - stop searching
	if (t->item == value) {
		return t;
	} 
	
	// search right
	if (t->item < value) {
		return findCeil(t->right, value); 
	} else {
		// continue searching left
		tree leftTree = findCeil(t->left, value);
		// leftTree is the ceiling
		if (leftTree != NULL && leftTree->item >= value) {
			return leftTree;
		}

		// current tree is the ceiling or the ceiling was not found 
		return t; 
	}
}

/*
* Given an existing tree, new node and item, find the items ordered place in the tree
* use the floor and ceiling functions to determine the location - O(log(n))
*/
tree insertBetween(tree t, tree new, int value) {
	// determine positon
	tree floor = findFloor(t, value);
	tree ceil = findCeil(t, value);
	// change the existing trees next pointers
	floor->next = new;
	// update the new nodes next
	new->next = ceil;
	return new;

}