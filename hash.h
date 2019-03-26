#include <iostream>
#include <vector>
#include <list>
#include <functional>

using std::pair;
using std::hash;
using std::vector;
using std::list;

template<class KeyType, class ValueType, class Hash = hash<KeyType>> class HashMap {
public:
    static const size_t CAPACITY_MULTIPLIER = 8;
    static constexpr double MAX_CAPACITY_RESIZE = 0.5;
    static constexpr double MIN_CAPACITY_RESIZE = 0.1;
    static const size_t INIT_CAPACITY = 128;

    using Elem = pair<const KeyType, ValueType>;
    using Table = vector<list<Elem>>;

    HashMap() {
        hTable = Table(INIT_CAPACITY);
        sz = 0;
    }

    void check_resize() {
        if (sz != 0) {
            if ((double)capacity * MAX_CAPACITY_RESIZE < sz ||
                (double)capacity * MIN_CAPACITY_RESIZE > sz)
                resize(sz * CAPACITY_MULTIPLIER);
        } else {
            return;
        }
    }

    void check_insert() {
        if (sz != 0 && (double)capacity * MAX_CAPACITY_RESIZE < sz)
            resize(sz * CAPACITY_MULTIPLIER);
        else
            return;
    }

    void check_erase() {
        if (sz != 0 && (double)capacity * MIN_CAPACITY_RESIZE > sz)
            resize(sz * CAPACITY_MULTIPLIER);
        else
            return;
    }

    void resize(size_t resizeCapacity) {
        vector<Elem> queryElem;
        queryElem.reserve(sz);

        for (auto it = begin(); it != end(); ++it) {
            queryElem.emplace_back(*it);
        }

        clear();

        sz = queryElem.size();
        capacity = resizeCapacity;
        hTable = Table(capacity);


        for (size_t i = 0; i < queryElem.size(); ++i) {
            reinsert(queryElem[i]);
        }
    }

    explicit HashMap(const Hash& h)
        : hTable (Table(INIT_CAPACITY))
        , hashFunction(h)
        , sz(0)
        {}

    template <typename Iter>
    HashMap(Iter begin, Iter end, const Hash& h = Hash()) {
        hTable = Table(INIT_CAPACITY);
        hashFunction = h;
        sz = 0;

        for (auto it = begin; it != end; ++it)
            this->insert(*it);
    }

    HashMap(std::initializer_list<Elem> init, Hash h = Hash()) {
        hTable = Table(INIT_CAPACITY);
        hashFunction = h;
        sz = 0;

        for (const auto& item : init)
            this->insert(item);
    }

    void reinsert(const Elem &element) {
        size_t pos = hashFunction(element.first) % capacity;

        hTable[pos].push_back(element);
    }


    void insert(const Elem &element) {
        size_t pos = hashFunction(element.first) % capacity;

        for (auto i : hTable[pos]) {
            if (i.first == element.first)
                return;
        }

        sz++;

        hTable[pos].push_back(element);

        check_insert();
    }

    void erase(const KeyType key) {
        size_t pos = hashFunction(key) % capacity;

        for (typename list<Elem>::iterator it = hTable[pos].begin(); it != hTable[pos].end(); ++it) {
            if (it->first == key) {
                hTable[pos].erase(it);

                sz--;

                check_erase();

                return;
            }
        }

        return;
    }

    class iterator {
    private:
        size_t pos;
        typename list<Elem>::iterator li;
        HashMap *parent;
    public:
        iterator(size_t pos_ = 0, typename list<Elem>::iterator li_ = typename list<Elem>::iterator(), HashMap *parent_ = nullptr)
            : pos(pos_)
            , li(li_)
            , parent(parent_)
            {}

        iterator operator++() {
            li++;

            if (li == parent->hTable[pos].end()) {
                ++pos;

                while (pos < parent->hTable.size() && parent->hTable[pos].size() == 0) {
                    ++pos;
                }

                if (pos == parent->hTable.size()) {
                    --pos;
                    li = parent->hTable[pos].end();
                } else {
                    li = parent->hTable[pos].begin();
                }
            }

            return *this;
        }

        iterator operator++(int) {
            iterator temp = *this;

            ++(*this);

            return temp;
        }

        Elem& operator*() {
            return *li;
        }

        typename list<Elem>::iterator operator->() {
            return li;
        }

        bool operator ==(iterator it) {
            return (pos == it.pos && li == it.li);
        }

        bool operator !=(iterator it) {
            return (!(*this == it));
        }
    };

    class const_iterator {
    private:
        size_t pos;
        typedef typename list<Elem>::const_iterator listIterConst;
        listIterConst li;
        const HashMap *parent;
    public:
        const_iterator(size_t pos_ = 0, listIterConst li_ = listIterConst(), const HashMap *parent_ = nullptr)
            : pos(pos_)
            , li(li_)
            , parent(parent_)
            {}

        const_iterator operator++() {
            li++;

            if (li == parent->hTable[pos].end()) {
                ++pos;

                while (pos < parent->hTable.size() && parent->hTable[pos].size() == 0)
                    ++pos;

                if (pos == parent->hTable.size()) {
                    --pos;
                    li = parent->hTable[pos].end();
                } else {
                    li = parent->hTable[pos].begin();
                }
            }

            return *this;
        }

        const_iterator operator++(int) {
            const_iterator temp = *this;

            ++(*this);

            return temp;
        }


        const Elem& operator*() {
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

        while (pos < capacity && hTable[pos].empty() == 1)
            ++pos;

        if (pos == capacity)
            return end();

        else return iterator(pos, hTable[pos].begin(), this);
    }

    const_iterator begin() const {
        size_t pos = 0;

        while (pos < capacity && hTable[pos].empty() == 1)
            ++pos;

        if (pos == capacity)
            return end();

        return const_iterator(pos, hTable[pos].begin(), this);
    }

    iterator end() {
        return iterator(capacity - 1, hTable.back().end(), this);
    }

    const_iterator end() const {
        return const_iterator(capacity - 1, hTable[capacity - 1].end(), this);
    }

    size_t size() const {
        return sz;
    }

    bool empty() const {
        return (sz == 0);
    }

    Hash hash_function() const {
        return hashFunction;
    }

    iterator find(const KeyType& key) {
        size_t pos = hashFunction(key) % capacity;

        for (typename list<Elem>::iterator i = hTable[pos].begin(); i != hTable[pos].end(); ++i) {
            if (i->first == key) {
                return iterator(pos, i, this);
            }
        }

        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = hashFunction(key) % capacity;

        for (typename list<Elem>::const_iterator i = hTable[pos].begin(); i != hTable[pos].end(); ++i) {
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
            hTable[i].erase(hTable[i].begin(), hTable[i].end());
        }

        sz = 0;
    }

    HashMap& operator=(const HashMap& cpy) {
        hTable = Table(cpy.hTable);

        sz = cpy.sz;
        hashFunction = cpy.hashFunction;

        return *this;
    }

private:
    Table hTable;
    Hash hashFunction;

    size_t sz = 0, capacity = INIT_CAPACITY;
};
