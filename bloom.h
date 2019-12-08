

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


class Bloom {
private:
    bit_table bloom_table;
    float maxFpProb;
    void Resize();

public:
    Bloom();

    ~Bloom();

    void InitTable(int expItems, float maxFpProb);

    bool NameAvailable(string checkName);

    bool AddName(string newName);

    float CollisionProbability();
};