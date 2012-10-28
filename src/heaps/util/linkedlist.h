class LinkedList {
  public:
    class Node {
      public:
        Node(void) :
          next(NULL)
        {}
        Node* next;
      private:
        Node(const Node&);
        //Node& operator=(const Node&);
    };

    inline void clear(void) {
      head.next = NULL;
    }

    inline Node* remove(void) {
      const Node* e = head.next;
      if (e == NULL) {
        return NULL;
      }
      head.next = e->next;
      return const_cast<Node*>(e);
    }

    inline void insert(void* e) {
      Node* node = reinterpret_cast<Node*>(e);
      node->next = head.next;
      head.next = node;
    }

  private:
    Node head;
};
