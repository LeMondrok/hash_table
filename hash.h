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
    using Elem = pair<const KeyType, ValueType>;
    using Table = vector<list<Elem>>;

    Table htable;
    Hash hashfunction;

    static constexpr size_t capmultiplier = 8;
    static constexpr double maxcapresize = 0.5, mincapresize = 0.1;
    static constexpr size_t initcapacity = 128;
    size_t sz = 0, capacity = initcapacity;

public:
    HashMap() {
        htable = Table(initcapacity);
        sz = 0;
    }

    void check_resize() {
        if (sz != 0) {
            if ((double)capacity * maxcapresize < sz ||
                (double)capacity * mincapresize > sz)
                resize();
        } else {
            return;
        }
    }

    void resize() {
        vector<Elem> queryelem;
        queryelem.reserve(sz);

        for (auto it = begin(); it != end(); ++it) {
            queryelem.emplace_back(*it);
        }

        clear();

        sz = queryelem.size();
        capacity = sz * capmultiplier;
        htable = Table(capacity);


        for (size_t i = 0; i < queryelem.size(); ++i) {
            reinsert(queryelem[i]);
        }
    }

    explicit HashMap(const Hash& h)
        : htable (Table(initcapacity))
        , hashfunction(h)
        , sz(0)
        {}

    template <typename Iter>
    HashMap(Iter begin, Iter end, const Hash& h = Hash()) {
        htable = Table(initcapacity);
        hashfunction = h;
        sz = 0;

        for (auto it = begin; it != end; ++it)
            this->insert(*it);
    }

    HashMap(std::initializer_list<Elem> init, Hash h = Hash()) {
        htable = Table(initcapacity);
        hashfunction = h;
        sz = 0;

        for (const auto& item : init)
            this->insert(item);
    }

    void reinsert(const Elem &element) {
        size_t pos = hashfunction(element.first) % capacity;

        htable[pos].push_back(element);
    }


    void insert(const Elem &element) {
        size_t pos = hashfunction(element.first) % capacity;

        for (auto i : htable[pos]) {
            if (i.first == element.first)
                return;
        }

        sz++;

        htable[pos].push_back(element);

        check_resize();
    }

    void erase(const KeyType key) {
        size_t pos = hashfunction(key) % capacity;

        for (typename list<Elem>::iterator it = htable[pos].begin(); it != htable[pos].end(); ++it) {
            if (it->first == key) {
                htable[pos].erase(it);

                sz--;

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
        return sz * 1;
    }

    bool empty() const {
        if (sz == 0)
            return 1;
        else
            return 0;
    }

    Hash hash_function() const {
        return hashfunction;
    }

    iterator find(const KeyType& key) {
        size_t pos = hashfunction(key) % capacity;

        for (typename list<Elem>::iterator i = htable[pos].begin(); i != htable[pos].end(); ++i) {
            if (i->first == key) {
                return iterator(pos, i, this);
            }
        }

        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = hashfunction(key) % capacity;

        for (typename list<Elem>::const_iterator i = htable[pos].begin(); i != htable[pos].end(); ++i) {
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
        htable = Table(cpy.htable);

        sz = cpy.sz;
        hashfunction = cpy.hashfunction;

        return *this;
    }
};
