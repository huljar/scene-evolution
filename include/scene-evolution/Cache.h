#ifndef CACHE_H
#define CACHE_H

#include <map>
#include <queue>
#include <utility>

template<class K, class V>
class Cache {
public:
    typedef K key_type;
    typedef V mapped_type;
    typedef std::pair<K, V> value_type;
    typedef typename std::map<K, V>::iterator iterator;
    typedef typename std::map<K, V>::const_iterator const_iterator;

    Cache(size_t cacheSize) : mCacheSize(cacheSize) { }

    iterator insert(const K& key, const V& value) {
        iterator pos = mCacheMap.find(key);
        if(pos == mCacheMap.end()) {
            pos = mCacheMap.insert(std::make_pair(key, value)).first;
            mCacheQueue.push(key);
        }

        while(mCacheQueue.size() > mCacheSize) {
            K front = mCacheQueue.front();
            mCacheQueue.pop();
            mCacheMap.erase(front);
        }

        return pos;
    }

    iterator load(const K& key) {
        return mCacheMap.find(key);
    }

    bool isValid(const iterator& iter) {
        return iter != mCacheMap.end();
    }

    bool isValid(const const_iterator& iter) {
        return iter != mCacheMap.cend();
    }

    size_t getCacheSize() const {
        return mCacheSize;
    }

    void setCacheSize(size_t cacheSize) {
        mCacheSize = cacheSize;

        while(mCacheQueue.size() > cacheSize) {
            K front = mCacheQueue.front();
            mCacheQueue.pop();
            mCacheMap.erase(front);
        }
    }

protected:
    typedef std::map<K, V> CacheMap;
    typedef std::queue<K> CacheQueue;

    CacheMap mCacheMap;
    CacheQueue mCacheQueue;
    size_t mCacheSize;
};

#endif // CACHE_H
