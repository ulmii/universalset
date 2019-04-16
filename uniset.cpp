#include <cstddef>
#include <array>
#include <bitset>

template <typename T, std::size_t UNIVERSE_SIZE>
class UniversalSet
{
    public: class Vector
    {
        friend class Iterator;
    
        class Iterator 
        {
            friend class Vector;

            public:
                Iterator(const Iterator&) = default;
                Iterator(Iterator&&) = default;

                Iterator& operator=(const Iterator&) = default;
                Iterator& operator=(Iterator&&) = default;

                bool operator==(const Iterator &it) const;
                bool operator!=(const Iterator &it) const { return !(*this == it); }
                Iterator& operator++() { do ++__index; while(!__vector_ptr->__bitset[__index] && __index < UNIVERSE_SIZE); return *this; }
                Iterator operator++(int) { Iterator temp(*this); ++(*this); return temp; }
                const T& operator*() const { return __vector_ptr->__universe->elem(__index); }
                const T* operator->() const { return *__vector_ptr->__universe->elem(__index); }
                operator bool() const { return __index != UNIVERSE_SIZE; }

            private:
                Iterator(const Vector* vector_ptr)
                    : __vector_ptr(vector_ptr) {} 

                Iterator(const Vector* vector_ptr, std::size_t index)
                    : __vector_ptr(vector_ptr), __index(index) {}

                const Vector* __vector_ptr;
                std::size_t __index{};
        };

        public:
            Vector(const UniversalSet* universe_ptr) 
                : __universe(universe_ptr) {}

            Vector(const UniversalSet* universe_ptr, const std::bitset<UNIVERSE_SIZE>& bitset)
                : __universe(universe_ptr), __bitset(bitset) {}

            Vector(const Vector&) = default;
            Vector(Vector&& universe) = default;

            Vector& operator=(const Vector& other) = default;
            Vector& operator=(Vector&& other) = default;

            bool operator==(const Vector& other) const; 
            bool operator!=(const Vector& other) const { return !(*this == other); }
            Vector operator+(const Vector& other) const { return Vector( __universe, (__bitset | other.__bitset) ); }
            Vector operator-(const Vector& other) const { return Vector( __universe, (__bitset & ~other.__bitset) ); }
            Vector operator*(const Vector& other) const { return Vector( __universe, (__bitset & other.__bitset) ); }
            Vector operator/(const Vector& other) const { return Vector( __universe, (__bitset ^ other.__bitset) ); }

            std::size_t count() const noexcept { return __bitset.count(); }
            std::pair<Iterator, bool> insert(std::size_t index);
            bool isMember(std::size_t index) const { if(isValidIndex(index)) return __bitset[index]; return false; }
            Iterator elemIterator(std::size_t index) const;
            bool remove(std::size_t index);
    
            Iterator begin() const { std::size_t index{0u}; while(!isMember(index)) ++index; return Iterator(this, index); }
            Iterator end() const { return Iterator(this, UNIVERSE_SIZE); }

        private:
            bool isValidIndex(std::size_t index) const { if(index >= 0u && index < UNIVERSE_SIZE) return true; return false; }

            std::bitset<UNIVERSE_SIZE> __bitset;
            const UniversalSet<T, UNIVERSE_SIZE>* __universe;
    };
    public:
        UniversalSet() = default;
        UniversalSet(const std::array<T, UNIVERSE_SIZE> &elems) 
            : __elements(elems) {}

        UniversalSet(const UniversalSet&) = default;
        UniversalSet(UniversalSet&&) = default;

        UniversalSet& operator=(const UniversalSet&) = default;
        UniversalSet& operator=(UniversalSet&&) = default;

        const T& operator[](std::size_t index) const { return elem(index); }

        constexpr std::size_t universeSize() const { return UNIVERSE_SIZE; }
        const T& elem(std::size_t index) const;
        Vector makeVector() const { return Vector(this); }

    private:
        std::array<T, UNIVERSE_SIZE> __elements;
};

template <typename T, std::size_t UNIVERSE_SIZE>
bool UniversalSet<T, UNIVERSE_SIZE>::Vector::Iterator::operator==(const Iterator &it) const 
{
    return __vector_ptr->__bitset == it.__vector_ptr->__bitset && __index == it.__index;
}

template <typename T, std::size_t UNIVERSE_SIZE>
bool UniversalSet<T, UNIVERSE_SIZE>::Vector::operator==(const Vector& other) const 
{
    return __universe->__elements == other.__universe->__elements && __bitset == other.__bitset;
}

template <typename T, std::size_t UNIVERSE_SIZE>
std::pair<typename UniversalSet<T, UNIVERSE_SIZE>::Vector::Iterator, bool> UniversalSet<T, UNIVERSE_SIZE>::Vector::insert(std::size_t index)
{
    if(isValidIndex(index))
    {   
        if(!__bitset[index])
        {
            __bitset[index] = true;
            return std::pair<Iterator, bool>(Iterator(this, index), true);
        }
        else
        {
            return std::pair<Iterator, bool>(Iterator(this, index), false);
        }
    }
    else
    {
       throw std::invalid_argument("UniversalSet::Vector::Iterator::insert() : Index out of bounds");
    }
}

template <typename T, std::size_t UNIVERSE_SIZE>
typename UniversalSet<T, UNIVERSE_SIZE>::Vector::Iterator UniversalSet<T, UNIVERSE_SIZE>::Vector::elemIterator(std::size_t index) const 
{
    if(isValidIndex(index) && isMember(index)) 
        if(__bitset[index])
            return Iterator(this, index);
    
    return Iterator(this, UNIVERSE_SIZE);
}

template <typename T, std::size_t UNIVERSE_SIZE>
bool UniversalSet<T, UNIVERSE_SIZE>::Vector::remove(std::size_t index)
{
   if(isValidIndex(index))
        if(__bitset[index])
        {
            __bitset[index] = false;
            return true;
        }
    
    return false;
}

template <typename T, std::size_t UNIVERSE_SIZE>
const T& UniversalSet<T, UNIVERSE_SIZE>::elem(std::size_t index) const
{
    if(index >= 0u && index < UNIVERSE_SIZE)
    {   
        return __elements[index];
    }
    else
    {
       throw std::invalid_argument("UniversalSet::elem() : Index out of bounds");
    }
}