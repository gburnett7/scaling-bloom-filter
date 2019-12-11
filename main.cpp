#include "bloom.h"
#include <iostream>
#include <string>

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

    // NameAvailable Test
    {
        cout << "NameAvailable Test" << endl;
        Bloom myBloom;
        myBloom.InitBloomTable(10, 0.01);

        if(!myBloom.NameAvailable("Art.Vandelay")){
            cout << "Art.Vandelay should not have been found" << endl;
        }
        if(!myBloom.NameAvailable("Costanza7")){
            cout << "Costanza7 should not have been found" << endl;
        }
        if(!myBloom.NameAvailable("33Kramer9")){
            cout << "Art.Vandelay should not have been found" << endl;
        }
        if(!myBloom.NameAvailable("Elaine_Benes")){
            cout << "Art.Vandelay should not have been found" << endl;
        }
    }

    // FalsePosProbability Test
    {

    }
    
}