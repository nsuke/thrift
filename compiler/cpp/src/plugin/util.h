/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

// Get rid of this once C++11 is introduced to compiler code

#ifndef T_PLUGIN_UTIL_H
#define T_PLUGIN_UTIL_H

#include <boost/type_traits.hpp>
#include <boost/mem_fn.hpp>

template <typename K, typename V>
struct PairTransform {
  typedef typename boost::remove_pointer<K>::type K0;
  typedef typename boost::remove_pointer<V>::type V0;
  typedef typename boost::function_traits<K0>::arg1_type K1;
  typedef typename boost::function_traits<V0>::arg1_type V1;
  typedef typename boost::function_traits<K0>::result_type K2;
  typedef typename boost::function_traits<V0>::result_type V2;
  PairTransform(K k, V v) : k_(k), v_(v) {}
  K k_;
  V v_;
  std::pair<K2, V2> operator()(const std::pair<K1, V1>& from) const {
    return std::make_pair(k_(from.first), v_(from.second));
  }
};
template <typename K, typename V>
PairTransform<K, V> pair_transform(K k, V v) {
  return PairTransform<K, V>(k, v);
}

template <typename T, typename K, typename V>
struct BinarySetter {
  typedef void (T::*F)(K, V);
  T* t_;
  F f_;
  BinarySetter(T* t, F f) : t_(t), f_(f) {}
  void operator()(const std::pair<K, V>& p) const { boost::mem_fn(f_)(t_, p.first, p.second); }
};
template <typename T, typename K, typename V>
BinarySetter<T, K, V> binary_setter(T* t, void (T::*f)(K, V)) {
  return BinarySetter<T, K, V>(t, f);
}

#endif
