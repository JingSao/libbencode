#ifndef INCLUDE_bencode_dict_hpp__
#define INCLUDE_bencode_dict_hpp__

#include <map>
#include <utility>
#include <bencode/basic_value.hpp>
#include <bencode/string.hpp>


namespace bencode
{


template
< typename CharT
, typename Traits = std::char_traits<CharT>
, template
< typename T
> class Allocator = std::allocator
, template
< typename Key
, typename T
, typename Compare
, typename Allocator
> class Container = std::map
> class basic_dict : public basic_value<CharT, Traits>
{
private:
    // Define the dict key alias.
    typedef basic_string<CharT, Traits> _Key;

    // Define the dict value alias.
    typedef basic_value<CharT, Traits> _Value;

    typedef std::shared_ptr<_Value> _Value_Ptr;

    // Define the key-value pair for the dict container.
    typedef std::pair<const _Key, _Value_Ptr> value_type;

    // Define the dict container key-value pairs allocator.
    typedef Allocator<value_type> _Alloc;

    // Define the lexicographical comparator as a wrapper
    // around the STL function.
    template <typename _T>
    struct lexicographical_compare
    {
        bool
        operator()(const _T& lhs, const _T& rhs)
        {
            return std::lexicographical_compare(
                lhs.cbegin(), lhs.cend(),
                rhs.cbegin(), rhs.cend());
        }
    };

    // Define the map comparator. According to the Bencode
    // specification the key in the associative list should
    // be sorted in the lexicographical order, therefore
    // it should denied for the user to specify the custom
    // comparator object.
    typedef lexicographical_compare<_Key> _Compare;

    // Define the dict container type to keep the map
    // of the Bencode values.
    typedef Container<_Key, _Value_Ptr, _Compare, _Alloc> _Dictionary_container;

    // Define the list constant iterator as a functional
    // equivalent to the container constant iterator.
    typedef typename _Dictionary_container::iterator iterator;

    // Define the list mutable iterator as a functional
    // equivalent to the container mutable iterator.
    typedef typename _Dictionary_container::const_iterator const_iterator;

    // The associative array of the key-value pairs.
    _Dictionary_container _M_container;

public:
    basic_dict() { }

    basic_dict(std::initializer_list<value_type> __list)
    : _M_container(__list.begin(), __list.end()) { }

    ~basic_dict() { }

    void
    dump(std::basic_ostream<CharT, Traits> &__s) const
    {
        __s << _Value::dict_type;

        std::for_each(_M_container.cbegin(), _M_container.cend(),
            [&__s](const value_type& value) {

            value.first.dump(__s);
            __s << _Value::delim_type;
            value.second->dump(__s);
        });

        __s << _Value::end_type;
    }

    void
    load(std::basic_istream<CharT, Traits> &__s) const
    {
        if (__s.peek() != _Value::dict_type) {
            throw type_error(
                "bencode::dict::load the specified stream does not "
                "contain interpretable bencode dictionary value\n");
        }

        // Read the "d" symbol from the provided stream.
        __s.get();

        // At the next step we are going to decode the items
        // of the B-encoded dictionary.
        while (__s.peek() != _Value::end_type) {
            // Read the key of the next dictionary item.
            _Key __key;
            __key.load(__s);

            if (__s.peek() != _Value::delim_type) {
                std::ostringstream __error(
                    "bencode::dict::load after the key a `:` "
                    "delimeter expected, but `");

                __error << __s.peek() << "` found\n";
                throw encoding_error(__error);
            }

            // Decode the value of the dictionary item.
            // TDB.
        }
    }

    void
    insert(std::initializer_list<value_type> __list)
    { _M_container.insert(__list.begin(), __list.cend()); }

    std::pair<iterator, bool>
    insert(const value_type& __value)
    { return _M_container.insert(__value); }
};


typedef basic_dict<char> dict;


typedef basic_dict<wchar_t> wdict;


} // namespace bencode

#endif // INCLUDE_bencode_dict_hpp__
