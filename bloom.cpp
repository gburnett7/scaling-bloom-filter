#include "bloom.h"
#include <math.h>
#include "MurmurHash3.h"
#include <cstring>
#include <iostream>

using namespace std;

Bloom::Bloom(){
    bloomFilter = shared_ptr<bloom_table>(NULL);
}

Bloom::~Bloom(){
}

void Bloom::InitBloomTable(int expItems, double maxFpProb){
    // Initialize bloomFilter
    bloomFilter = shared_ptr<bloom_table>(new bloom_table);
    bloomFilter->count = 0;
    bloomFilter->maxFpProb = maxFpProb;
    bloomFilter->tables = shared_ptr<table_set>(new table_set(0, NULL));

    // Create first bit table
    InitBitTable(expItems, maxFpProb * 0.5);

}

bool Bloom::NameAvailable(string checkName){
    if(bloomFilter){
        // Check each table
        for(int i = 0; i < bloomFilter->tables->size(); i++){
            shared_ptr<bit_table> cur = bloomFilter->tables->at(i);
            vector<int> buckets = GetHashBuckets(checkName, cur->hash_count, cur->size);
            bool check = true;

            // Check each hash bucket
            for(int j = 0; j < buckets.size(); j++){
                int bucket = buckets.at(j);
                check = check && cur->table->at(bucket);
            }
            if(check){return false;}
        }
    }
    return true;
}

bool Bloom::AddName(string newName){
    if(bloomFilter && NameAvailable(newName)){
        int tableCount = bloomFilter->tables->size();
        shared_ptr<bit_table> lastTable = bloomFilter->tables->at(tableCount - 1);
        vector<int> buckets = GetHashBuckets(newName, lastTable->hash_count, lastTable->size);

        // Set hash buckets to true
        for(int j = 0; j < buckets.size(); j++){
            int bucket = buckets.at(j);
            lastTable->table->at(bucket) = 1;
        }

        lastTable->items++;
        Expand();
        return true;
    }

    return false;
}

double Bloom::FalsePosProbability(){
    if(bloomFilter){
        double mult = 1;
        for(int i = 0; i < bloomFilter->tables->size(); i++){
            shared_ptr<bit_table> cur = bloomFilter->tables->at(i);
            double prob = pow(1 - pow(1 - 1 / double(cur->size), double(cur->hash_count * cur->items)), cur->hash_count);
            mult *= (1 - prob);
        }
        return 1 - mult;
    }

    return 0;
}

void Bloom::Reset(){
    if(bloomFilter){
        // Remove all bit tables except the first
        for(int i = bloomFilter->count; i > 1; i--){
            bloomFilter->tables->pop_back();
            bloomFilter->count--;
        }

        // Reset vector to false
        shared_ptr<bit_table> initialTbl = bloomFilter->tables->at(0);
        initialTbl->items = 0;
        initialTbl->table = shared_ptr<vector<bool>>(new vector<bool>(initialTbl->size, 0));
    }
}

void Bloom::Expand(){
    int tableCount = bloomFilter->tables->size();
    shared_ptr<bit_table> lastTable = bloomFilter->tables->at(tableCount - 1);

    if(lastTable->expected == lastTable->items){
        InitBitTable(lastTable->expected * 2, lastTable->maxFpProb * 0.5);
    }
}

vector<int> Bloom::GetHashBuckets(string input, int hashCount, int tblSize){
    vector<int> ret;
    
    // Convert input to char array for hash func
    char char_array[input.size() + 1];
    strcpy(char_array, input.c_str());
    const char *key = char_array;

    // Get each hash bucket
    for(int i = 0; i < hashCount; i++){
        uint32_t out[1];
        MurmurHash3_x86_32(key, (int)strlen(key), 1+i, out);
        int bucket = out[0] % tblSize;
        ret.push_back(bucket);
    }

    return ret;
}

void Bloom::InitBitTable(int expItems, double maxFpProb){
    shared_ptr<bit_table> newBit(new bit_table);
    bloomFilter->tables->push_back(newBit);
    bloomFilter->count++;

    newBit->items = 0;
    newBit->maxFpProb = maxFpProb;
    newBit->expected = expItems;

    double size = ceil(-1.0 * double(expItems) * log(newBit->maxFpProb) / pow(log(2), 2));
    newBit->size = size;

    double numHash = ceil(double(newBit->size) * log(2) / double(newBit->expected));
    newBit->hash_count = numHash;

    // Initialize vector to false
    newBit->table = shared_ptr<vector<bool>>(new vector<bool>(newBit->size, 0));
}


// Testing Functions
shared_ptr<bloom_table> Bloom::GetMainTable(){
    return this->bloomFilter;
}

vector<int> Bloom::GetHash(string input, int hashCount, int tblSize){
    return GetHashBuckets(input, hashCount, tblSize);
}