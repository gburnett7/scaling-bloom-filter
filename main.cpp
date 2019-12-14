#include "bloom.h"
#include <iostream>
#include <string>
#include "MurmurHash3.h"
#include <cstring>
#include <math.h>

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

    // InitBloomTable/InitBitTable Test
    {
        cout << "InitBloomTable/InitBitTable Test" << endl;
        
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

    // 1 Table Test
    {
        cout << "1 Table Test" << endl;
        Bloom myBloom;
        myBloom.InitBloomTable(10, 0.01);

        myBloom.AddName("Art.Vandelay");
        myBloom.AddName("Costanza7");
        myBloom.AddName("33Kramer9");
        myBloom.AddName("Elaine_Benes");

        // Check Manually
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

        // Check that item count updated
        check_equal(4, myBloom.GetMainTable()->tables->at(0)->items);

        //Check that the bloom filter wasn't resized
        check_equal(1, myBloom.GetMainTable()->tables->size());

        // Check false positive calculation
        double test_prob = myBloom.FalsePosProbability();
        double check_prob = pow(1 - pow(1 - 1.0 / 111, 8.0 * 4.0), 8.0);
        cout << "  FP Prob (check manually): " << check_prob << " = "  << test_prob << endl;

        // Reset filter
        myBloom.Reset();
        check_equal(1, myBloom.GetMainTable()->count);
        check_equal(1, myBloom.GetMainTable()->tables->size());
        check_equal(0, myBloom.GetMainTable()->tables->at(0)->items);

        shared_ptr<bit_table> bit_test = myBloom.GetMainTable()->tables->at(0);
        int ones = 0;
        for(int i = 0; i < bit_test->table->size(); i++){
            if(bit_test->table->at(i) == 1){ones++;}
        }
        if(ones > 0){
            cout << "  Table contains 1's" << endl;
        }
    }

    // Multi-Table Test
    {
        cout << "Multi-Table Test" << endl;
        Bloom myBloom;
        myBloom.InitBloomTable(5, 0.1);

        myBloom.AddName("Art.Vandelay");
        myBloom.AddName("Costanza7");
        myBloom.AddName("33Kramer9");
        myBloom.AddName("Elaine_Benes");
        myBloom.AddName("Jerry.Seinfeld");
        myBloom.AddName("Newman111");
        myBloom.AddName("elaine_benes");

        // Successfully expanded?
        check_equal(2, myBloom.GetMainTable()->tables->size());
        check_equal(2, myBloom.GetMainTable()->count);
        check_equal(5, myBloom.GetMainTable()->tables->at(0)->items);
        check_equal(2, myBloom.GetMainTable()->tables->at(1)->items);

        // Check Properties of second table
        check_equal(0.025, myBloom.GetMainTable()->tables->at(1)->maxFpProb);
        check_equal(10, myBloom.GetMainTable()->tables->at(1)->expected);
        check_equal(77, myBloom.GetMainTable()->tables->at(1)->size);
        check_equal(6, myBloom.GetMainTable()->tables->at(1)->hash_count);

        // Does NameAvailable check both tables?
        check_equal(0, myBloom.NameAvailable("Art.Vandelay"));
        check_equal(0, myBloom.NameAvailable("Costanza7"));
        check_equal(0, myBloom.NameAvailable("33Kramer9"));
        check_equal(0, myBloom.NameAvailable("Elaine_Benes"));
        check_equal(0, myBloom.NameAvailable("Jerry.Seinfeld"));
        check_equal(0, myBloom.NameAvailable("Newman111"));
        check_equal(0, myBloom.NameAvailable("elaine_benes"));

        // Overall false positive probability
        cout << "  FP Prob (check manually): 0.0493565 = " << myBloom.FalsePosProbability() << endl;

        // Add enough names to requre a third table
        myBloom.AddName("FrankCostanza!?#");
        myBloom.AddName("SERENTIY_NOW!!!");
        myBloom.AddName("david.Puddy_Go_Devils");
        myBloom.AddName("Marine Biologist3");
        myBloom.AddName("1Latex.Salesman");
        myBloom.AddName("Vandelay_Industries");
        myBloom.AddName("Yadda.Yadda.Yadda");
        myBloom.AddName("SevenCostanza7");
        myBloom.AddName("Mug_Costanza");

        // Successfully expanded?
        check_equal(3, myBloom.GetMainTable()->tables->size());
        check_equal(3, myBloom.GetMainTable()->count);
        check_equal(5, myBloom.GetMainTable()->tables->at(0)->items);
        check_equal(10, myBloom.GetMainTable()->tables->at(1)->items);
        check_equal(1, myBloom.GetMainTable()->tables->at(2)->items);

        // Check Properties of third table
        check_equal(0.0125, myBloom.GetMainTable()->tables->at(2)->maxFpProb);
        check_equal(20, myBloom.GetMainTable()->tables->at(2)->expected);
        check_equal(183, myBloom.GetMainTable()->tables->at(2)->size);
        check_equal(7, myBloom.GetMainTable()->tables->at(2)->hash_count);

        // Overall false positive probability
        cout << "  FP Prob (check manually): 0.0738701 = " << myBloom.FalsePosProbability() << endl;

        // Reset filter
        myBloom.Reset();
        check_equal(1, myBloom.GetMainTable()->count);
        check_equal(1, myBloom.GetMainTable()->tables->size());
        check_equal(0, myBloom.GetMainTable()->tables->at(0)->items);

        shared_ptr<bit_table> bit_test = myBloom.GetMainTable()->tables->at(0);
        int ones = 0;
        for(int i = 0; i < bit_test->table->size(); i++){
            if(bit_test->table->at(i) == 1){ones++;}
        }
        if(ones > 0){
            cout << "  Table contains 1's" << endl;
        }
    }
}