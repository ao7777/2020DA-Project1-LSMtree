#include "SSTable.h"
#include <string>
#include <iostream>
#include <cstdio>
#include <cmath>
SStable::SStable(std::string &dir) : directory(dir), file(dir, std::iostream::out | std::iostream::in | std::iostream::binary)
{
    hfile.open(dir + "/headerfile.txt", std::fstream::binary | std::fstream::in | std::fstream::out);
    if (!hfile)
    {
        levels.push_back(0);
        hfile.open(dir + "/headerfile.txt", std::fstream::binary | std::fstream::in | std::fstream::trunc | std::fstream::out);
    }
    else
    {
        initialize();
    }
    headerPos = MAXFILESIZE;
}
SStable::~SStable()
{
    file.close();
    hfile.close();
}
void SStable::merge(int level)
{
    //merges the last sstable on this level into next level
    skiplist<key_t, dataPair<int, key_t>> ends; //key:first key;value: sequence&lastkey
    for (int i = 0; i < levels[level + 1]; i++) //reads data of the next level
    {
        auto p = indexCache[pos(level + 1, i)];
        ends.insert(p.getFirst()->key, dataPair<int, key_t>(i, p.getLast()->key));
    }
    auto lastPos = pos(level + 1, 0) - 1;
    auto l = indexCache[lastPos];
    ends.insert(l.getFirst()->key, dataPair<int, key_t>(-1, l.getLast()->key)); //reads the last sstable on this level
    dataPair<int, key_t> prev(0, 0);
    std::vector<dataPair<int, std::ifstream>> candidates;
    for (auto &&p = ends.getNext(); p; p = ends.getNext())
    { //get a vector of sstables that overlaps
        if (prev.value && p->value.value <= prev.value)
        {
            std::ifstream it(directory + "/level" + std::to_string(level + (p->value.key > 0)) + "-" +
                                 std::to_string((p->value.key > 0) ? p->value.key : lastPos),
                             std::ios::in | std::ios::binary);
            std::ifstream ip(directory + "/level" + std::to_string(level + (prev.key > 0)) + "-" +
                                 std::to_string((prev.key) > 0 ? prev.key : lastPos),
                             std::ios::in | std::ios::binary);
            if (prev.value != 0)
                candidates.push_back(dataPair<int, std::ifstream>(prev.key + 1, ip, 0));
            candidates.push_back(dataPair<int, std::ifstream>(p->value.key + 1, ip, 0));
        }
        prev = p->value;
    }
    skiplist<key_t, dataPair<
                        std::shared_ptr<skiplist<key_t, value_t>::qNode>, std::ifstream>>
        merger;
    for (auto i : candidates)
    {
        auto lev = level + (i.key == 0);
        auto idx = (i.key) ? i.key : lastPos;
        merger.insert(indexCache[pos(lev, idx)].getFirst()->key, dataPair<
                                                                     std::shared_ptr<skiplist<key_t, value_t>::qNode>,
                                                                     std::ifstream>(indexCache[pos(lev, idx)].getFirstNode(), std::move(i.value)));
    } //sort files by the first key
    std::ofstream of(directory + "/level" + std::to_string(level + 1) + "-" + std::to_string(0), std::ios::out | std::ios::binary);
    int order = 0;
    indexCache[pos(level + 1, order)] = skiplist<key_t, std::streamoff>();
    std::vector<dataPair<key_t, std::streamoff>> offsets;
    while (!merger.isEmpty())
    {
        auto &&p = merger.getFirst();
        key_t length;
        p->value.value.read((char *)&length, sizeof(length));
        if ((uint64_t)of.tellp() + sizeof(key_t) + sizeof(length) + length >= MAXFILESIZE * pow(10, level))
        {
            of.seekp(MAXFILESIZE * pow(10, level));
            for (auto i : offsets)
            {
                std::streamoff offset = i.value;
                of.write((char *)&i.key, sizeof(key_t));
                of.write((char *)&offset, sizeof(std::streamoff));
            }
            of.close();
            of.open(directory + "/level" + std::to_string(level + 1) + "-" + std::to_string(++order), std::ios::out | std::ios::binary);
            indexCache[pos(level + 1, order)] = skiplist<key_t, std::streamoff>();
        }
        of.write((char *)&p->key, sizeof(key_t));
        offsets.push_back(dataPair<key_t, std::streamoff>(p->value.key->entry.key, of.tellp()));
        indexCache[pos(level + 1, order)].insert(p->value.key->entry.key, of.tellp());
        char *str = new char[length];
        p->value.value.read(str, length);
        of.write((char *)&length, sizeof(length));
        of.write(str, length);
        merger.remove(p->value.key->entry.key);
        merger.insert(p->value.key->next->entry.key, dataPair<
                                                         std::shared_ptr<skiplist<key_t, value_t>::qNode>,
                                                         std::ifstream>(p->value.key->next, std::move(p->value.value)));
    }
    of.seekp(MAXFILESIZE * pow(10, level));
    for (auto i : offsets)
    {
        std::streamoff offset = i.value;
        of.write((char *)&i.key, sizeof(key_t));
        of.write((char *)&offset, sizeof(std::streamoff));
    }
    of.close();
};
skiplist<key_t, std::streamoff> SStable::mergeWrite(int level, int table1, int table2)
{
}
std::shared_ptr<value_t> SStable::get(key_t)
{
    return nullptr;
};
void SStable::write(skiplist<key_t, value_t> &s)
{
    auto fileName = directory + "/level" + std::to_string(0) + '-' + std::to_string(levels[0]); //第0层第0个
    file.open(fileName, std::ios::out | std::ios::binary);
    std::vector<dataPair<key_t, std::streamoff>> offsets;
    auto &&p = s.getNext();
    int initTime = 0;
    indexCache.push_back(skiplist<key_t, std::streamoff>());
    while (p)
    {
        auto length = p->value.size();
        if ((uint64_t)file.tellp() + sizeof(key_t) + sizeof(length) + length >= MAXFILESIZE)
        {
            file.seekp(MAXFILESIZE);
            for (auto i : offsets)
            {
                std::streamoff offset = i.value;
                file.write((char *)&i.key, sizeof(key_t));
                file.write((char *)&offset, sizeof(std::streamoff));
            }
            file.close();
            file.open(directory + "/level" + std::to_string(0) + "-" + std::to_string(++levels[0]), std::ios::out | std::ios::binary);
            indexCache.push_back(skiplist<key_t, std::streamoff>());
        }
        file.write((char *)&p->key, sizeof(key_t));
        offsets.push_back(dataPair<key_t, std::streamoff>(p->key, file.tellg()));
        indexCache.back().insert(p->key, file.tellg());
        file.write((char *)&length, sizeof(length));
        file.write(p->value.c_str(), length);
        p = s.getNext();
    }
    file.seekp(MAXFILESIZE);
    for (auto i : offsets)
    {
        std::streamoff offset = i.value;
        file.write((char *)&i.key, sizeof(key_t));
        file.write((char *)&offset, sizeof(std::streamoff));
    }
    headerPos = file.tellp();
    levels[0]++;
    int i = 0;
    while ((levels[i] > 4 * pow(10.0, i)) || levels[0] == 4)
        merge(i++);
    auto levelHeight = levels.size();
    hfile.seekp(std::ios::beg);
    hfile.write((char *)&levelHeight, sizeof(levelHeight));
    for (auto i : levels)
        hfile.write((char *)&i, sizeof(i));
};
bool SStable::remove(key_t key)
{
    return false;
}
void SStable::initialize()
{
    uint64_t levelHeight = 0;
    hfile.read((char *)&levelHeight, sizeof(levelHeight));
    for (int i = 0; i < levelHeight; i++)
    {
        key_t num;
        hfile.read((char *)&num, sizeof(num));
        levels.push_back(num);
        for (int j = 0; j < num; j++)
        {
            std::ifstream is(directory + "/level" + std::to_string(i) + "-" + std::to_string(j), std::ios::in | std::ios::binary); //第i层第j个
            is.seekg(MAXFILESIZE);
            key_t key;
            std::streamoff pos;
            indexCache.push_back(skiplist<key_t, std::streamoff>());
            while (is.read((char *)&key, sizeof(key)))
            {
                is.read((char *)&pos, sizeof(pos));
                indexCache[this->pos(i, j)].insert(key, pos);
            }
        }
    }
}