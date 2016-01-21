#ifndef GALLOCY_HEAPLAYERS_ADDHEAP_H_
#define GALLOCY_HEAPLAYERS_ADDHEAP_H_


namespace HL {

/**
 * Reserve space for a class in the head of every allocated object.
 */
template <class Add, class Super>
class AddHeap : public Super {
 public:
  inline void *malloc(size_t sz) {
    void *ptr = Super::malloc(sz + align(sizeof(Add)));
    void *newPtr = reinterpret_cast<void *>(align(reinterpret_cast<size_t>(reinterpret_cast<Add *>(ptr) + 1)));
    return newPtr;
  }

  inline void free(void *ptr) {
    void *origPtr = reinterpret_cast<void *>(reinterpret_cast<Add *>(ptr) - 1);
    Super::free(origPtr);
  }

  inline size_t getSize(void *ptr) {
    void *origPtr = reinterpret_cast<void *>(reinterpret_cast<Add *>(ptr) - 1);
    return Super::getSize(origPtr);
  }

 private:
  static inline size_t align(size_t sz) {
    return (sz + (sizeof(double) - 1)) & ~(sizeof(double) - 1);
  }
};

}  // namespace HL

#endif  // GALLOCY_HEAPLAYERS_ADDHEAP_H_
