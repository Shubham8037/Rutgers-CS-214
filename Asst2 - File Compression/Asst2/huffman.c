
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include "helper.h"


nodeT * createNode(char * token, unsigned int oft) { // creates the node for huffman tree
	nodeT * ptr = (nodeT *)malloc(sizeof(nodeT));
	ptr->left = NULL;
	ptr->right = NULL;
	ptr->token = (char *)malloc(strlen(token) + 1);
	strncpy(ptr->token, token, (strlen(token)));	
	ptr->oft = oft;
	return ptr;
}


tree * createTree(unsigned int mSize) { // starts the tree
	tree * ptr = (tree *)malloc(sizeof(tree)); // allocates the space according to the required space
	ptr->size = 0;
	ptr->mSize = mSize;
	ptr->nodeArr = (nodeT **)malloc(mSize * sizeof(nodeT *));
	return ptr;
}

void nodeSwap(nodeT ** first, nodeT ** second) { //updates the node based on the heirarchy
	nodeT * temp = *first;
	*first = *second;
	*second = temp;
}


void heapify(tree * root, int index) { //heapify function from data structers
	int base = index;
	int left = 2 * index + 1;
	int right = 2 * index + 2;
	if (left < root->size && root->nodeArr[left] != NULL && root->nodeArr[base] != NULL && root->nodeArr[left]->oft < root->nodeArr[base]->oft) {
		base = left;
	}
	if (right < root->size && root->nodeArr[right] != NULL && root->nodeArr[base] != NULL && root->nodeArr[right]->oft < root->nodeArr[base]->oft) {
		base = right;
	}
	if (root->nodeArr[base] != NULL && root->nodeArr[index] != NULL && base != index) {
		nodeSwap(&root->nodeArr[base], &root->nodeArr[index]);
		heapify(root, base);
	}
	return;
}

nodeT * lastNode(tree * root) { // provides the last node from the tree
	nodeT * ptr = root->nodeArr[0];
	int i = 1;
	root->nodeArr[0] = root->nodeArr[root->size - i];
	--root->size;
	heapify(root, 0);
	return ptr;
}

void inNode(tree * root, nodeT * node) { // inserts a node to the tree
	++root->size;
	int i = root->size - 1;
	while (i > 0 && root->nodeArr[(i - 1)/2] != NULL && node->oft < root->nodeArr[(i - 1)/2]->oft) {
		root->nodeArr[i] = root->nodeArr[(i - 1) / 2];
		i = (i - 1) / 2;
	}
	root->nodeArr[i] = node;
}


void makeTree(tree * root) { // makes the tree in the correct order
	int i = 0;
	int j = root->size - 1;
	for (i = (j - 1) / 2; i >= 0; --i) {
		heapify(root, i);
	}

}


tree * enTree(unsigned int size, nodeLi * node) { //makes the entire tree with all the nodes
	tree * root = createTree(size);
	nodeLi * ptr = node;
	int i = 0;
	for (i = 0; i < size; ++i) {
		root->nodeArr[i] = createNode(ptr->token, ptr->oft);
		ptr = ptr->next;
		if (ptr == NULL) {
			break;
		}
	}
	root->size = size;
	makeTree(root);
	return root;
}


nodeT * createHuff(unsigned int size, nodeLi * node) { // creates the huffman tree using the ndoes and the root
	nodeT * parent;
	nodeT * left;
	nodeT * right;
	tree * root = enTree(size, node);
	while (root->size != 1) {
		left = lastNode(root);
		right = lastNode(root);
		int leftoft = 0, rightoft = 0;
		if (left != NULL) {
			leftoft = left->oft;
		}
		if (right != NULL) {
			rightoft = right->oft;
		}
		char temp[3] = "/\\\0";
		parent = createNode(temp, leftoft + rightoft);
		parent->left = left;
		parent->right = right;
		inNode(root, parent);
	}
	return lastNode(root);
}

void setCod(nodeT * node, unsigned short code_arr[], int parent, int fd) { // left is 0 and right is 1 for the leaves
	if (node->left != NULL) {
		code_arr[parent] = 0;
		setCod(node->left, code_arr, parent+1, fd);
	}
	if (node->right != NULL) {
		code_arr[parent] = 1;
		setCod(node->right, code_arr, parent+1, fd);
	}
	if (node->left == NULL && node->right == NULL) {
		int i = 0;
		char buf[parent];
		int count = 0;
		for (i = 0; i < parent; ++i) { // writes the code based on the binary set to leaves
			if (code_arr[i] == 1 || code_arr[i] == 0) {
				buf[i] = code_arr[i] + '0';
				++count;
			} else {
				break;
			}
		}
		write(fd, buf, count);
		write(fd, "\t", 1);
		write(fd, node->token, strlen(node->token));
		write(fd, "\n", 1);
	}
}

void huffman(int size, nodeLi * root, int fd) {
	nodeT * base = createHuff(size, root);
	short cod[size];
	setCod(base, cod, 0, fd);
}
