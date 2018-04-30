/* Author: Raveen Karnik
 * Date: 3-7-18
 * This program creates a binary tree and allows users to remove numbers from it */

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
    else if(parent(n)->color == RED)
    {
		//if the parent is red and its sibling is red, split the node and pop out the middle
		if(uncle(n) && uncle(n)->color == RED)
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

//removes a number from the tree
void remove(Node* root, int toRemove)
{
    Node* toRem = find(root, toRemove);
    //if the number was found
    if(toRem)
    {
		//if the number to be removed has two children
		if(toRem->left && toRem->right)
		{
			//find the next smallest number
			Node* replace = toRem->left;
			while(replace->right)
				replace = replace->right;

			//replace the node with the next smallest number
			toRem->num = replace->num;
			toRem = replace;
		}
		
		//store which side of its parent the node is on
		int side = ROOT;
		if(parent(toRem))
			side = (parent(toRem)->right == toRem) ? RIGHT : LEFT;
		
		if(toRem->color == RED)
		{
			if(side == RIGHT)
				parent(toRem)->right = NULL;
			else if(side == LEFT)
				parent(toRem)->left = NULL;
			
			delete toRem;
		}
		else if(toRem->left && toRem->left->color == RED)
		{
			
		}
		
		/*
		//if there is no left or right, make the above node point to NULL where the node was
		if(!toRem->right && !toRem->left)
		{
			if(side == ROOT)
			{
				toRem->num = 0;
				cout << "The tree is now empty.\n";
			4}
			else if(side == LEFT)
			{
				parent(toRem)->left = NULL;
				delete toRem;
			}
			else if(side == RIGHT)
			{
				parent(toRem)->right = NULL;
				delete toRem;
			}
		}
		//if the node has a left but not a right, move the left-side branch up to the node's spot
		else if(!toRem->right && toRem->left)
		{
			if(side == ROOT)
			{
				Node* temp = toRem->left;
				toRem->num = temp->num;
				toRem->left = temp->left;
				toRem->right = temp->right;
				delete temp;
			}
			else if(side == LEFT)
			{
				parent(toRem)->left = toRem->left;
				delete toRem;
			}
			else if(side == RIGHT)
			{
				parent(toRem)->right = toRem->left;
				above->right = temp->left;
		}
		//if the node has a right but not a left, move the right-side branch up to the node's spot
		else if(temp->right && !temp->left)
		{
			if(side)
				above->left = temp->right;
			else
				above->right = temp->right;

			//delete the node
			delete temp;
		}
		//if the node has both a right and a left
		else*/
    }
    //if the number wasn't found, complain
    else
    {
		cout << "That number is not in the tree.\n";
    }
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
					int toRemove = atoi(input);
					cout << endl;

					//remove the number from the tree
					//remove(tree, toRemove);
							
					//if the tree is empty, end
					if(!tree || tree->num == 0)
					{
						tree = NULL;
						break;
					}
					else
						visualize(tree);
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
