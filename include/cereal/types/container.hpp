/*! \file container.hpp
    \brief Support for general container types
    \ingroup STLSupport */
/*
  Copyright (c) 2019, Michael Walz
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES OR SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef CEREAL_TYPES_CONTAINER_HPP_
#define CEREAL_TYPES_CONTAINER_HPP_

#include "cereal/cereal.hpp"
#include "cereal/types/memory.hpp"

namespace cereal {

//! Helper class to check if a container has a 'reserve'-member function
template <typename Container>
struct has_container_reserve {
    template <class T>
    inline static std::false_type
    has_container_reserve_impl(...);

    template <class T>
    inline static typename std::enable_if<std::is_member_function_pointer<decltype(&T::reserve)>::value, std::true_type>::type
        has_container_reserve_impl(std::nullptr_t);

    static const bool value = decltype(has_container_reserve_impl<Container>(nullptr))::value;
};

//! Loading container<T>, case when user provides load_and_construct for T
/*! Loading relies on push_back and clear and only requires type T to be movable.
 */
template <class Archive, class Container>
inline decltype(
    std::declval<Container>().push_back(std::declval<typename Container::value_type>()),
    std::declval<Container>().clear(),
    typename std::enable_if<traits::has_load_and_construct<typename Container::value_type, Archive>::value, void>::type())
CEREAL_LOAD_FUNCTION_NAME(Archive& ar, Container& container)
{
    size_type size;
    ar(make_size_tag(size));

    container.clear();

#ifdef CEREAL_HAS_CPP17
    if constexpr (has_container_reserve<Container>::value) {
        container.reserve(static_cast<std::size_t>(size));
    }
#endif

    for (size_type n = 0; n < size; n++) {
        container.push_back(cereal::load_and_construct<typename Container::value_type>(ar));
    }
}

}

#endif // CEREAL_TYPES_CONTAINER_HPP_
