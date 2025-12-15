#include <iostream>
#include <queue>
using namespace std;

int leastInterval(char tasks[], int length, int n)
{
    int frequency[26] = { 0 }; //how many times the letter is repeated

    // count frequency
    for (int i = 0; i < length; i++)
        frequency[tasks[i] - 'A']++; //subtracting each ASCII value of a letter with the ASCII value of the letter 'A', 

    // max heap because the time complexity should be O(nlog(n))
    priority_queue<int> pq;
    for (int i = 0; i < 26; i++) 
        if (frequency[i] > 0) //if the task repeated 
            pq.push(frequency[i]); //then push it in the queue

    int time = 0; //counter

    while (!pq.empty())
    {
        int cycle = n + 1; //position 
        vector<int> temp; //store repeated tasks 

        // execute up to n+1 tasks
        while (cycle > 0 && !pq.empty())
        {
            int mostrepeated = pq.top(); //get the most repeated task
            pq.pop(); //removing it from the queue
            
            m the queue
            mostrepeated--;

            if (mostrepeated > 0)
                temp.push_back(mostrepeated); //storing any remaining repeats in the queue

            time++;
            cycle--;
        }

        // push remaining tasks back
        for (; cycle > 0 && !pq.empty(); cycle--)
        {
            mostrepeated = pq.top(); 
            pq.pop(); 
            mostrepeated--;

            if (mostrepeated > 0)
                temp.push_back(curr);

            time++;
        }

    }

    return time;
}

int main()
{
    int length;
    cout << "Enter number of tasks: ";
    cin >> length;

    char tasks[10000];
    cout << "Enter tasks (A-Z): ";
    for (int i = 0; i < length; i++)
        cin >> tasks[i];

    int n;
    cout << "Enter cooling interval n: ";
    cin >> n;

    cout << "Minimum CPU intervals = "
        << leastInterval(tasks, length, n) << endl;

    return 0;
}
