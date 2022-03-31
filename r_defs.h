#ifndef R_DEFS_H
#define R_DEFS_H

#include <list>
#include <cstdint>
#include <type_traits>
#include <stdexcept>
#include <span>

template<typename Test, template<typename...> class Template>
struct is_specialization : std::false_type {};

template<template<typename...> class Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <class Test, template <class...> class Template>
inline constexpr bool is_specialization_v = is_specialization<Test, Template>::value;

template <typename T>
class safe_list_t
{
    std::list<T> elems;

public:

    struct iterator
    {
        using sub_iterator = typename decltype(elems)::iterator;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = typename sub_iterator::difference_type;
        using value_type = typename sub_iterator::value_type;
        using pointer = typename sub_iterator::pointer;
        using reference = typename sub_iterator::reference;

        iterator() {}

        iterator(sub_iterator i, sub_iterator e) : m_current{ i }, m_next{ i }, m_end{e} {
            if (m_next != m_end)
                std::advance(m_next, 1);
        }

        reference operator*() const { return m_current.operator*(); }
        pointer operator->() const { return m_current.operator->(); }
        iterator& operator++()
        {
            m_current = m_next;
            if (m_next != m_end)
                std::advance(m_next, 1);
            return *this;
        }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        sub_iterator here() const { return m_current; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.m_current == b.m_current; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.m_current != b.m_current; };

        friend iterator safe_list_t::erase(iterator i);
    private:
        
        sub_iterator m_current;
        sub_iterator m_next;
        sub_iterator m_end;
    };
    

    template< class... Args >
    auto emplace_front(Args&&... args) {return elems.emplace_front(std::forward<Args...>(args...)); };

    template< class... Args >
    auto emplace_back(Args&&... args) { return elems.emplace_back(std::forward<Args...>(args...)); };

    template< class... Args >
    iterator emplace(iterator pos, Args&&... args) { return iterator{ elems.emplace(pos.here(), std::forward<Args...>(args...)), elems.end() }; }

    iterator begin() { return iterator{ elems.begin(), elems.end() }; }
    iterator end() { return iterator{ elems.end(), elems.end() }; }
    iterator erase(iterator i) { return iterator{ elems.erase(i.m_current), elems.end() }; }
};

template <typename T, typename = std::enable_if_t<is_specialization_v<T, safe_list_t>>>
struct list_key_t
{
    using key_t = T::iterator;

    list_key_t() : m_me{ nullptr }, m_key{} {}
    list_key_t(T* me, key_t key) : m_me{ me }, m_key{ key } {}

    list_key_t(const list_key_t&) = delete;
    list_key_t& operator=(const list_key_t&) = delete;

    list_key_t(list_key_t&& rhs)
        : m_me{ rhs.m_me }
        , m_key{ rhs.m_key }
    {
        if (rhs.m_me)
        {
            rhs.m_key = std::end(*rhs.m_me);
            rhs.m_me = nullptr;
        }
    }
    list_key_t& operator=(list_key_t&& rhs)
    {
        release();
        m_me = rhs.m_me;
        m_key = rhs.m_key;
        if (rhs.m_me)
        {
            rhs.m_key = std::end(*rhs.m_me);
            rhs.m_me = nullptr;
        }
        return *this;
    }

    ~list_key_t() { release(); }

    T* m_me;
    key_t m_key;

    void release()
    {
        if (m_me && m_key != std::end(*m_me))
        {
            m_me->erase(m_key);
            m_key = std::end(*m_me);
        }
    }
};

struct post_t
{
    std::uint8_t		m_topdelta;
    std::uint8_t		m_length;
    std::uint8_t		m_pad_; //unused

    struct iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const std::uint8_t;
        using pointer = const std::uint8_t*;
        using reference = const std::uint8_t&;

        iterator(const std::uint8_t* ptr) : m_ptr{ ptr } {}

        reference operator*() const { return *operator->(); }
        pointer operator->() const { return m_ptr; }
        iterator& operator++()
        {
            ++m_ptr; return *this;
        }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; };
    private:

        const std::uint8_t* m_ptr;
    };


    iterator begin() const
    {
        return iterator(reinterpret_cast<const std::uint8_t*>(this + 1));
    }

    iterator end() const
    {
        return iterator(reinterpret_cast<const std::uint8_t*>(this + 1) + m_length);
    }
};

static_assert(std::has_unique_object_representations_v<post_t> && sizeof(post_t) == 3, "Bug");

struct patch_t;
struct column_t
{
    struct iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const post_t;
        using pointer = const post_t*;
        using reference = const post_t&;

        iterator(const post_t* ptr) : m_ptr{ ptr } {}

        reference operator*() const { return *operator->(); }
        pointer operator->() const { return m_ptr; }
        iterator& operator++()
        {
            if (m_ptr)
            {
                m_ptr = reinterpret_cast<const post_t*>(reinterpret_cast<const char*>(m_ptr) + m_ptr->m_length + 4);
                if (m_ptr->m_topdelta == std::numeric_limits<decltype(m_ptr->m_topdelta)>::max())
                    m_ptr = nullptr;
            }
            return *this;
        }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; };
    private:

        const post_t* m_ptr;
    };

    column_t() : m_ptr{ nullptr }, m_idx{ 0 }{}
    column_t(const patch_t* ptr, std::uint16_t idx) : m_ptr{ptr} , m_idx{idx}{}
    column_t& operator++() { ++m_idx; return *this; }

    operator bool() const {
        return m_ptr != nullptr;
    }

    friend bool operator== (const column_t& a, const column_t& b) { return a.m_ptr == b.m_ptr && a.m_idx == b.m_idx; };
    friend bool operator!= (const column_t& a, const column_t& b) { return a.m_ptr != b.m_ptr || a.m_idx != b.m_idx; };

    iterator begin() const;
    iterator end() const { return iterator(nullptr); }
private:

    const patch_t* m_ptr;
    std::uint16_t m_idx;
};

struct patch_t
{
    std::uint16_t		m_width;		// bounding box size 
    std::uint16_t		m_height;
    std::int16_t		m_leftoffset;	// pixels to the left of origin 
    std::int16_t		m_topoffset;	// pixels below the origin

    struct iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const column_t;
        using pointer = const column_t*;
        using reference = const column_t&;

        iterator(const patch_t* ptr, std::uint16_t idx) : m_column{ ptr, idx } {}

        reference operator*() const { return m_column; }
        pointer operator->() const { return &m_column; }
        iterator& operator++() {
            ++m_column;
            return *this;
        }
        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.m_column == b.m_column; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.m_column != b.m_column; };
    private:

        column_t m_column;
    };

    column_t operator[](std::uint16_t idx) const
    {
        if (idx >= m_width)
            throw std::runtime_error("Invalid patch number");
        return column_t(this, idx);
    }

    iterator begin() const { return iterator(this, 0); }
    iterator end() const { return iterator(this, m_width); }
};

static_assert(std::has_unique_object_representations_v<patch_t> && sizeof(patch_t) == 8, "Bug");

inline column_t::iterator column_t::begin() const { return iterator(reinterpret_cast<const post_t*>(reinterpret_cast<const char*>(m_ptr) + reinterpret_cast<const std::uint32_t*>(m_ptr + 1)[m_idx])); }

class composite_patch_t
{
#pragma pack(push, 1)
    struct composite_patch_head_t
    {
        patch_t m_patch;
        std::uint32_t m_post_offset;
        post_t m_post;

        composite_patch_head_t(std::uint8_t datasize)
            : m_patch{ 1, datasize, 0, 0 }
            , m_post_offset{ sizeof(patch_t) + sizeof(std::uint32_t) }
            , m_post{ 0, datasize, 0 }
        {
        }
    };
#pragma pack(pop)
    static_assert(std::has_unique_object_representations_v<composite_patch_head_t> && sizeof(composite_patch_head_t) == 15, "Bug");

    struct composite_patch_tail_t
    {
        std::uint8_t m_pad{ 0 }; // 1
        post_t m_end_post{ 0, std::numeric_limits<std::uint8_t>::max(), 0 };
    };
    static_assert(std::has_unique_object_representations_v<composite_patch_tail_t> && sizeof(composite_patch_tail_t) == 4, "Bug");

    const patch_t* header() const { return reinterpret_cast<const patch_t*>(this); }

public:

    static void* operator new(std::size_t count, std::uint8_t data_size)
    {
        void *new_patch = ::operator new(sizeof(composite_patch_head_t) + data_size + sizeof(composite_patch_tail_t));
        composite_patch_head_t *head = reinterpret_cast<composite_patch_head_t*>(new_patch);
        new (head) composite_patch_head_t{ data_size };

        void* tail = reinterpret_cast<char*>(new_patch) + sizeof(composite_patch_t) + data_size;
        new (tail) composite_patch_tail_t{};

        return new_patch;
    }
    static void* operator new[](std::size_t count) = delete;

    static void operator delete(void* ptr, std::size_t sz)
    {
        ::operator delete(ptr, sizeof(composite_patch_head_t) + reinterpret_cast<const patch_t*>(ptr)->m_height + sizeof(composite_patch_tail_t));
    }
    static void operator delete[](void* ptr, std::size_t count) = delete;

    std::span<std::uint8_t> data()
    {
        return std::span{ reinterpret_cast<std::uint8_t *>(this) + sizeof(composite_patch_head_t), header()->m_height };
    }

    column_t column() const
    {
        return column_t{ header(), 0 };
    }
};

enum class skill_t : std::uint8_t
{
    sk_baby,
    sk_easy,
    sk_medium,
    sk_hard,
    sk_nightmare
};

#endif