#pragma once
#include <algorithm>
#include <stdexcept>
#include <utility>

template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::AdaptableHeapPriorityQueue() : heap_{}, comp_{}, nextId_{1} {}
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::AdaptableHeapPriorityQueue(Compare comp) : heap_{}, comp_{std::move(comp)}, nextId_{1} {}
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::~AdaptableHeapPriorityQueue(){ for(auto e:heap_) delete e; }
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>::AdaptableHeapPriorityQueue(AdaptableHeapPriorityQueue&& other) noexcept : heap_(std::move(other.heap_)), comp_(std::move(other.comp_)), nextId_(other.nextId_) { for(std::size_t i=0;i<heap_.size();++i) heap_[i]->index=i; other.heap_.clear(); other.nextId_=1; }
template <typename K, typename V, typename Compare>
AdaptableHeapPriorityQueue<K,V,Compare>& AdaptableHeapPriorityQueue<K,V,Compare>::operator=(AdaptableHeapPriorityQueue&& other) noexcept { if(this==&other)return *this; for(auto e:heap_)delete e; heap_=std::move(other.heap_); comp_=std::move(other.comp_); nextId_=other.nextId_; for(std::size_t i=0;i<heap_.size();++i)heap_[i]->index=i; other.heap_.clear(); other.nextId_=1; return *this; }
template <typename K, typename V, typename Compare>
std::size_t AdaptableHeapPriorityQueue<K,V,Compare>::size() const noexcept{return heap_.size();}
template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K,V,Compare>::empty() const noexcept{return heap_.empty();}
template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K,V,Compare>::less(EntryPtr a,EntryPtr b) const{return comp_(a->key,b->key);}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::swapEntries(std::size_t i,std::size_t j) noexcept{std::swap(heap_[i],heap_[j]);heap_[i]->index=i;heap_[j]->index=j;}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::upHeap(std::size_t i){while(i>0){std::size_t p=(i-1)/2;if(!less(heap_[i],heap_[p]))break;swapEntries(i,p);i=p;}}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::downHeap(std::size_t i){while(true){std::size_t l=2*i+1;if(l>=heap_.size())break;std::size_t r=l+1,c=l;if(r<heap_.size()&&less(heap_[r],heap_[l]))c=r;if(!less(heap_[c],heap_[i]))break;swapEntries(i,c);i=c;}}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::bubble(std::size_t i){if(i>0&&less(heap_[i],heap_[(i-1)/2]))upHeap(i);else downHeap(i);}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::validateHandle(EntryPtr e) const{if(!e||e->index>=heap_.size()||heap_[e->index]!=e)throw std::invalid_argument("Invalid or stale priority-queue entry handle");}
template <typename K, typename V, typename Compare>
typename AdaptableHeapPriorityQueue<K,V,Compare>::EntryPtr AdaptableHeapPriorityQueue<K,V,Compare>::insert(const K& key,const V& value){
    // TODO: allocate, append, assign index/id, then up-heap.
    std::size_t idx = heap_.size();
    EntryPtr entry = new Entry{key, value, idx, nextId_++};
    heap_.push_back(entry);
    upHeap(idx);
    return entry;
}
template <typename K, typename V, typename Compare>
const typename AdaptableHeapPriorityQueue<K,V,Compare>::Entry& AdaptableHeapPriorityQueue<K,V,Compare>::min() const{if(empty())throw std::out_of_range("min() on empty priority queue");return *heap_.front();}
template <typename K, typename V, typename Compare>
typename AdaptableHeapPriorityQueue<K,V,Compare>::Entry AdaptableHeapPriorityQueue<K,V,Compare>::removeMin(){
    if (empty())
        throw std::out_of_range("removeMin() on empty priority queue");

    EntryPtr minEntry = heap_.front();
    Entry result = *minEntry;
    if(heap_.size() > 1)
        swapEntries(0, heap_.size() - 1);
    heap_.pop_back();
    delete minEntry;
    if(!empty())
        downHeap(0);
    return result;

}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::remove(EntryPtr e){
    validateHandle(e);
    std::size_t idx = e->index;
    std::size_t lastIdx = heap_.size() - 1;

    if(idx != lastIdx) 
        swapEntries(idx, lastIdx);
    heap_.pop_back();
    delete e;
    if(idx < heap_.size())
        bubble(idx);
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::replaceKey(EntryPtr e,const K& k){
    validateHandle(e);
    e->key = k;
    bubble(e->index);
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::replaceValue(EntryPtr e,const V& v){
    validateHandle(e);
    e->value = v;
}
template <typename K, typename V, typename Compare>
void AdaptableHeapPriorityQueue<K,V,Compare>::buildHeap(std::vector<std::pair<K,V>> entries){
    /* TODO: bottom-up construction, not repeated insert */
    for(auto e : heap_)
        delete e;
    heap_.clear();
    heap_.reserve(entries.size());

    for(auto& p: entries) {
        std::size_t idx = heap_.size();
        EntryPtr entry = new Entry{p.first, p.second, idx, nextId_++};
        heap_.push_back(entry);
    }
    if(!heap_.empty()) {
        for(long long i = static_cast<long long>(heap_.size() / 2) - 1; i>= 0; i--)
            downHeap(static_cast<std::size_t>(i));
    }
}
template <typename K, typename V, typename Compare>
bool AdaptableHeapPriorityQueue<K,V,Compare>::validHeap() const noexcept{for(std::size_t i=1;i<heap_.size();++i){std::size_t p=(i-1)/2;if(less(heap_[i],heap_[p])||heap_[i]->index!=i)return false;}return true;}
template <typename T, typename Compare>
void heapSort(std::vector<T>& values,Compare comp){
    // TODO: implement in-place heap sort. Default comparator must produce ascending order.
    if (values.size() <= 1) return;

    // Helper lambda for sift down using max-heap condition based on comp
    auto siftDown = [&](std::vector<T>& arr, std::size_t start, std::size_t end) {
        std::size_t root = start;
        while (2 * root + 1 <= end) {
            std::size_t child = 2 * root + 1;
            std::size_t swap_idx = root;

            if (comp(arr[swap_idx], arr[child])) {
                swap_idx = child;
            }
            if (child + 1 <= end && comp(arr[swap_idx], arr[child + 1])) {
                swap_idx = child + 1;
            }
            if (swap_idx == root) {
                return;
            } else {
                std::swap(arr[root], arr[swap_idx]);
                root = swap_idx;
            }
        }
    };

    std::size_t n = values.size();
    // Build max-heap
    for (long long i = static_cast<long long>(n / 2) - 1; i >= 0; --i) {
        siftDown(values, static_cast<std::size_t>(i), n - 1);
    }

    // Extract elements from heap one by one
    for (std::size_t i = n - 1; i > 0; --i) {
        std::swap(values[0], values[i]);
        siftDown(values, 0, i - 1);
    }
}
