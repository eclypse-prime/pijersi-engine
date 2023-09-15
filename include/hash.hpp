#ifndef HASH_HPP
#define HASH_HPP

#include <list>
#include <unordered_map>
#include <assert.h>

namespace PijersiEngine::Hash
{
    extern uint64_t pieceHashKeys[1575];
    void hashInit();
    uint64_t hash(uint8_t cells[45], int recursionDepth);

    template <class KEY_T, class VAL_T> class LRUCache{
    private:
        std::list<std::pair<KEY_T,VAL_T> > item_list;
        std::unordered_map<KEY_T, decltype(item_list.begin()) > item_map;
        size_t cache_size;
    private:
        void clean(void){
            while(item_map.size()>cache_size){
                auto last_it = item_list.end(); last_it --;
                item_map.erase(last_it->first);
                item_list.pop_back();
            }
        };
    public:
        LRUCache(int cache_size_):cache_size(cache_size_){
        };

        void put(const KEY_T &key, const VAL_T &val){
            auto it = item_map.find(key);
            if(it != item_map.end()){
                item_list.erase(it->second);
                item_map.erase(it);
            }
            item_list.push_front(std::make_pair(key,val));
            item_map.insert(std::make_pair(key, item_list.begin()));
            clean();
        };
        bool exist(const KEY_T &key){
            return (item_map.count(key)>0);
        };
        VAL_T get(const KEY_T &key){
            assert(exist(key));
            auto it = item_map.find(key);
            item_list.splice(item_list.begin(), item_list, it->second);
            return it->second->second;
        };
    };

    extern LRUCache<uint64_t, uint64_t> hashTable;
}

#endif