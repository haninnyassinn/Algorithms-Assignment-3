#include <iostream>
#include <vector>
using namespace std;




// PART B: DYNAMIC PROGRAMMING
class InventorySystem {
public:
    // Minimizes difference between two coin stacks
    static int optimizeLootSplit(int n, vector<int>& coins);

    // 0/1 Knapsack logic
    static int maximizeCarryValue(int capacity, vector<pair<int, int>>& items);

    // String decoding possibilities
    static long long countStringPossibilities(string s);
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




int main() {

    // Loot Splitting
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



    return 0;
}