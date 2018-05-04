/* Author: Raveen Karnik
 * Date: 5-4-18
 * This program creates a self-balancing binary tree and allows users to remove numbers from it 
 * while still maintaining its balance. The binary tree will have at most twice the number of nodes
 * on one side as the other no matter what. */

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <fstream>

using namespace std;

const bool RED = true;
const bool BLACK = false;
const int ROOT = 0;
const int LEFT = 1;
const int RIGHT = 2;

//creates a node object with a number, a left node, and a right node
struct Node
{
    int num;
    Node* left = NULL;
    Node* right = NULL;
	Node* parent = NULL;
	bool color;
    Node(int number)
    {
		num = number;
		color = RED;
    }
};

//returns the node above a given node
Node* parent(Node* n)
{
	return n->parent;
}

//returns the other branch stemming off of a node's parent
Node* sibling(Node* n)
{
	if(parent(n))
	{
		if(parent(n)->left == n)
			return parent(n)->right;
		else
			return parent(n)->left;
	}
	return NULL;
}

//returns the parent of a node's parent
Node* grandparent(Node* n)
{
	if(parent(n))
		return parent(n)->parent;
	else
		return NULL;
}

//returns the sibling of a node's parent
Node* uncle(Node* n)
{
	if(grandparent(n))
		return sibling(parent(n));
	else
		return NULL;
}

//replaces a node with another
Node* replace(Node* old, Node* replacement)
{
	//if the node being replaced is not the root, put it where old was
	if(parent(old))
	{
		replacement->parent = parent(old);
		
		if(parent(old)->right == old)
			parent(old)->right = replacement;
		
		else
			parent(old)->left = replacement;
		
		old->parent = NULL;
		
		return old;
	}
	//if the node being replaced is the root, change the root's data so it matches the replacement
	else
	{
		old->num = replacement->num;
		old->left = replacement->left;
		old->right = replacement->right;
		if(old->left)
			old->left->parent = old;
		if(old->right)
			old->right->parent = old;
		
		return replacement;
	}
}

//returns true if a node is either colored black or if it's a leaf
bool isBlack(Node* n)
{
	if(!n)
		return true;
	else if(n->color == BLACK)
		return true;
	else
		return false;
}

//rearranges the tree by pivoting to the left at a specific node
void rotateLeft(Node* n)
{
	Node* r = n->right;
	if(r)
	{
		n->right = r->left;
		if(n->right)
			n->right->parent = n;
		r->parent = n->parent;
		if(r->parent)
		{
			if(r->parent->left == n)
				r->parent->left = r;
			else
				r->parent->right = r;
		}
		r->left = n;
		n->parent = r;
	}
}

//rearranges the tree by pivoting to the right at a specific node
void rotateRight(Node* n)
{
	Node* l = n->left;
	if(l)
	{
		n->left = l->right;
		if(n->left)
			n->left->parent = n;
		l->parent = n->parent;
		if(l->parent)
		{
			if(l->parent->left == n)
				l->parent->left = l;
			else
				l->parent->right = l;
		}
		l->right = n;
		n->parent = l;
	}
}

//print out the tree in a sideways form
void visualize(Node* root, int depth=0)
{
    if(root->right)
	visualize(root->right, depth+1);

    for(int i = 0; i < depth; i++)
    {
		cout << "\t";
    }

    //print out number in node followed by its color
    cout << root->num;
    if(root->color == RED)
    	cout << "R\n";
    else
    	cout << "B\n";

    if(root->left)
		visualize(root->left, depth+1);
}

//adds an integer to the binary tree recursively
void recursiveAdd(Node* root, Node* toAdd)
{
    //if the integer being added is greater than the current node go right
    if(toAdd->num > root->num)
    {
		//if there is already a node to the right, go right
		if(root->right)
		{
			recursiveAdd(root->right, toAdd);
		}
		//if there is not a node to the right, create a new one containing the integer
		else
		{
			root->right = toAdd;
			toAdd->parent = root;
		}
    }
    //otherwise go left
    else
    {
		//if the left node exists, go there, otherwise make a new left
		if(root->left)
		{
			recursiveAdd(root->left, toAdd);
		}
		else
		{
			root->left = toAdd;
			toAdd->parent = root;
		}
    }
}

//rearranges the red-black tree so it adheres to the rules
void repairAdd(Node* n)
{
    //if node is the root, make sure it's black
    if(!parent(n))
    {
    	n->color = BLACK;
    }
    //because all new nodes are automatically internal and you can't have four in a node,
    //check if the parent is red as well. If it is black, there's nothing to worry about
    else if(!isBlack(parent(n)))
    {
		//if the parent is red and its sibling is red, split the node and pop out the middle
		if(uncle(n) && !isBlack(uncle(n)))
		{
			uncle(n)->color = BLACK;
			parent(n)->color = BLACK;
			grandparent(n)->color = RED;
			repairAdd(grandparent(n));
		}
		//if the parent is red but its sibling is black,
		//then the node is shaped weird so rearrange it
		else
		{
			if(grandparent(n)->left && n == grandparent(n)->left->right)
			{
				rotateLeft(parent(n));
				n = n->left;
			}
			else if(grandparent(n)->right && n == grandparent(n)->right->left)
			{
				rotateRight(parent(n));
				n = n->right;
			}
				
			grandparent(n)->color = RED;
			parent(n)->color = BLACK;
			if(parent(n)->left == n)
				rotateRight(grandparent(n));
			else
				rotateLeft(grandparent(n));
		}
    }
}

//adds a node to the tree
Node* add(Node* root, int toAdd)
{
    //first add the node where it should go in a regular binary tree
    Node* n = new Node(toAdd);
    recursiveAdd(root, n);
    //now rearrange the tree to even it out
    repairAdd(n);
    
    //return the root in case it changed
    root = n;
    while(parent(root))
    	root = parent(root);
    return root;
}

//recursively finds a number in the tree, and returns the node above it
Node* find(Node* root, int toRemove)
{
    //if the number being looked for is greater than the current, go right
    if(toRemove > root->num)
    {
		if(root->right)
		{
			return find(root->right, toRemove);
		}
		//if there is no node to the right, return NULL indicating the number is not in the tree
		else
		{
			return NULL;
		}
    }
    //when the number is found, return the node above it
    else if(toRemove == root->num)
    {
		return root;
    }
    //if the number being looked for is less than the current, go left
    else if(toRemove < root->num)
    {
		if(root->left)
		{
			return find(root->left, toRemove);
		}
		//if there is no node to the left, return NULL indicating the number is not in the tree
		else 
		{
			return NULL;
		}
    }
}

//removes a node from the tree while maintaining the RBT's structure
void safeRemove(Node* n, int side)
{

	//if the node being removed is the root, nothing has to be done
	if(side != ROOT)
	{
		//n is black & it's not the root, therefore it has to have a sibling
		
		//if the sibling is red
		if(!isBlack(sibling(n)))
		{
			//switch the parent and the sibling's colors
			sibling(n)->color = BLACK;
			parent(n)->color = RED;
			
			//rotate at the parent, making the sibling the grandparent
			if(side == RIGHT)
				rotateRight(parent(n));
			else
				rotateLeft(parent(n));
			
			//now, it is ensured that n's sibling is black while the old sibling, which is where
			//the parent was, is the same color as the parent, ensuring no properties are broken
		}
		
		//now the sibling must be black
		
		//if the sibling's children are black
		if(isBlack(sibling(n)->right) && isBlack(sibling(n)->left))
		{
			//if the parent is black as well
			if(isBlack(parent(n)))
			{
				//make the sibling red to account for the deletion of n, balancing this subtree
				sibling(n)->color = RED;
				
				//although this section of the tree is now balanced, it is imbalanced with respect
				//to the rest of the tree, so recursively call the function on n's parent
				if(!parent(parent(n)))
					safeRemove(parent(n), ROOT);
				else if(parent(parent(n))->right == parent(n))
					safeRemove(parent(n), RIGHT);
				else
					safeRemove(parent(n), LEFT);
				
				return;
			}
			
			//if the parent is red
			else
			{
				//simply switch the parent and the sibling's color, adding a black to n's path
				//without changing anything on the sibling's path
				parent(n)->color = BLACK;
				sibling(n)->color = RED;
				
				return;
			}
		}
		
		//n's sibling is now guaranteed to have at least one red child
		
		//for convenience sake, we want to make sure that the outside child of n's sibling is red
		//if the inside one is red, swap its color with the sibling and rotate it out
		if(side == RIGHT && isBlack(sibling(n)->left) && !isBlack(sibling(n)->right))
		{
			sibling(n)->right->color = BLACK;
			sibling(n)->color = RED;
			rotateLeft(sibling(n));
		}
		else if(side == LEFT && isBlack(sibling(n)->right) && !isBlack(sibling(n)->left))
		{
			sibling(n)->left->color = BLACK;
			sibling(n)->color = RED;
			rotateRight(sibling(n));
		}
		
		//the outside child of n's sibling is now guaranteed to be red
		
		//swap the parent and the sibling's colors, paint the red child black, and rotate the parent
		//over to n's side, effectively adding a black to n's side without changing the count on the
		//sibling side while simultaneously ensuring the node in the parent's spot is the same color
		sibling(n)->color = parent(n)->color;
		parent(n)->color = BLACK;
		if(side == RIGHT)
		{
			sibling(n)->left->color = BLACK;
			rotateRight(parent(n));
		}
		else
		{
			sibling(n)->right->color = BLACK;
			rotateLeft(parent(n));
		}
	}
}

//removes a number from the tree
Node* remove(Node* root, int toRemove)
{
    Node* toRem = find(root, toRemove);
    //if the number was found
    if(toRem)
    {
		//if the number to be removed has two children
		if(toRem->right && toRem->left)
		{
			//find the next smallest number
			Node* replace = toRem->left;
			while(replace->right)
				replace = replace->right;

			//replace the node with the next smallest number
			toRem->num = replace->num;
			toRem = replace;
		}
		
		//toRem now has to either have 0 or 1 child

		//store which side of its parent the node is on
		int side = ROOT;
		if(parent(toRem))
			side = (parent(toRem)->right == toRem) ? RIGHT : LEFT;
		
		//if the node to be removed is red, it must either have 0 or 2 children
		//because toRem can't have 2 children, it has none, which means it can simply be deleted
		if(!isBlack(toRem))
		{
			if(side == RIGHT)
				parent(toRem)->right = NULL;
			else if(side == LEFT)
				parent(toRem)->left = NULL;
		}
		
		//toRem is therefore black
		
		//if it has a child, that child has to be red, so repaint that child to be black and replace it
		else if(toRem->right || toRem->left)
		{
			Node* child = (toRem->right) ? toRem->right : toRem->left;
			child->color = BLACK;
			toRem = replace(toRem, child);
		}
				
		//toRem is therefore black with no children, so a separate function is created for this scenario
		else
		{
			safeRemove(toRem, side);
		}
		
		//if the root is being deleted, special case
		if(!parent(toRem))
		{
			toRem->num = 0;
			cout << "The tree is now empty.\n";
		}
		//otherwise, just delete it
		else
		{
			if(side == RIGHT && parent(toRem))
				parent(toRem)->right = NULL;
			else if(side == LEFT && parent(toRem))
				parent(toRem)->left = NULL;
			
			delete toRem;
		}

    }
    //if the number wasn't found, complain
    else
    {
		cout << "That number is not in the tree.\n";
    }
	
    while(parent(root))
    	root = parent(root);
    return root;
}

int main()
{
    Node* tree = NULL;
    bool fileFound = true;
    bool running = true;

    while(running)
    {
		cout << "Would you like to INPUT numbers, SEARCH for numbers REMOVE numbers, PRINT the tree, or QUIT?\n";
		//thanks to myself for writing the input code in heap
		char* input = new char[8];
		cin >> input;
		
		if(!strcmp(input, "INPUT") || !strcmp(input, "input"))
		{
			//prompt the user for how numbers will be entered
			cout << "Would you like to ADD numbers manually or READ in from a file? \n";
			cin >> input;
			//if the user wants to enter numbers manually
			if(!strcmp(input, "ADD") || !strcmp(input, "add"))
			{
				//add numbers one by one until the user says they are done
				cout << "Enter integers from 1-1000 one at a time.\nEnter DONE when you are done adding numbers \n";
				cin >> input;
				if(isdigit(input[0]) && !tree)
				{
					tree = new Node(atoi(input));
					tree->color = BLACK;
					cin >> input;
				}
				while(strcmp(input, "DONE") && strcmp(input, "done"))
				{
					int toAdd = atoi(input);
					tree = add(tree, toAdd);
					cout << endl;
					visualize(tree);
					cout << endl;
					cin >> input;
				}
			}
			//if the user wants to enter numbers from a file
			else if(!strcmp(input, "READ") || !strcmp(input, "read"))
			{
				char* file = new char[32];
				char* list = new char[512];

				//prompt the user for the name of the file
				cout << "\nEnter the name of the file.\n";
				cin.get();
				cin.get(file, 32);
				cin.get();

				//open the file
				ifstream stream(file);
				if(stream.is_open())
				{
					fileFound = true;
					//extract the text from the file
					stream.getline(list, 512);
					stream.close();
					cout << endl << list << endl;
					//extract the numbers from the text
					for(int i = 0; i < strlen(list); i++)
					{
						if(isdigit(list[i]))
						{
							//isolate the number into a separate char pointer
							int start = i;
							char* number = new char[5];
							do number[i-start] = list[i];
							while(list[++i] != ',' && i < strlen(list));
							number[i-start] = '\0';

							//convert the isolated char pointer into an integer and add it to the tree
							if(!tree)
							{
								tree = new Node(atoi(number));
								tree->color = BLACK;
							}
							else
							{
								int toAdd = atoi(number);
								tree = add(tree, toAdd);
							}
						}
					}
				}
				else
				{
					cout << "File not found\n";
					fileFound = false;
				}
			}
			//if the user entered neither option, throw an error.
			else
			{
				cout << "Sorry I couldn't understand that.\n";
				fileFound = false;
			}
		}
		else if(!strcmp(input, "SEARCH") || !strcmp(input, "search"))
		{
			//prompt the user for the number to search for
			cout << "Enter the number you want to search the tree for.\n";
			cin >> input;
			int toSearch = atoi(input);
			cout << endl;
			
			//if the tree exists
			if(tree)
			{				
				if(find(tree, toSearch))
					cout << "That number is in the tree.\n";
				else
					cout << "That number is not in the tree.\n";
			}
			else
			{
				cout << "The tree is empty.\n";
			}
		}
		else if(!strcmp(input, "REMOVE") || !strcmp(input, "remove"))
		{
			//if the tree exists
			if(fileFound)
			{
				//print it out
				visualize(tree);

				//if the user wants to delete, delete
				while(strcmp(input, "DONE") && strcmp(input, "done"))
				{
					//ask the user for the numbers to delete
					cout << "Enter numbers to remove one at a time.\nEnter DONE when you are done removing numbers.\n";
					cin >> input;
					if(strcmp(input, "DONE") && strcmp(input, "done"))
					{
						int toRemove = atoi(input);
						cout << endl;

						//remove the number from the tree
						tree = remove(tree, toRemove);
								
						//if the tree is empty, end
						if(!tree || tree->num == 0)
						{
							delete tree;
							tree = NULL;
							break;
						}
						else
							visualize(tree);
					}
				}
			}
		}
		else if(!strcmp(input, "PRINT") || !strcmp(input, "print"))
		{
			visualize(tree);
		}
		else if(!strcmp(input, "QUIT") || !strcmp(input, "quit"))
		{
			running = false;
		}
    }
    return 0;
}
