#ifndef __DUMP_H__
#define __DUMP_H__

#include "../utils/parameters.h"
#include "../utils/argument_parser.h"
#include "../utils/utils.h"
#include "../frequent_pattern_mining/frequent_pattern_mining.h"
#include "../data_processing/documents.h"

namespace Dump
{

using FrequentPatternMining::Pattern;
using FrequentPatternMining::patterns;
using FrequentPatternMining::pattern2id;
using FrequentPatternMining::truthPatterns;
using FrequentPatternMining::unigrams;
using Documents::id2token;

void dumpLabels(const string& filename, const vector<Pattern>& truth)
{
    FILE* out = tryOpen(filename, "w");
    for (Pattern pattern : truth) {
        for (int j = 0; j < pattern.tokens.size(); ++ j) {
            fprintf(out, "%s%c", id2token[pattern.tokens[j]].c_str(), j + 1 == pattern.tokens.size() ? '\t' : ' ');
        }
        fprintf(out,"%d\n",pattern.label);
    }
    fclose(out);
}

void dumpFeatures(const string& filename, const vector<vector<double>>& features, const vector<Pattern>& truth)
{
    FILE* out = tryOpen(filename, "w");
    for (Pattern pattern : truth) {
        PATTERN_ID_TYPE i = FrequentPatternMining::pattern2id[pattern.hashValue];
        if (features[i].size() > 0) {
            for (int j = 0; j < features[i].size(); ++ j) {
                fprintf(out, "%.10f%c", features[i][j], j + 1 == features[i].size() ? '\n' : '\t');
            }
        }
    }
    fclose(out);
}


template<class T>
void dumpRankingList(const string& filename, vector<pair<T, PATTERN_ID_TYPE>> &order)
{
    FILE* out = tryOpen(filename, "w");
    string intType = "i";
    sort(order.rbegin(), order.rend());
    for (size_t iter = 0; iter < order.size(); ++ iter) {
        PATTERN_ID_TYPE i = order[iter].second;
        if (typeid(order[iter].first).name() == intType) {
            fprintf(out, "%d\t", order[iter].first);
        } else {
            fprintf(out, "%.10f\t",order[iter].first);
        }
        for (int j = 0; j < patterns[i].tokens.size(); ++ j) {
            fprintf(out, "%s%c", id2token[patterns[i].tokens[j]].c_str(), j + 1 == patterns[i].tokens.size() ? '\n' : ' ');
        }
    }
    fclose(out);
}

void dumpResults(const string& prefix)
{
    vector<pair<double, PATTERN_ID_TYPE>> order;
    for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
        if (patterns[i].size() > 1 && patterns[i].currentFreq > 0) {
            order.push_back(make_pair(patterns[i].quality, i));
        }
    }
    dumpRankingList(prefix + "_multi-words.txt", order);

    order.clear();
    for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
        if (patterns[i].size() == 1 && patterns[i].currentFreq > 0 && unigrams[patterns[i].tokens[0]] >= MIN_SUP) {
            order.push_back(make_pair(patterns[i].quality, i));
        }
    }
    dumpRankingList(prefix + "_unigrams.txt", order);

    order.clear();
    for (PATTERN_ID_TYPE i = 0; i < patterns.size(); ++ i) {
        if (patterns[i].size() > 1 && patterns[i].currentFreq > 0 || patterns[i].size() == 1 && patterns[i].currentFreq > 0 && unigrams[patterns[i].tokens[0]] >= MIN_SUP) {
            order.push_back(make_pair(patterns[i].quality, i));
        }
    }
    dumpRankingList(prefix + "_salient.txt", order);
}

};

#endif