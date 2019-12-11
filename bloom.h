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
    void Resize();
    vector<int> GetHashBuckets(string input, int hashCount, int tblSize);

public:
    Bloom();

    ~Bloom();

    void InitBloomTable(int expItems, double maxFpProb);

    bool NameAvailable(string checkName);

    bool AddName(string newName);

    double FalsePosProbability();

    // Functions for testing
    shared_ptr<bloom_table> GetMainTable();
};