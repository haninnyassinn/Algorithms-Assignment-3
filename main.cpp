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
#include <set>

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
        double frac = (key*A) - (int)(key*A);   // fractional part which is (key⋅A)%1)
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
            if (usedSlots[idx]==false || playerIDs[idx] == playerID) {
                playerIDs[idx] = playerID;//set the id
                playerNames[idx] = name; // set the name or update it
                usedSlots[idx] = true;//set the place to true meaning its used
                return;
            }
        }
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
            playerID(id), score(s), next(list_level + 1, nullptr) {}
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
        start = new Node(-1, INT_MIN, maxNumberOfLists); // make the player id -1 and the score to min value
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
            // go to next slot while next exists means it isnt nullptr and has higher score
            while (current->next[list] != nullptr &&
                   current->next[list]->score > score) {
                current = current->next[list];//will point to next node
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
                if (current->next[list]->score > INT_MIN) {
                    current = current->next[list];
                } else {
                    break;
                }
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
            itemID=id;
            price=p;
            isRed=true; // all new leafs will be red at the start
            left=nullptr;
            right=nullptr;
            parent=nullptr;
        }
    };

    RBNode* root;
    RBNode* nil;  // leaf node for tree

    // this is a helper function that rotate x to left side
    void leftRotate(RBNode* x) {
        RBNode* y = x->right; //y will be the x-right and will take its position
        x->right = y->left; // the x-right child will be y->left now
        if (y->left != nil) {
            y->left->parent = x; // connect the child with the parent which is x
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y; // if x is the root then y will be the root
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
    }
    // this is a helper function that rotate x into right side
    void rightRotate(RBNode* x) {
        RBNode* y = x->left;
        x->left = y->right;

        if (y->right != nil) {
            y->right->parent = x;
        }

        y->parent = x->parent;

        if (x->parent == nullptr) {
            root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }

        y->right = x;
        x->parent = y;
    }
    void fixInsert(RBNode* z) {
        while (z->parent != nullptr && z->parent->isRed == true) {
            if (z->parent == z->parent->parent->left) {
                RBNode* y = z->parent->parent->right;  // Uncle

                // Case 1: Uncle is RED
                if (y->isRed == true) {
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;
                } else {
                    // Case 2: z is right child
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    // Case 3: z is left child
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    rightRotate(z->parent->parent);
                }
            } else {
                // Mirror cases
                RBNode* y = z->parent->parent->left;  // Uncle

                if (y->isRed == true) {
                    // Case 1
                    z->parent->isRed = false;
                    y->isRed = false;
                    z->parent->parent->isRed = true;
                    z = z->parent->parent;
                } else {
                    // Case 2
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    // Case 3
                    z->parent->isRed = false;
                    z->parent->parent->isRed = true;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->isRed = false;  // Root must be BLACK
    }
    // this is a helper function that replaces two nodes used for deleting
    void replaceNode(RBNode* u, RBNode* v) {
        if (u->parent == nullptr) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    //this is a helper function that we find min by it,used for deleting
    RBNode* findMin(RBNode* node) {
        while (node->left != nil) { //go to left size until we find nil
            node = node->left;//go to left size until we find nil
        }
        return node; //return node this will be the smallest node
    }

     // this is a helper function that fixes the tree after delete happenes
    void fixDelete(RBNode* x) {
        while (x != root && x->isRed == false) {
            if (x == x->parent->left) {
                RBNode* w = x->parent->right;  // Sibling

                // Case 1: Sibling is RED
                if (w->isRed == true) {
                    w->isRed = false;
                    x->parent->isRed = true;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }

                // Case 2: Both sibling's children are BLACK
                if (w->left->isRed == false && w->right->isRed == false) {
                    w->isRed = true;
                    x = x->parent;
                } else {
                    // Case 3: Sibling's right child is BLACK
                    if (w->right->isRed == false) {
                        w->left->isRed = false;
                        w->isRed = true;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    // Case 4: Sibling's right child is RED
                    w->isRed = x->parent->isRed;
                    x->parent->isRed = false;
                    w->right->isRed = false;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                // Mirror cases
                RBNode* w = x->parent->left;

                if (w->isRed == true) {
                    w->isRed = false;
                    x->parent->isRed = true;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }

                if (w->right->isRed == false && w->left->isRed == false) {
                    w->isRed = true;
                    x = x->parent;
                } else {
                    if (w->left->isRed == false) {
                        w->right->isRed = false;
                        w->isRed = true;
                        leftRotate(w);
                        w = x->parent->left;
                    }

                    w->isRed = x->parent->isRed;
                    x->parent->isRed = false;
                    w->left->isRed = false;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->isRed = false;
    }
    // this is a helper function to find node by the item id
    RBNode* findNodeByIDHelper(RBNode* node, int itemID) {
        if (node == nil) return nullptr; // empty

        if (node->itemID == itemID) return node; //check current node

        RBNode* leftResult = findNodeByIDHelper(node->left, itemID); //go left
        if (leftResult != nullptr) return leftResult;

        return findNodeByIDHelper(node->right, itemID);//go right
    }
    //this is to find the nide by id just by searching with id
    RBNode* findNodeByID( int itemID) {
        return findNodeByIDHelper(root, itemID); //returns the findNodeByIDHelper start from root
    }

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
        //create nil
        nil = new RBNode(-1, -1);
        nil->isRed = false; // the colour of nil is always black
        nil->left = nil;//it points to itself
        nil->right = nil;//it points to itself
        nil->parent = nullptr;
        root = nil; //empty tree it starts with nil(empty)
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
        // Create new node (starts as RED)
        RBNode* newNode = new RBNode(itemID, price);

        // Find where to insert (BST search by price)
        RBNode* y = nullptr;
        RBNode* x = root;

        while (x != nil) {
            y = x;
            if (price < x->price) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        // Set parent
        newNode->parent = y;

        if (y == nullptr) {
            root = newNode;  // First node
        } else if (price < y->price) {
            y->left = newNode;
        } else {
            y->right = newNode;
        }

        // Initialize as leaf
        newNode->left = nil;
        newNode->right = nil;
        newNode->isRed = true;  // New nodes are RED

        // Fix Red-Black tree properties
        fixInsert(newNode);
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
        // Find node to delete
        RBNode* z = findNodeByID( itemID);
        if (z == nullptr || z == nil) return;

        RBNode* y = z;
        RBNode* x;
        bool yWasBlack = (y->isRed == false);

        if (z->left == nil) {
            x = z->right;
            replaceNode(z, z->right);
        } else if (z->right == nil) {
            x = z->left;
            replaceNode(z, z->left);
        } else {
            y = findMin(z->right);
            yWasBlack = (y->isRed == false);
            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            } else {
                replaceNode(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            replaceNode(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->isRed = z->isRed;
        }

        // Fix tree if we removed a BLACK node
        if (yWasBlack) {
            fixDelete(x);
        }

        // Free memory
        delete z;
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================
int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // calculate total coins
    int total=0;
    for (int i=0; i<n; i++) {
        total+=coins[i];
    }
    bool dp[n+1][total+1];   // +1 ==> start from coin 0

    // set array to false
    for (int i=0; i<n+1; i++) {
        for (int j=0; j<total+1; j++) {
            dp[i][j] = false;
        }
    }
    dp[0][0] = true;  // set first index to true

    for (int i=1; i<n+1; i++) {  // coins
        for (int j=0; j<total+1; j++) {   // summation of coins
            dp[i][j] = dp[i-1][j];
            if (coins[i-1]<= j) {   // check
                // j-coins[i-1] summation of coins before adding coin i
                // if it was ture => when add i, new sum(j) will be available
                // so dp[i][j] true
                dp[i][j] = dp[i-1][j] || dp[i-1][j-coins[i-1]];  // coins[i-1] = i in dp array
            }
        }
    }

    int mid = total/2;

    while (!dp[n][mid]) {
        mid--;
    }

    int diff = total-2*mid;
    return diff;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    int n = items.size();
    int dp[n+1][capacity+1];

    // set array = 0
    for (int i=0; i<n+1; i++) {
        for (int j=0; j<capacity+1; j++) {
            dp[i][j] = 0;
        }
    }

    for (int i = 1; i <= n; i++) {
        int weight = items[i-1].first;
        int value  = items[i-1].second;

        for (int j = 1; j<=capacity; j++) {
            dp[i][j] = dp[i-1][j];
            if (weight <= j) {
                dp[i][j] = max(dp[i][j], value + dp[i-1][j-weight]);
            }
        }
    }
    return dp[n][capacity];
}

long long InventorySystem::countStringPossibilities(string s) {
    const int mod = 1000000007;
    int n = s.length();
    long long dp[n];
    dp[0] = 1;

    for (int i=1; i<n; i++) {
        dp[i] = 0;
    }

    for (int i=1; i<n; i++) {
        dp[i] = dp[i-1];
        if (s[i] == s[i-1]) {
            if (i>1)
                dp[i] = (dp[i]+dp[i-2])%mod;
            else
                dp[i] = (dp[i]+1)%mod;
        }
    }
    return dp[n-1];
}
// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // TODO: Implement path existence check using BFS or DFS
    // edges are bidirectional
    return false;
}

long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    // TODO: Implement Minimum Spanning Tree (Kruskal's or Prim's)
    // roadData[i] = {u, v, goldCost, silverCost}
    // Total cost = goldCost * goldRate + silverCost * silverRate
    // Return -1 if graph cannot be fully connected
    return -1;
}

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    // TODO: Implement All-Pairs Shortest Path (Floyd-Warshall)
    // Sum all shortest distances between unique pairs (i < j)
    // Return the sum as a binary string
    // Hint: Handle large numbers carefully
    return "0";
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================

int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // TODO: Implement task scheduler with cooling time
    // Same task must wait 'n' intervals before running again
    // Return minimum total intervals needed (including idle time)
    // Hint: Use greedy approach with frequency counting
    return 0;
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
// KEEP THIS FOR BUILDING - REMOVE BEFORE SUBMISSION

int main() {
    // Test code here...
    cout<<"Hello World!"<<endl;
    cout<<"hashing"<<endl;
    ConcretePlayerTable table;

    // 2. Add 3 players
    table.insert(1, "Ali");
    table.insert(2, "Omar");
    table.insert(3, "Ahmed");

    // 3. Test finding them
    cout << table.search(1) << endl;  // Should print "Ali"
    cout << table.search(2) << endl;  // Should print "Omar"
    cout << table.search(3) << endl;  // Should print "Ahmed"

    // 4. Test NOT finding someone
    cout << table.search(99) << endl; // Should print nothing (empty)

    // 5. Test updating
    table.insert(1, "Ali_New");
    cout << table.search(1) << endl;  // Should print "Ali_New"

    cout<<"skip lists"<<endl;
    ConcreteLeaderboard lb;

    lb.addScore(10, 50);   // add player 10
    lb.addScore(20, 30);   // add player 20
    lb.addScore(20, 80);   // update player 20 score

    auto top = lb.getTopN(5);

    for (int id : top)
        cout << id << " ";
    cout << "\n=== Testing AuctionTree (Red-Black Tree) ===" << endl;
    ConcreteAuctionTree auction;
    auction.insertItem(1, 500);
    auction.insertItem(2, 300);
    auction.insertItem(3, 700);
    auction.insertItem(4, 200);

    cout << "Items inserted successfully" << endl;
    auction.deleteItem(2);
    cout << "Item 2 deleted successfully" << endl;
    cout<<"dynamic programming"<<endl;
    vector<int>coinsA = {1,2,4};
    vector<int>coinsB = {2, 2};
    vector<int>coinsC = {1, 5, 11, 5};

    int a = InventorySystem::optimizeLootSplit(3, coinsA);
    int b = InventorySystem::optimizeLootSplit(2, coinsB);
    int c = InventorySystem::optimizeLootSplit(4, coinsC);

    cout << "\nLoot Splitting";
    cout << "\nThe minimum difference 1: " << a;
    cout << "\nThe minimum difference 2: " << b;
    cout << "\nThe minimum difference 3: " << c;

    cout << "\n";

    // Inventory Packer
    vector<pair<int,int>> itemsA = {{1,10}, {2,15}, {3,40}};
    vector<pair<int,int>> itemsB = {{1,10}, {2,15}, {3,40}};
    vector<pair<int,int>> itemsC = {{1,10}, {2,20}, {3,30}};

    a = InventorySystem::maximizeCarryValue(3, itemsA);
    b = InventorySystem::maximizeCarryValue(5, itemsB);
    c = InventorySystem::maximizeCarryValue(10, itemsC);

    cout << "\nInventory Packer";
    cout << "\nThe maximum total value they can carry 1: " << a;
    cout << "\nThe maximum total value they can carry 2: " << b;
    cout << "\nThe maximum total value they can carry 3: " << c;

    cout << "\n";

    // Chat Autocorrect
    long long num1;
    long long num2;
    num1 = InventorySystem::countStringPossibilities("uu");
    num2 = InventorySystem::countStringPossibilities("uunn");

    cout << "\nChat Autocorrect";
    cout << "\nThe number of possible original strings 1: " << num1;
    cout << "\nThe number of possible original strings 2: " << num2;

}
