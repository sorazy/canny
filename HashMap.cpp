#include <iostream>
#include "HashMap.h"

// Point constructor
Point::Point(int a, int b)
{
	x = a;
	y = b;
}

// HashMap constructor
HashMap::HashMap(void)
{
	primes = {503, 1009, 2027, 4637, 10061, 22391};
	size = 0;
	capacity = primes[0];
	currentPrime = 0;
	table = new Point[primes[0]];
}

HashMap::hash(int a, int b)
{
	return (a * b) % this->primes[this->currentPrime];
}
