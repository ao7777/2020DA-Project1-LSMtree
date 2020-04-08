#include "kvstore.h"
#include <string>
#include "skiplist.h"
KVStore::KVStore(const std::string &dir): KVStoreAPI(dir),dirc(dir)
{
	
}

KVStore::~KVStore()
{
	
}

/**
 * Insert/Update the key-value pair.
 * No return values for simplicity.
 */
void KVStore::put(uint64_t key, const std::string &s)
{
	memtable.insert(key,s);
}
/**
 * Returns the (string) value of the given key.
 * An empty string indicates not found.
 */
std::string KVStore::get(uint64_t key)
{
	auto s=memtable.get(key);
	return s?*s:"";
}
/**
 * Delete the given key-value pair if it exists.
 * Returns false iff the key is not found.
 */
bool KVStore::del(uint64_t key)
{
	return memtable.remove(key);
	return false;
}

/**
 * This resets the kvstore. All key-value pairs should be removed,
 * including memtable and all sstables files.
 */
void KVStore::reset()
{
	memtable.~skiplist();
}
