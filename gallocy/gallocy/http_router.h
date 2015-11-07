#ifndef GALLOCY_HTTP_ROUTER_H_
#define GALLOCY_HTTP_ROUTER_H_

#include <map>
#include <vector>

#include "gallocy/stringutils.h"
#include "gallocy/objutils.h"


template <typename HandlerFunction = int>
class RoutingTable {
 public:
  /**
   * Internal data structure to maintain a route tree.
   */
  class Node {
   public:
    Node() {
      size_t s = sizeof(gallocy::map<gallocy::string, Node *>);
      routes = new (internal_malloc(s)) gallocy::map<gallocy::string, Node *>();
    }
    ~Node() {
      internal_free(routes);
    }
    HandlerFunction func;
    gallocy::string dynamic;
    gallocy::map<gallocy::string, Node *> *routes;
  };

 public:
  RoutingTable() {
    table = new (internal_malloc(sizeof(Node))) Node();
  }
  ~RoutingTable() {
    internal_free(table);
  }
  HandlerFunction match(gallocy::string path);
  void dump_table();
  void register_handler(gallocy::string route, HandlerFunction func);

 private:
  Node *table;
};


/**
 * Help get map values from a pointer to a map.
 */
template<typename K, typename V>
V _get_item(gallocy::map<K, V> *Map, K key) {
  auto it = Map->find(key);
  if (it != Map->end())
    return it->second;
  else
    return nullptr;
}


/**
 * Dump the routing table.
 */
template <typename HandlerFunction>
void RoutingTable<HandlerFunction>::dump_table() {
  gallocy::vector<Node *> queue;
  queue.push_back(table);
  while (queue.size() > 0) {
    Node *tmp = queue.at(0);
    std::cout << "Dynamic key: " << tmp->dynamic << std::endl;
    std::cout << "Handler: " << &tmp->func << std::endl;
    std::cout << "Table size: " << tmp->routes->size() << std::endl;;
    for (typename gallocy::map<gallocy::string, RoutingTable<HandlerFunction>::Node *>::iterator it
        = tmp->routes->begin(); it != tmp->routes->end(); ++it) {
      std::cout << "  " << it->first << std::endl;
      gallocy::string key = it->first;
      Node *next = it->second;
      queue.push_back(next);
    }
    queue.erase(queue.begin());
  }
}

/**
 * Register a handler.
 *
 * A route is a *theoretical* HTTP URI string that may have variables. A route
 * can look like any of the following:
 *
 *   /foo
 *   /foo/<param>
 *   /foo/<param>/bar
 *
 *  Where the strings surrounded by `<` and `>` characters are variables and
 *  will match any other string not containing special characters (including
 *  the `/` character).
 *
 * :param route: The route to register..
 * :param handler: The handler that should be associated with this route.
 */
template <typename HandlerFunction>
inline void RoutingTable<HandlerFunction>::register_handler(gallocy::string route, HandlerFunction handler) {
  gallocy::vector<gallocy::string> route_parts;
  utils::split(route, '/', route_parts);
  Node *tmp = table;
  for (auto part = std::begin(route_parts) + 1; part != std::end(route_parts); ++part) {
    if (utils::startswith(*part, "<") && utils::endswith(*part, ">"))
      tmp->dynamic = *part;
    if (!tmp->routes->count(*part))
      tmp->routes->insert(std::make_pair(*part, new (internal_malloc(sizeof(Node))) Node()));
    tmp = _get_item<gallocy::string, Node *>(tmp->routes, *part);
  }
  tmp->func = handler;
}



/**
 * Get the handler for a URI path.
 *
 * A path is a *realized* HTTP URI string, meaning all parts of the registered
 * route from :meth:`register_handler` are materialized. A path can look like
 * one of the following:
 *
 *   /foo
 *   /foo/1
 *   /foo/1/bar
 *
 * :param path: The path to look up the handler for.
 */
template <typename HandlerFunction>
inline HandlerFunction RoutingTable<HandlerFunction>::match(gallocy::string path) {
  gallocy::vector<gallocy::string> args;
  gallocy::vector<gallocy::string> path_parts;
  utils::split(path, '/', path_parts);
  Node *tmp = table;
  for (auto it = std::begin(path_parts) + 1; it != std::end(path_parts); ++it) {
    tmp = _get_item<gallocy::string, Node *>(tmp->routes, *it);
    // If this is the last element in the iterator, we're done searching for
    // the handler function, so break.
    if (std::distance(it, std::end(path_parts)) == 1)
      break;
    // If we encounter a dynamic route, save the path item as an argument to
    // the handler function, advance the iterator past the dyanmic argument,
    // and continue.
    if (tmp->dynamic.compare("") != 0) {
      tmp = _get_item<gallocy::string, Node *>(tmp->routes, tmp->dynamic);
      std::advance(it, 1);
      args.push_back(*it);
    }
  }
  return tmp->func;
}

#endif  // GALLOCY_HTTP_ROUTER_H_
