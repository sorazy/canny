#include <iostream>

using namespace std;

// Point class that will be used in the HashMap
class Point
{
	int x, y;
public:
	Point(int, int);
};

class HashMap
{
	public:
		HashMap(void);
		int insert(int, int);
		int get(int, int);
		int getSize(void);
		int getCapacity(void);
	private:
		Point *table;
		int expand();
		int hash(int, int)
		int size;
		int capacity;
		int currentPrime;
		int primes[10];
};
