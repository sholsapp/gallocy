#ifndef __STL_H__
#define __STL_H__

#include <limits>
#include <memory>

/**
 * A custom allocator class for STL containers.
 */
template <class T, class Allocator>
class STLAllocator : public Allocator {

  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;


    /**
     * Default constructor.
     */
    STLAllocator() {}

    /**
     * Default destructor.
     */
    ~STLAllocator() noexcept {}

    /**
     * Constructor with size.
     */
    STLAllocator(size_type size) noexcept {}

    /**
     * Copy constructor.
     */
    STLAllocator(STLAllocator const& other) noexcept {
      *this = other;
    }

    /**
     * Copy constructor.
     */
    template <typename U>
    inline STLAllocator(STLAllocator<U, Allocator> const& other) noexcept {
      *this = other;
    }

    /**
     * Copy.
     */
    STLAllocator& operator=(const STLAllocator& other) {
      this->_alloc = other._alloc;
      return *this;
    }

    /**
     * Copy.
     */
    template<typename U>
    STLAllocator& operator=(const STLAllocator<U, Allocator>& other) {
      this->_alloc = other._alloc;
      return *this;
    }

    /**
     * Move constructor.
     */
    STLAllocator(STLAllocator&&) noexcept {}

    /**
     * Move.
     */
    STLAllocator& operator=(STLAllocator&& other) noexcept {
      this->_alloc = other._alloc;
      return *this;
    }

    /**
     * Rebind allocator to other type ``U``.
     */
    template <class U> struct rebind {
      typedef STLAllocator<U, Allocator> other;
    };

    /**
     * Return address of values.
     */
    pointer address(reference value) const {
      return &value;
    }

    /**
     * Return constant address of values.
     */
    const_pointer address(const_reference value) const {
       return &value;
    }

    /**
     * Return maximum number of elements that can be allocated.
     */
    size_type max_size() const noexcept {
      return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    /**
     * Allocate but don't initialize num elements of type T.
     */
    pointer allocate(size_type num, const void* = 0) {
      pointer ret = (pointer)(Allocator::malloc(num*sizeof(T)));
      return ret;
    }

    /**
     * Construct elements of allocated storage p with value value.
     */
    void construct(pointer p, const T& value) {
      new ((void*) p) T(value);
    }

    /**
     * Construct element of allocated storage p with no value.
     */
    template<typename U, typename... Args>
    void construct(U* p, Args&&... args) {
      new ((void*)p) U(std::forward<Args>(args)...);
    }

    /**
     * Destroy elements of initialized storage p.
     */
    void destroy(pointer p) {
      p->~T();
    }

    /**
     * Destroy elements of initialized storage p.
     */
    template<typename U>
    void destroy(U* p) {
        p->~U();
    }

    /**
     * Deallocate storage p of deleted elements.
     */
    void deallocate(pointer p, size_type num) {
      Allocator::free((void*)p);
    }

  private:

    Allocator _alloc;
};

template <class T1, class A1, class T2, class A2>
bool operator== (const STLAllocator<T1, A1>&,
    const STLAllocator<T2, A2>&) throw() {
  return true;
}

template <class T1, class A1, class T2, class A2>
bool operator!= (const STLAllocator<T1, A1>&,
    const STLAllocator<T2, A2>&) throw() {
  return false;
}

#endif
