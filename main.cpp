#include "bloom.h"
#include <iostream>
#include <string>
#include "MurmurHash3.h"
#include <cstring>

using namespace std;

int check_equal(double num1, double num2){
    if(num1 != num2){
        cout << "  " << num1 << " != " << num2 << endl;
    }
    return 0;
}

int main() {
    // Constructor Test
    {
        cout << "Constructor Test" << endl;

        Bloom myBloom;
        if(myBloom.GetMainTable()) {
            cout << "  bloomFilter isn't null" << endl;
        }
    }

    // InitBloomTable Test
    {
        cout << "InitBloomTable Test" << endl;
        
        Bloom myBloom;
        myBloom.InitBloomTable(10, 0.01);
        shared_ptr<bloom_table> test = myBloom.GetMainTable();

        check_equal(test->maxFpProb, 0.01);
        check_equal(test->count, 1);

        shared_ptr<bit_table> bit_test = test->tables->at(0);
        check_equal(bit_test->items, 0);
        check_equal(bit_test->expected, 10);
        check_equal(bit_test->size, 111);
        check_equal(bit_test->maxFpProb, 0.005);
        check_equal(bit_test->hash_count, 8);

        int ones = 0;
        for(int i = 0; i < bit_test->table->size(); i++){
            if(bit_test->table->at(i) == 1){ones++;}
        }
        if(ones > 0){
            cout << "  Table contains 1's" << endl;
        }
    }

    // GetHashBuckets Test
    {
        cout << "GetHashBuckets Test" << endl;
        Bloom myBloom;

        vector<int> check;
        uint32_t out[1];
        int bucket;

        const char *key = "Summer.of.George99";
        MurmurHash3_x86_32(key, (int)strlen(key), 1, out);
        bucket = out[0] % 47;
        check.push_back(bucket);
        MurmurHash3_x86_32(key, (int)strlen(key), 2, out);
        bucket = out[0] % 47;
        check.push_back(bucket);
        MurmurHash3_x86_32(key, (int)strlen(key), 3, out);
        bucket = out[0] % 47;
        check.push_back(bucket);

        vector<int> test = myBloom.GetHash("Summer.of.George99", 3, 47);
        for(int i = 0; i < 3; i++){
            check_equal(check.at(i), test.at(i));
        }

        check.clear();
        test.clear();
        
        const char *key2 = "!Bob_Sacamano";
        MurmurHash3_x86_32(key2, (int)strlen(key2), 1, out);
        bucket = out[0] % 192;
        check.push_back(bucket);
        MurmurHash3_x86_32(key2, (int)strlen(key2), 2, out);
        bucket = out[0] % 192;
        check.push_back(bucket);
        MurmurHash3_x86_32(key2, (int)strlen(key2), 3, out);
        bucket = out[0] % 192;
        check.push_back(bucket);
        MurmurHash3_x86_32(key2, (int)strlen(key2), 4, out);
        bucket = out[0] % 192;
        check.push_back(bucket);
        MurmurHash3_x86_32(key2, (int)strlen(key2), 5, out);
        bucket = out[0] % 192;
        check.push_back(bucket);

        test = myBloom.GetHash("!Bob_Sacamano", 5, 192);
        for(int i = 0; i < 5; i++){
            check_equal(check.at(i), test.at(i));
        }
    }

    // NameAvailable (Blank Table) Test
    {
        cout << "NameAvailable (Blank Table) Test" << endl;
        Bloom myBloom;
        myBloom.InitBloomTable(10, 0.01);

        check_equal(1, myBloom.NameAvailable("Art.Vandelay"));
        check_equal(1, myBloom.NameAvailable("Costanza7"));
        check_equal(1, myBloom.NameAvailable("33Kramer9"));
    }

    // AddName (1 Table) Test
    {
        cout << "AddName (1 Table) Test" << endl;
        Bloom myBloom;
        myBloom.InitBloomTable(10, 0.01);

        // Check manually
        myBloom.AddName("Art.Vandelay");
        myBloom.AddName("Costanza7");
        myBloom.AddName("33Kramer9");
        myBloom.AddName("Elaine_Benes");
        vector<vector<int>> test_group;
        test_group.push_back(myBloom.GetHash("Art.Vandelay", 8, 111));
        test_group.push_back(myBloom.GetHash("Costanza7", 8, 111));
        test_group.push_back(myBloom.GetHash("33Kramer9", 8, 111));
        test_group.push_back(myBloom.GetHash("Elaine_Benes", 8, 111));

        shared_ptr<vector<bool>> test_table = myBloom.GetMainTable()->tables->at(0)->table;
        for(int i = 0; i < test_group.size(); i++){
            vector<int> cur = test_group.at(i);
            for(int j = 0; j < cur.size(); j++){
                check_equal(1, test_table->at(cur.at(j)));
            }
        }

        // Check with NameAvailable
        check_equal(0, myBloom.NameAvailable("Art.Vandelay"));
        check_equal(0, myBloom.NameAvailable("Costanza7"));
        check_equal(0, myBloom.NameAvailable("33Kramer9"));
        check_equal(0, myBloom.NameAvailable("Elaine_Benes"));

        // Check some names that weren't added
        check_equal(1, myBloom.NameAvailable("Jerry.Seinfeld"));
        check_equal(1, myBloom.NameAvailable("Newman111"));
        check_equal(1, myBloom.NameAvailable("elaine_benes"));
    }

    

    // FalsePosProbability Test
    {

    }
    
}