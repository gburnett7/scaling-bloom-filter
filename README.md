# scaling-bloom-filter

To compile this code, run the following command in the terminal while in the project folder:

```
g++ -o main main.cpp bloom.cpp MurmurHash3.cpp
```

Then run the tests with the following command:

```
./main
```

The main function will print the test names to the console, along with any problems it discovers. The false-positive results must be checked manually.