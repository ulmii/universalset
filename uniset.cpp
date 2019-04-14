#include <cstddef>
#include <array>
#include <bitset>

#include <iostream>

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

//FIXME: zmienic linijke w invalid argument
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
       throw std::invalid_argument("on line ???: Index out of bounds");
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

//FIXME: zmienic linijke w invalid_argument 
template <typename T, std::size_t UNIVERSE_SIZE>
const T& UniversalSet<T, UNIVERSE_SIZE>::elem(std::size_t index) const
{
    if(index >= 0u && index < UNIVERSE_SIZE)
    {   
        return __elements[index];
    }
    else
    {
       throw std::invalid_argument("on line ???: Index out of bounds");
    }
}

#include <string>

using namespace std;

template <typename T, std::size_t N>
void printSizeAndElems(const typename UniversalSet<T, N>::Vector &v)
{
    std::cout << "Size: " << v.count() << std::endl;
    
    for(auto &e : v) std::cout << e << ", ";
        std::cout << std::endl;
}

int main()
    {
    constexpr std::size_t N = 10u;
    UniversalSet<std::string, N> us({"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"});
 
    auto v1 = us.makeVector();
    auto v2 = us.makeVector();

    for(std::size_t i = 0u; i< us.universeSize(); ++i )
    {
        if(i&1u) v1.insert(i);
        else v2.insert(i);
    }
    auto v3 = v2;

    // std::cout << "count: " << v3.count() << std::endl;
    
    std::cout << (v1.isMember(1u) ? "Znaleziono" : "Nie znaleziono") << std::endl;
    std::cout << (v1.isMember(2u) ? "Znaleziono" : "Nie znaleziono") << std::endl;
    std::cout << (v2.remove(8u) ? "Usunieto" : "Nie usunieto") << std::endl;
    std::cout << (v2.remove(9u) ? "Usunieto" : "Nie usunieto") << std::endl;
    std::cout << (v3.insert(1u).second ? "Dodano" : "Nie dodano") << std::endl;
    std::cout << (v3.insert(2u).second ? "Dodano" : "Nie dodano") << std::endl;
    std::cout << (v3.insert(3u).second ? "Dodano" : "Nie dodano") << std::endl;
    std::cout << (v3.insert(4u).second ? "Dodano" : "Nie dodano") << std::endl;
    
    auto v4 = v1 + v3;
    const auto v5 = v3 - v2;
    auto v6 = v3 * v5;
    const auto v7 = v5 / v3;

    printSizeAndElems<std::string, N>(v4);
    printSizeAndElems<std::string, N>(v5);
    printSizeAndElems<std::string, N>(v6);
    printSizeAndElems<std::string, N>(v7);

    auto it1 = v7.elemIterator(1u);
    auto it2 = v7.elemIterator(2u);

    std::cout << (it1 ? *it1 : "end") << std::endl;
    std::cout << (it2 ? *it2 : "end") << std::endl;
    // *it2 = "nie powinno sie skompilowac";
    std::cout << us[2] << std::endl;
    // us[2] = "nie powinno sie skompilowac"; 

return 0;
}