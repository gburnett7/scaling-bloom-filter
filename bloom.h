#include <string>
#include <memory>
#include <vector>

using namespace std;

struct bit_table {
    // number of inserted strings
    int items;

    // number of buckets in table
    int size;

    // number of hash functions
    int hash_count;

    // points to vector of booleans
    shared_ptr<vector<bool>> table;
};

typedef vector<shared_ptr<bit_table>> table_set;

struct bloom_table {
    // max desired probability of false positives
    int maxFpProb;

    // number of bit tables
    int count;

    // vector of bit tables
    table_set tables;
};



class Bloom {
private:
    shared_ptr<bloom_table> bloomFilter;
    void Resize();

public:
    Bloom();

    ~Bloom();

    void InitTable(int expItems, float maxFpProb);

    bool NameAvailable(string checkName);

    bool AddName(string newName);

    float FalsePosProbability();
};