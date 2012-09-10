#include <limits>

/**
 * A custom allocator class for STL containers.
 */
template <class T, class Allocator> class STLAllocator {
  public:
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    STLAllocator() throw() {
      _alloc = Allocator();
    }

    STLAllocator(const STLAllocator&) throw() {}

    template <class U> STLAllocator (const STLAllocator<U, Allocator>&) throw() {}

    ~STLAllocator() throw() {}

    /**
     * Rebind allocator to type U
     */
    template <class U> struct rebind {
      typedef STLAllocator<U, Allocator> other;
    };

    /**
     * Return address of values
     */
    pointer address (reference value) const {
      return &value;
    }
    const_pointer address (const_reference value) const {
       return &value;
    }

    /**
     * Return maximum number of elements that can be allocated.
     */
    size_type max_size () const throw() {
      return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    /**
     * Allocate but don't initialize num elements of type T.
     */
    pointer allocate (size_type num, const void* = 0) {
      pointer ret = (pointer)(_alloc.malloc(num*sizeof(T)));
      return ret;
    }

    /**
     * Initialize elements of allocated storage p with value value.
     */
    void construct (pointer p, const T& value) {
      new((void*)p)T(value);
    }

    /**
     * Destroy elements of initialized storage p.
     */
    void destroy (pointer p) {
      p->~T();
    }

    /**
     * Deallocate storage p of deleted elements.
     */
    void deallocate (pointer p, size_type num) {
      _alloc.free((void*)p);
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
