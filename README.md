# Scaling Bloom Filter

This project is a scalable implementation of the bloom filter data structure. It is inspired largely by the theory layed out in http://gsd.di.uminho.pt/members/cbm/ps/dbloom.pdf and is intended to be used in situations where the number of input elements is not known ahead of time. Instead of using a single table, as in the standard implementation, we use a series of bit-tables that increase in size and decrease in false-positive limit. The false-positive limit of each successive table follows a geometric progression, thereby scaling to the overall desired threshold. For our hash function, we used MurmurHash3.

The implementation itself is located in bloom.cpp. main.cpp contains some simple tests I used to check

To compile this project, run the following command:

```
g++ -o main main.cpp bloom.cpp MurmurHash3.cpp
```

Then run the tests with the following command:

```
./main
```

The main function will print the test names to the console, along with any problems it discovers. The false-positive results must be checked manually.

## Acknowledgments

* http://gsd.di.uminho.pt/members/cbm/ps/dbloom.pdf
* MurmurHash3 - Austin Appleby: https://github.com/aappleby/smhasher/blob/master/src/MurmurHash3.cpp