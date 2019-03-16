#include <iostream>
#include <vector>
#include <list>
#include <functional>

using std::pair;
using std::hash;
using std::vector;
using std::list;

template<class KeyType, class ValueType, class Hash = hash<KeyType>> class HashMap {
private:
    using elem = pair<const KeyType, ValueType>;
    using HashTable = vector<list<elem>>;

    HashTable htable;
    Hash hsfunc;

    const size_t mult = 4;
    const double maxcapresize = 0.5;
    size_t sz = 0, capacity = 128;

public:
    HashMap() {
        htable = HashTable(capacity);
        sz = 0;
    }

    void resize() {
        if ((double)capacity * maxcapresize > sz)
            return;

        vector<elem> queuElem;
        queuElem.reserve(sz);

        for (auto i = begin(); i != end(); ++i) {
            queuElem.emplace_back(*i);
        }

        clear();

        capacity *= mult;
        htable = HashTable(capacity);

        for (size_t i = 0; i < queuElem.size(); ++i) {
            insert(queuElem[i]);
        }
    }

    explicit HashMap(const Hash& h)
        : htable (HashTable(128))
        , hsfunc(h)
        , sz(0)
        {}

    template <typename Iter>
    HashMap(Iter begin, Iter end, const Hash& h = Hash()) {
        htable = HashTable(128);
        hsfunc = h;
        sz = 0;

        for (auto it = begin; it != end; ++it)
            this->insert(*it);
    }

    HashMap(std::initializer_list<elem> init, Hash h = Hash()) {
        htable = HashTable(128);
        hsfunc = h;
        sz = 0;

        for (const auto& item : init)
            this->insert(item);
    }


    void insert(const elem &el) {
        size_t pos = hsfunc(el.first) % capacity;

        for (auto i : htable[pos]) {
            if (i.first == el.first)
                return;
        }

        sz++;

        htable[pos].push_back(el);

        resize();
    }

    void erase(const KeyType key) {
        size_t pos = hsfunc(key) % capacity;

        for (typename list<elem>::iterator i = htable[pos].begin(); i != htable[pos].end(); ++i) {
            if (i->first == key) {
                htable[pos].erase(i);

                sz--;

                return;
            }
        }

        return;
    }

    class iterator {
    private:
        size_t pos;
        typename list<elem>::iterator li;
        HashMap *parent;
    public:
        iterator(size_t pos_ = 0, typename list<elem>::iterator li_ = typename list<elem>::iterator(), HashMap *par_ = nullptr)
            : pos(pos_)
            , li(li_)
            , parent(par_)
            {}

        iterator operator++() {
            li++;

            if (li == parent->htable[pos].end()) {
                ++pos;

                while (pos < parent->htable.size() && parent->htable[pos].size() == 0) {
                    ++pos;
                }

                if (pos == parent->htable.size()) {
                    --pos;
                    li = parent->htable[pos].end();
                } else {
                    li = parent->htable[pos].begin();
                }
            }

            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;

            ++(*this);

            return temp;
        }

        elem& operator*() {
            return *li;
        }

        typename list<elem>::iterator operator->() {
            return li;
        }

        bool operator ==(iterator it) {
            if (pos == it.pos && li == it.li)
                return true;
            else
                return false;
        }

        bool operator !=(iterator it) {
            return (!(*this == it));
        }
    };

    class const_iterator {
    private:
        size_t pos;
        typedef typename list<elem>::const_iterator listIterConst;
        listIterConst li;
        const HashMap *parent;
    public:
        const_iterator(size_t pos_ = 0, listIterConst li_ = listIterConst(), const HashMap *par_ = nullptr)
            : pos(pos_)
            , li(li_)
            , parent(par_)
            {}

        const_iterator operator++() {
            li++;

            if (li == parent->htable[pos].end()) {
                ++pos;

                while (pos < parent->htable.size() && parent->htable[pos].size() == 0)
                    ++pos;

                if (pos == parent->htable.size()) {
                    --pos;
                    li = parent->htable[pos].end();
                } else {
                    li = parent->htable[pos].begin();
                }
            }

            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp = *this;

            ++(*this);

            return temp;
        }


        const elem& operator*() {
            return *li;
        }

        listIterConst operator->() {
            return li;
        }

        bool operator ==(const_iterator it) {
            if (pos == it.pos && li == it.li)
                return true;
            else
                return false;
        }

        bool operator !=(const_iterator it) {
            return (!(*this == it));
        }
    };

    iterator begin() {
        size_t pos = 0;

        while (pos < capacity && htable[pos].empty() == 1)
            ++pos;

        if (pos == capacity)
            return end();

        else return iterator(pos, htable[pos].begin(), this);
    }

    const_iterator begin() const {
        size_t pos = 0;

        while (pos < capacity && htable[pos].empty() == 1)
            ++pos;

        if (pos == capacity)
            return end();

        return const_iterator(pos, htable[pos].begin(), this);
    }

    iterator end() {
        return iterator(capacity - 1, htable.back().end(), this);
    }

    const_iterator end() const {
        return const_iterator(capacity - 1, htable[capacity - 1].end(), this);
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        if (sz == 0)
            return 1;
        else
            return 0;
    }

    Hash hash_function() const {
        return hsfunc;
    }

    iterator find(const KeyType& key) {
        size_t pos = hsfunc(key) % capacity;

        for (typename list<elem>::iterator i = htable[pos].begin(); i != htable[pos].end(); ++i) {
            if (i->first == key) {
                return iterator(pos, i, this);
            }
        }

        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = hsfunc(key) % capacity;

        for (typename list<elem>::const_iterator i = htable[pos].begin(); i != htable[pos].end(); ++i) {
            if (i->first == key) {
                return const_iterator(pos, i, this);
            }
        }

        return end();
    }

    ValueType& operator[](const KeyType& key) {
        if (find(key) == end()) {
            insert(std::make_pair(key, ValueType()));
        }

        return (*find(key)).second;
    }

    const ValueType& at(const KeyType& k) const {
        auto it = find(k);

        if (it == end()) {
            throw std::out_of_range("out of range");
        }

        return (*find(k)).second;
    }

    void clear() {
        for (size_t i = 0; i < capacity; ++i) {
            htable[i].erase(htable[i].begin(), htable[i].end());
        }

        sz = 0;
    }

    HashMap& operator=(const HashMap& cpy) {
        htable = HashTable(cpy.htable);

        sz = cpy.sz;
        hsfunc = cpy.hsfunc;

        return *this;
    }
};
