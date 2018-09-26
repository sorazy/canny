#ifndef HASHMAP_HPP
#define HASHMAP_HPP

// Point class that will be used in the HashMap
class Point
{
public:
	int x, y;
	Point(int, int);
	Point(void);
};

// For my implementation, we're not going to need a delete fucntion
class HashMap
{
	public:
		HashMap(void);
		void insert(int, int);
		int contains(int, int);
		int getSize(void);
		int getCapacity(void);
		void printTable(void);
	private:
		Point **table;
		void expand();
		int hash(Point*);
		int size = 0;
		int capacity = 503;
		int currentPrime = 0;
		int primes[10] = {503, 1009, 2027, 4637, 10061, 22391, 47161};
};

#endif
