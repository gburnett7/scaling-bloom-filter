#include <string>
#include <memory>
#include <vector>

using namespace std;

struct bit_table {
    // number of inserted strings
    int items;

    // expected number of items
    int expected;

    // number of buckets in table
    int size;

    // max probability of false positives
    double maxFpProb;

    // number of hash functions
    int hash_count;

    // points to vector of booleans
    shared_ptr<vector<bool>> table;
};

typedef vector<shared_ptr<bit_table>> table_set;

struct bloom_table {
    // max desired probability of false positives
    double maxFpProb;

    // number of bit tables
    int count;

    // vector of bit tables
    shared_ptr<table_set> tables;
};



class Bloom {
private:
    shared_ptr<bloom_table> bloomFilter;

    // Adds a new table if all existing tables can no longer accept new
    // elements. FP probability of new tables follows the geometric 
    // progression with common ratio (1/2). Expected elements is double
    // the previous table.
    void Expand();

    // Calculates all hash values based on the input string and number 
    // of specified has functions. Then returns the corresponding table
    // buckts based on the specified table size
    vector<int> GetHashBuckets(string input, int hashCount, int tblSize);

    // Initalizes a new bit_table and adds it to the bloom_table 'tables'
    // vector.
    void InitBitTable(int expItems, double maxFpProb);

public:
    Bloom();

    ~Bloom();

    // Creates the bloom_table object and initializes the first bit table
    void InitBloomTable(int expItems, double maxFpProb);

    // Checks all existing bit tables for inclusion of the input string
    bool NameAvailable(string checkName);

    // Adds a new name to last table in filter if the name is available
    // and then expands the filter with a new table if necesssary
    bool AddName(string newName);

    // Returns the overall probability of having a false positive when
    // checking for the availability of a name
    double FalsePosProbability();

    // Removes all but the initial table and resets every bucket to false
    void Reset();

    // Functions for testing
    shared_ptr<bloom_table> GetMainTable();
    
    vector<int> GetHash(string input, int hashCount, int tblSize);
};