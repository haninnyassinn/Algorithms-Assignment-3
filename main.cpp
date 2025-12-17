// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

// --- 1. PlayerTable (Double Hashing) ---

class ConcretePlayerTable : public PlayerTable {
    // TODO: Define your data structures here
private:
    int* playerIDs;      // Array for player IDs
    string* playerNames; // Array for names
    bool* usedSlots;     // Array to mark if slot is used

    static const int TableSize = 101; //the array size
    static const int primeNum = 97; //that is a prime number less than the table size
    //Multiplication Method
    int hash1(int key) const {

        const double A = 0.61803398875;//used golden ratio
        double frac = (key * A) - (int)(key * A);   // fractional part which is (key⋅A)%1)
        return (int)(TableSize * frac); //return the place
    }
    //used in rule for double hashing
    int hash2(int key) const {
        return primeNum - (key % primeNum);
    }



public:
    //constructor
    ConcretePlayerTable() {
        playerIDs = new int[TableSize];
        playerNames = new string[TableSize];
        usedSlots = new bool[TableSize];

        // initialize all as empty
        for (int i = 0; i < TableSize; i++) {
            playerIDs[i] = -1;
            usedSlots[i] = false;
        }
    }
    //insert function
    void insert(int playerID, string name) override {
        // TODO: Implement double hashing insert
        int h1 = hash1(playerID); //using the hash1 function to get the place
        int h2 = hash2(playerID); //using the hash 2 function

        for (int i = 0; i < TableSize; i++) {
            int idx = (h1 + i * h2) % TableSize; // the double hashing part

            // If slot empty or updating same player using the same id
            if (usedSlots[idx] == false || playerIDs[idx] == playerID) {
                playerIDs[idx] = playerID;//set the id
                playerNames[idx] = name; // set the name or update it
                usedSlots[idx] = true;//set the place to true meaning its used
                return;
            }
        }
        cout << "Error: Table is full" << endl;
    }
    //search function
    string search(int playerID) override {
        // TODO: Implement double hashing search
        // Return "" if player not found
        int h1 = hash1(playerID); //getting h1 for the double hashing equation
        int h2 = hash2(playerID); //getting h2 for the double hashing equation
        for (int i = 0; i < TableSize; i++) {
            int idx = (h1 + i * h2) % TableSize; //getting the index the player suppose to be in

            if (!usedSlots[idx]) {
                break; // empty slot so no player found
            }

            if (playerIDs[idx] == playerID) {
                return playerNames[idx]; // found
            }
        }

        return ""; // not found
    }
    //destructor
    ~ConcretePlayerTable() {
        delete[] playerIDs;
        delete[] playerNames;
        delete[] usedSlots;
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

    struct Node {
        int playerID; // this is player id
        int score; // this is the player score
        vector<Node*> next;  // pointer to next node in the same list

        Node(int id, int s, int list_level) :
            playerID(id), score(s), next(list_level + 1, nullptr) {
        }
    };
    static const int maxNumberOfLists = 16;
    Node* start;     // dummy head node
    int highestList;  // current highest level will start with 0

    // Random level generator - 50% chance for each additional level
    int randomLevel() {
        int list_level = 0; // all get list 0
        // that is 50% chance to be in other lists or not
        while ((rand() % 2) == 0 && list_level < maxNumberOfLists - 1) {
            list_level++;
        }
        return list_level;
    }

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
        start = new Node(-1, INT_MIN, maxNumberOfLists - 1); // make the player id -1 and the score to min value
        highestList = 0;

    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
        //first remove player if it exists before
        removePlayer(playerID);

        // use the function to get how many lists the player will get
        int listsNumber = randomLevel();

        //give the player a new node
        Node* newNode = new Node(playerID, score, listsNumber);

        // check if the player got more than the 0 list if so update it
        if (listsNumber > highestList) {
            highestList = listsNumber;
        }

        // Find where to insert at each level
        Node* current = start;
        vector<Node*> placeToInsert(maxNumberOfLists + 1, nullptr);


        for (int list = highestList; list >= 0; list--) {
            // go to next slot while next exists means it isnt nullptr and has higher score and if equal then go with older id
            while (current->next[list] != nullptr &&
                (current->next[list]->score > score ||
                    (current->next[list]->score == score &&
                        current->next[list]->playerID < playerID))) {
                current = current->next[list];
            }
            placeToInsert[list] = current;  // this will tell us where it should be inserted in each list
        }

        // Insert the node at all levels up to its height
        for (int list = 0; list <= listsNumber; list++) {
            newNode->next[list] = placeToInsert[list]->next[list];//making it point to the next node
            placeToInsert[list]->next[list] = newNode;//making the one before it have the index of it
        }

    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
        vector<Node*> lastBefore(maxNumberOfLists + 1, nullptr);// Store last slot before the one to delete
        Node* current = start;// Start from dummy

        // find the place for it to delete
        for (int list = highestList; list >= 0; list--) {
            /// Move while next exists and it isn't our player id
            while (current->next[list] != nullptr &&
                current->next[list]->playerID != playerID) {
                //move if its real not dummy or like end
                current = current->next[list];
            }
            lastBefore[list] = current;// this will save the slot before the player we are suppose to delete

        }

        //  getting the node of the to be deleted from list 0
        Node* toDelete = lastBefore[0]->next[0];

        // deleting the player
        if (toDelete != nullptr && toDelete->playerID == playerID) {
            // remove from all lists it is in
            for (int list = 0; list <= highestList; list++) {
                // if it isnt find in higher lists break
                if (lastBefore[list]->next[list] != toDelete) {
                    break;
                }

                //skip over deleted player
                lastBefore[list]->next[list] = toDelete->next[list];
            }

            // delete the node
            delete toDelete;

            //  update highest list if top lists is now empty so it will be 0 or 1
            while (highestList > 0 && start->next[highestList] == nullptr) {
                highestList--;
            }
        }
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        vector<int> topPlayers;

        // start of the player after the dummy
        Node* current = start->next[0];

        // looping through list 0 as it has all players in right order
        while (current != nullptr && n > 0) {
            // add player id to the topPlayers vector
            topPlayers.push_back(current->playerID);
            // Move to next player in list 0
            current = current->next[0];
            // decrease n by 1
            n--;
        }
        //return the topPlayers
        return topPlayers;
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers
    struct RBNode {
        int itemID;
        int price;
        bool isRed;      // will return true if red will return false if black
        RBNode* left;
        RBNode* right;
        RBNode* parent;

        RBNode(int id, int p) {
            itemID = id;
            price = p;
            isRed = true; // all new leafs will be red at the start
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
    };

    RBNode* root;
    RBNode* nil;  // leaf node for tree

    // this is a helper function that rotate x to left side
    //what happens is is the right child takes place of the parent and the parent will be the child
    void leftRotate(RBNode* Node) {
        RBNode* rightChildOfNode = Node->right; //rightChildOfNode will be the node-right (the rightChildOfNode will be the new parent)
        Node->right = rightChildOfNode->left; // move rightChildOfNode left child to node right
        //if there is a child on the left side of the rightChildOfNode then make the node its new parent
        if (rightChildOfNode->left != nil) {
            rightChildOfNode->left->parent = Node; // connect the left child of rightChildOfNode with the parent which is node
        }
        //rightChildOfNode parent is now the node parent
        rightChildOfNode->parent = Node->parent;
        //update node parent to look at the rightChildOfNode-node not node
        if (Node->parent == nullptr) {
            root = rightChildOfNode; // if node is the root then rightChildOfNode will be the root
        }
        else if (Node == Node->parent->left) {
            Node->parent->left = rightChildOfNode; //here node was left child so rightChildOfNode will be left child
        }
        else {
            Node->parent->right = rightChildOfNode; //hete node was right child so rightChildOfNode will be right child
        }

        rightChildOfNode->left = Node; //node will be rightChildOfNode-node child on the left side
        Node->parent = rightChildOfNode;//update node parent to rightChildOfNode-node
    }
    // this is a helper function that rotate x into right side
    //what happens is the left child takes place of the parent and the parent will be the child right
    void rightRotate(RBNode* Node) {
        RBNode* leftChildOfNode = Node->left;//leftChildOfNode will be the node-left child
        Node->left = leftChildOfNode->right;//move leftChildOfNode right child into node-left so it become node left child
        //if there is a child on the right side of leftChildOfNode then make node its new parent
        if (leftChildOfNode->right != nil) {
            leftChildOfNode->right->parent = Node;//connecting the right child of leftChildOfNode to the node so it became the parent
        }

        leftChildOfNode->parent = Node->parent; //now the parent of leftChildOfNode is the node parent
        //update node parent to point at the leftChildOfNode-node not node
        if (Node->parent == nullptr) {
            root = leftChildOfNode; //if it was root so make leftChildOfNode root
        }
        else if (Node == Node->parent->right) {
            Node->parent->right = leftChildOfNode;//if it was the child in the right side them make leftChildOfNode the child in the right side
        }
        else {
            Node->parent->left = leftChildOfNode;//if it was the child in the left side them make leftChildOfNode the child in the left side
        }

        leftChildOfNode->right = Node;//node will be leftChildOfNode-node child on right side
        Node->parent = leftChildOfNode;//update node parent to leftChildOfNode-node
    }
    //helper function to fex insert
    //double red problem
    //when we insert a new node its red
    void fixInsert(RBNode* Node) {
        //while there is a parent to the node it isnt root and the parent colour is red we have cases to fix
        while (Node->parent != nullptr && Node->parent->isRed == true) {
            //check if parent is the left child of the grandparent
            RBNode* GrandParent = Node->parent->parent;
            if (Node->parent == GrandParent->left) {
                RBNode* uncle = GrandParent->right; //getting the uncle to check its colour
                // the first case is the uncle colour
                if (uncle->isRed == true) {
                    //change both parent and uncle to black colour
                    Node->parent->isRed = false;
                    uncle->isRed = false;
                    //change the colour of grandparent to red
                    GrandParent->isRed = true;
                    Node = GrandParent;//move to grandparent to check there is no new violations
                }
                else {
                    // the second case is the node is the right child of the parent
                    if (Node == Node->parent->right) {
                        Node = Node->parent;
                        leftRotate(Node);//left rotate the parent of the node
                    }
                    // the third case is the node is the left child of the parent
                    Node->parent->isRed = false;//recolour parent to black
                    GrandParent->isRed = true;//recolour grandparent to red
                    rightRotate(GrandParent);//right rotate the grandparent of node
                }
            }
            else {
                // parent is the right child of the grandparent
                RBNode* uncle = GrandParent->left;  // getting the uncle from the left side
                //first case uncle is red
                if (uncle->isRed == true) {
                    // recolour the parent and uncle to black
                    Node->parent->isRed = false;
                    uncle->isRed = false;
                    //reclour grandparent to red
                    GrandParent->isRed = true;
                    Node = GrandParent;//move up to check there no other violations
                }
                else {
                    //second case node is left child of the parent
                    if (Node == Node->parent->left) {
                        Node = Node->parent;
                        rightRotate(Node);//right rotate the parent
                    }
                    // third case node is right child of the parent
                    Node->parent->isRed = false;
                    GrandParent->isRed = true;
                    leftRotate(GrandParent);//left rotate the grandparent of the node
                }
            }
        }
        root->isRed = false;  //check that the root always stays black
    }
    // this is a helper function that replaces two nodes used for deleting
    void replaceNode(RBNode* firstNode, RBNode* secondNode) {

        //if the firstNode is the root then the sedcondNode will be the root
        if (firstNode->parent == nullptr) {
            root = secondNode;
        }
        else if (firstNode == firstNode->parent->left) {
            firstNode->parent->left = secondNode; //the firstNode is the left child then the secondNode will be the left child
        }
        else {
            firstNode->parent->right = secondNode; //the firstNode is the right child then the secondNode will be the right child
        }
        secondNode->parent = firstNode->parent; // update the parent to look at the secondNde
    }
    //this is a helper function that we find min by it,used for deleting
    RBNode* findMin(RBNode* node) {
        while (node->left != nil) { //go to left side until we find nil
            node = node->left;
        }
        return node; //return node this will be the smallest node
    }

    // this is a helper function that fixes the tree after double black problem happens
    void fixDelete(RBNode* Node) {
        while (Node != root && Node->isRed == false) {
            //case 1 Node is the left child of parent
            if (Node == Node->parent->left) {
                RBNode* sibling = Node->parent->right;

                // case 1 sibling is red
                if (sibling->isRed == true) {
                    sibling->isRed = false; //make the sibling colour to black
                    Node->parent->isRed = true; //make the parent colour to red
                    leftRotate(Node->parent); //rotate the parent to left
                    sibling = Node->parent->right;//get new sibling after rotation to make sure there is no other violations
                }

                //case 2 both the sibling children are black
                if (sibling->left->isRed == false && sibling->right->isRed == false) {
                    sibling->isRed = true; //make sibling red
                    Node = Node->parent; //move up to parent
                }
                else {
                    // case 3 sibling right child is black and the other is red
                    if (sibling->right->isRed == false) {
                        sibling->left->isRed = false; //make the left child black
                        sibling->isRed = true;//sibling colour will be red
                        rightRotate(sibling);// rotate sibling to right
                        sibling = Node->parent->right;//get the new sibling to make sure there is no other violations
                    }
                    // case 4 sibling right child is red
                    sibling->isRed = Node->parent->isRed; //sibling takes the parent child
                    Node->parent->isRed = false;//parent clour is black
                    sibling->right->isRed = false;//sibling right child is black
                    leftRotate(Node->parent);//rotate parent to left
                    Node = root; //node becames root we exit the loop
                }
            }
            else {

                RBNode* sibling = Node->parent->left; //sibling is the left side of the parent
                //sibling is red
                if (sibling->isRed == true) {
                    sibling->isRed = false; //make the sibling black
                    Node->parent->isRed = true;//the parent red
                    rightRotate(Node->parent);//right rotate the parent
                    sibling = Node->parent->left;//get the new sibling
                }
                //both sibling children are black
                if (sibling->right->isRed == false && sibling->left->isRed == false) {
                    sibling->isRed = true; //make sibling red
                    Node = Node->parent;//move up to parent
                }
                else {
                    //sibling left child is black the right side is red
                    if (sibling->left->isRed == false) {
                        sibling->right->isRed = false;//make the right child black
                        sibling->isRed = true; //make the sibling red
                        leftRotate(sibling);//left rotate the sibling
                        sibling = Node->parent->left;//get new sibling
                    }
                    //sibling left child is red
                    sibling->isRed = Node->parent->isRed;//sibling takes parent colour
                    Node->parent->isRed = false; //make parent black
                    sibling->left->isRed = false;//make the left child of sibling black
                    rightRotate(Node->parent);//right rotate on the parent
                    Node = root;//node is root we exit the root
                }
            }
        }
        Node->isRed = false; //set node to black
    }
    // this is a helper function to find node by the item id
    RBNode* findNodeByIDHelper(RBNode* node, int itemID) {
        if (node == nil) return nullptr; // empty

        if (node->itemID == itemID) return node; //check current node

        RBNode* leftResult = findNodeByIDHelper(node->left, itemID); //go left
        if (leftResult != nullptr) return leftResult;

        return findNodeByIDHelper(node->right, itemID);//go right
    }
    //this is to find the node by id just by searching with id
    RBNode* findNodeByID(int itemID) {
        return findNodeByIDHelper(root, itemID); //returns the findNodeByIDHelper start from root
    }
   /* void printInOrder(RBNode* node) {
        if (node == nil) return;

        printInOrder(node->left);

        cout << "ItemID: " << node->itemID
            << " | Price: " << node->price
            << " | Color: " << (node->isRed == true ? "RED" : "BLACK")
            << endl;

        printInOrder(node->right);
    }
    void printTree() {
        cout << "the red black tree in order from smallest price to biggest: "<<endl;
        printInOrder(root);
        cout <<endl;
    }*/

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
        //create nil
        nil = new RBNode(-1, -1);
        nil->isRed = false; // the colour of nil is always black
        nil->left = nil;//it points to itself
        nil->right = nil;//it points to itself
        nil->parent = nullptr;//has no parent yet
        root = nil; //empty tree it starts with nil(empty)
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
        // create a new node and set its colour to red
        RBNode* newNode = new RBNode(itemID, price);

        // search where to insert it
        RBNode* parent = nullptr; //track to where we will insert
        RBNode* currentNode = root;//start at root and move down
        //while the current isnt nil
        while (currentNode != nil) {
            parent = currentNode;
            if (price < currentNode->price || (price == currentNode->price && itemID < currentNode->itemID)) { //compare prices to see if it will be left side or right
                currentNode = currentNode->left; //go left if the price is smaller than current
            }
            else {
                currentNode = currentNode->right; //go right if price is larger than current
            }
        }

        // set the newNode to parent node
        newNode->parent = parent;
        //if there is no parent then the newnode is the root node
        if (parent == nullptr) {
            root = newNode;
        }
        else if (price < parent->price || (price == parent->price && itemID < parent->itemID)) {
            parent->left = newNode;//if the price is less than the parent insert the new node as the left child of the parent
        }
        else {
            parent->right = newNode;//if the price is higher than the parent insert the new node as the right child of the parent
        }
        // make the newNode children to nil
        newNode->left = nil;
        newNode->right = nil;
        // fix any problem
        fixInsert(newNode);
        //cout << "the item id is : " << newNode->itemID << " the price for it is : " << newNode->price << " inserted correctly" << endl;
        //printTree();
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
        // find the node to delete
        RBNode* nodeToBeDeleted = findNodeByID(itemID);
        if (nodeToBeDeleted == nullptr || nodeToBeDeleted == nil) return; //nothing to delete

        RBNode* minValueInRight = nodeToBeDeleted; //this will take the position of the nodeToBeDeleted
        RBNode* replacementNode;
        //to know if the node we are deleting is black or not ,to use fixDelte or not
        bool wasNodeBlack = (minValueInRight->isRed == false);
        //case 1 is node has no left child
        if (nodeToBeDeleted->left == nil) {
            //replace will be the node to the right
            replacementNode = nodeToBeDeleted->right;
            replaceNode(nodeToBeDeleted, nodeToBeDeleted->right); // replace the node that is to be deleted with its right child which is nill
        }
        else if (nodeToBeDeleted->right == nil) { //case 2 is node have no right child
            replacementNode = nodeToBeDeleted->left; //replace will be node to the left
            replaceNode(nodeToBeDeleted, nodeToBeDeleted->left); //replace the node to be deleted with the node to left child which is nill
        }
        else { //has two children
            //we will find the min value in the right tree
            //this will node will get removed from its position
            minValueInRight = findMin(nodeToBeDeleted->right);// getting the minValue in right sub tree
            wasNodeBlack = (minValueInRight->isRed == false);//will give true if it is black
            replacementNode = minValueInRight->right; //this will be nil

            if (minValueInRight->parent == nodeToBeDeleted) {
                replacementNode->parent = minValueInRight; //is the min value a direct child
            }
            else {
                replaceNode(minValueInRight, minValueInRight->right);//make it be nil
                minValueInRight->right = nodeToBeDeleted->right; //take the child of the node to be deleted
                minValueInRight->right->parent = minValueInRight; //update it to be the child
            }

            replaceNode(nodeToBeDeleted, minValueInRight);//replace both nodes so that minValueInRight becames in the place of to be delted
            minValueInRight->left = nodeToBeDeleted->left;//takes its left child
            minValueInRight->left->parent = minValueInRight;//update the parent
            minValueInRight->isRed = nodeToBeDeleted->isRed;//take the colour of the nodeToBeDeleted
        }
        if (wasNodeBlack) {
            fixDelete(replacementNode); //if the node is black fix it
        }
       // cout << "item with id " << nodeToBeDeleted->itemID << " with price: " << nodeToBeDeleted->price << " is deleted" << endl;
        // delete the node
        delete nodeToBeDeleted;
        //printTree();
    }

};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================
int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // calculate total coins
    int total = 0;
    for (int i = 0; i < n; i++) {
        total += coins[i];
    }
    bool dp[n + 1][total + 1];   // +1 ==> start from coin 0

    // set array to false
    for (int i = 0; i < n + 1; i++) {
        for (int j = 0; j < total + 1; j++) {
            dp[i][j] = false;
        }
    }
    dp[0][0] = true;  // set first index to true

    for (int i = 1; i < n + 1; i++) {  // coins
        for (int j = 0; j < total + 1; j++) {   // summation of coins
            dp[i][j] = dp[i - 1][j];
            if (coins[i - 1] <= j) {   // check
                // j-coins[i-1] summation of coins before adding coin i
                // if it was ture => when add i, new sum(j) will be available
                // so dp[i][j] true
                dp[i][j] = dp[i - 1][j] || dp[i - 1][j - coins[i - 1]];  // coins[i-1] = i in dp array
            }
        }
    }

    int mid = total / 2;

    while (!dp[n][mid]) {
        mid--;
    }

    int diff = total - 2 * mid;
    return diff;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int n = items.size();
    int dp[n + 1][capacity + 1];

    // set array = 0
    for (int i = 0; i < n + 1; i++) {
        for (int j = 0; j < capacity + 1; j++) {
            dp[i][j] = 0;
        }
    }

    for (int i = 1; i <= n; i++) {
        int weight = items[i - 1].first;
        int value = items[i - 1].second;

        for (int j = 1; j <= capacity; j++) {
            dp[i][j] = dp[i - 1][j];
            if (weight <= j) {
                dp[i][j] = max(dp[i][j], value + dp[i - 1][j - weight]);
            }
        }
    }
    return dp[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    const int MOD = 1000000007;
    int n = s.length();
    if (n == 0) return 1;

    long long dp[n+1];
    dp[0] = 1; // empty string
    dp[1] = 1; // single character

    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i-1]; // take current char as is

        // Check for "uu" -> "w"
        if (s[i-2] == 'u' && s[i-1] == 'u') {
            dp[i] = (dp[i] + dp[i-2]) % MOD;
        }
        // Check for "nn" -> "m"
        else if (s[i-2] == 'n' && s[i-1] == 'n') {
            dp[i] = (dp[i] + dp[i-2]) % MOD;
        }
    }

    return dp[n] % MOD;
}
// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

// Union-Find helper
vector<int> parent;
vector<int> rankArr;

void makeSet(int n) {
    parent.resize(n);
    rankArr.resize(n, 0);
    for (int i = 0; i < n; i++) {
        parent[i] = i;
    }
}

int find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

bool unite(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    if (rootX == rootY) return false;

    if (rankArr[rootX] < rankArr[rootY]) {
        parent[rootX] = rootY;
    } else if (rankArr[rootX] > rankArr[rootY]) {
        parent[rootY] = rootX;
    } else {
        parent[rootY] = rootX;
        rankArr[rootX]++;
    }
    return true;
}

// Path Existence

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    if (source == dest) return true;

    // Build adjacency list
    vector<vector<int>> adj(n);
    for (auto& edge : edges) {
        int u = edge[0];
        int v = edge[1];
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // BFS
    vector<bool> visited(n, false);
    queue<int> q;

    visited[source] = true;
    q.push(source);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        if (current == dest) return true;

        for (int neighbor : adj[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }

    return false;
}


//  Minimum Bribe Cost
long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                      vector<vector<int>>& roadData) {

    // Build adjacency list
    vector<vector<pair<int, long long>>> adj(n);
    for (auto& road : roadData) {
        int u = road[0];
        int v = road[1];
        long long gold = road[2];
        long long silver = road[3];
        long long weight = gold * goldRate + silver * silverRate;

        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight});
    }

    // Prim's algorithm
    vector<long long> key(n, LLONG_MAX);
    vector<bool> visited(n, false);

    priority_queue<pair<long long, int>,
                   vector<pair<long long, int>>,
                   greater<pair<long long, int>>> pq;

    // Start from vertex 0
    key[0] = 0;
    pq.push({0, 0});

    long long mstCost = 0;
    int visitedCount = 0;

    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();

        int u = current.second;
        long long weight = current.first;

        if (visited[u]) continue;

        visited[u] = true;
        mstCost += weight;
        visitedCount++;

        for (auto& neighbor : adj[u]) {
            int v = neighbor.first;
            long long w = neighbor.second;

            if (!visited[v] && w < key[v]) {
                key[v] = w;
                pq.push({w, v});
            }
        }
    }

    return (visitedCount == n) ? mstCost : -1;
}


// (Floyd-Warshall)

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    const long long INF = LLONG_MAX / 2;
    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    // Initialize
    for (int i = 0; i < n; i++) {
        dist[i][i] = 0;
    }

    // Add DIRECTED edges (matching test case 3)
    for (auto& road : roads) {
        int u = road[0];
        int v = road[1];
        long long weight = road[2];

        // Only set one direction (directional)
        if (weight < dist[u][v]) {
            dist[u][v] = weight;
        }

        // For bidirectional, uncomment below:
        // if (weight < dist[v][u]) {
        //     dist[v][u] = weight;
        // }
    }

    // Floyd-Warshall
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; j++) {
                if (dist[k][j] == INF) continue;
                if (dist[i][j] > dist[i][k] + dist[k][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }

    // Calculate sum
    long long totalSum = 0;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (dist[i][j] < INF) {
                totalSum += dist[i][j];
            }
        }
    }

    // Convert to binary
    if (totalSum == 0) return "0";

    string binary = "";
    while (totalSum > 0) {
        binary = to_string(totalSum % 2) + binary;
        totalSum /= 2;
    }

    return binary;
}
// =========================================================
// PART D: ServerKernel
// =========================================================
// TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
int ServerKernel::minIntervals(vector<char>& tasks, int n)
    {
        // ===== constraints check =====
        if (tasks.size() < 1 || tasks.size() > 10000)
            return -1;

        if (n < 0 || n > 100)
            return -1;
        // =============================

        int frequency[26] = {0};

        // count frequency
        for (int i = 0; i < tasks.size(); i++)
            frequency[tasks[i] - 'A']++;

        // max heap
        priority_queue<int> pq;
        for (int i = 0; i < 26; i++)
            if (frequency[i] > 0)
                pq.push(frequency[i]);

        int time = 0;

        while (!pq.empty())
        {
            int cycle = n + 1;
            vector<int> temp;

            while (cycle > 0 && !pq.empty())
            {
                int mostRepeated = pq.top();
                pq.pop();

                mostRepeated--;
                if (mostRepeated > 0)
                    temp.push_back(mostRepeated);

                time++;
                cycle--;
            }

            // push remaining tasks back
            for (int i = 0; i < temp.size(); i++)
                pq.push(temp[i]);

            // add idle time if tasks still exist
            if (!pq.empty())
                time += cycle;
        }

        return time;
    }


// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() {
        return new ConcretePlayerTable();
    }

    Leaderboard* createLeaderboard() {
        return new ConcreteLeaderboard();
    }

    AuctionTree* createAuctionTree() {
        return new ConcreteAuctionTree();
    }
}
