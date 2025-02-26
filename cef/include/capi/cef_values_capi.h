// Copyright (c) 2019 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//
// $hash=cc8995c8ae5c1336941b95770f8a07dc273fb2f1$
//

#ifndef CEF_INCLUDE_CAPI_CEF_VALUES_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_VALUES_CAPI_H_
#pragma once

#include "include/capi/cef_base_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _cef_binary_value_t;
struct _cef_dictionary_value_t;
struct _cef_list_value_t;

///
// Structure that wraps other data value types. Complex types (binary,
// dictionary and list) will be referenced but not owned by this object. Can be
// used on any process and thread.
///
typedef struct _cef_value_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if the underlying data is valid. This will always be true
  // (1) for simple types. For complex types (binary, dictionary and list) the
  // underlying data may become invalid if owned by another object (e.g. list or
  // dictionary) and that other object is then modified or destroyed. This value
  // object can be re-used by calling Set*() even if the underlying data is
  // invalid.
  ///
  int(CEF_CALLBACK* is_valid)(struct _cef_value_t* self);

  ///
  // Returns true (1) if the underlying data is owned by another object.
  ///
  int(CEF_CALLBACK* is_owned)(struct _cef_value_t* self);

  ///
  // Returns true (1) if the underlying data is read-only. Some APIs may expose
  // read-only objects.
  ///
  int(CEF_CALLBACK* is_read_only)(struct _cef_value_t* self);

  ///
  // Returns true (1) if this object and |that| object have the same underlying
  // data. If true (1) modifications to this object will also affect |that|
  // object and vice-versa.
  ///
  int(CEF_CALLBACK* is_same)(struct _cef_value_t* self,
                             struct _cef_value_t* that);

  ///
  // Returns true (1) if this object and |that| object have an equivalent
  // underlying value but are not necessarily the same object.
  ///
  int(CEF_CALLBACK* is_equal)(struct _cef_value_t* self,
                              struct _cef_value_t* that);

  ///
  // Returns a copy of this object. The underlying data will also be copied.
  ///
  struct _cef_value_t*(CEF_CALLBACK* copy)(struct _cef_value_t* self);

  ///
  // Returns the underlying value type.
  ///
  cef_value_type_t(CEF_CALLBACK* get_type)(struct _cef_value_t* self);

  ///
  // Returns the underlying value as type bool.
  ///
  int(CEF_CALLBACK* get_bool)(struct _cef_value_t* self);

  ///
  // Returns the underlying value as type int.
  ///
  int(CEF_CALLBACK* get_int)(struct _cef_value_t* self);

  ///
  // Returns the underlying value as type double.
  ///
  double(CEF_CALLBACK* get_double)(struct _cef_value_t* self);

  ///
  // Returns the underlying value as type string.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_string)(struct _cef_value_t* self);

  ///
  // Returns the underlying value as type binary. The returned reference may
  // become invalid if the value is owned by another object or if ownership is
  // transferred to another object in the future. To maintain a reference to the
  // value after assigning ownership to a dictionary or list pass this object to
  // the set_value() function instead of passing the returned reference to
  // set_binary().
  ///
  struct _cef_binary_value_t*(CEF_CALLBACK* get_binary)(
      struct _cef_value_t* self);

  ///
  // Returns the underlying value as type dictionary. The returned reference may
  // become invalid if the value is owned by another object or if ownership is
  // transferred to another object in the future. To maintain a reference to the
  // value after assigning ownership to a dictionary or list pass this object to
  // the set_value() function instead of passing the returned reference to
  // set_dictionary().
  ///
  struct _cef_dictionary_value_t*(CEF_CALLBACK* get_dictionary)(
      struct _cef_value_t* self);

  ///
  // Returns the underlying value as type list. The returned reference may
  // become invalid if the value is owned by another object or if ownership is
  // transferred to another object in the future. To maintain a reference to the
  // value after assigning ownership to a dictionary or list pass this object to
  // the set_value() function instead of passing the returned reference to
  // set_list().
  ///
  struct _cef_list_value_t*(CEF_CALLBACK* get_list)(struct _cef_value_t* self);

  ///
  // Sets the underlying value as type null. Returns true (1) if the value was
  // set successfully.
  ///
  int(CEF_CALLBACK* set_null)(struct _cef_value_t* self);

  ///
  // Sets the underlying value as type bool. Returns true (1) if the value was
  // set successfully.
  ///
  int(CEF_CALLBACK* set_bool)(struct _cef_value_t* self, int value);

  ///
  // Sets the underlying value as type int. Returns true (1) if the value was
  // set successfully.
  ///
  int(CEF_CALLBACK* set_int)(struct _cef_value_t* self, int value);

  ///
  // Sets the underlying value as type double. Returns true (1) if the value was
  // set successfully.
  ///
  int(CEF_CALLBACK* set_double)(struct _cef_value_t* self, double value);

  ///
  // Sets the underlying value as type string. Returns true (1) if the value was
  // set successfully.
  ///
  int(CEF_CALLBACK* set_string)(struct _cef_value_t* self,
                                const cef_string_t* value);

  ///
  // Sets the underlying value as type binary. Returns true (1) if the value was
  // set successfully. This object keeps a reference to |value| and ownership of
  // the underlying data remains unchanged.
  ///
  int(CEF_CALLBACK* set_binary)(struct _cef_value_t* self,
                                struct _cef_binary_value_t* value);

  ///
  // Sets the underlying value as type dict. Returns true (1) if the value was
  // set successfully. This object keeps a reference to |value| and ownership of
  // the underlying data remains unchanged.
  ///
  int(CEF_CALLBACK* set_dictionary)(struct _cef_value_t* self,
                                    struct _cef_dictionary_value_t* value);

  ///
  // Sets the underlying value as type list. Returns true (1) if the value was
  // set successfully. This object keeps a reference to |value| and ownership of
  // the underlying data remains unchanged.
  ///
  int(CEF_CALLBACK* set_list)(struct _cef_value_t* self,
                              struct _cef_list_value_t* value);
} cef_value_t;

///
// Creates a new object.
///
CEF_EXPORT cef_value_t* cef_value_create();

///
// Structure representing a binary value. Can be used on any process and thread.
///
typedef struct _cef_binary_value_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this object is valid. This object may become invalid if
  // the underlying data is owned by another object (e.g. list or dictionary)
  // and that other object is then modified or destroyed. Do not call any other
  // functions if this function returns false (0).
  ///
  int(CEF_CALLBACK* is_valid)(struct _cef_binary_value_t* self);

  ///
  // Returns true (1) if this object is currently owned by another object.
  ///
  int(CEF_CALLBACK* is_owned)(struct _cef_binary_value_t* self);

  ///
  // Returns true (1) if this object and |that| object have the same underlying
  // data.
  ///
  int(CEF_CALLBACK* is_same)(struct _cef_binary_value_t* self,
                             struct _cef_binary_value_t* that);

  ///
  // Returns true (1) if this object and |that| object have an equivalent
  // underlying value but are not necessarily the same object.
  ///
  int(CEF_CALLBACK* is_equal)(struct _cef_binary_value_t* self,
                              struct _cef_binary_value_t* that);

  ///
  // Returns a copy of this object. The data in this object will also be copied.
  ///
  struct _cef_binary_value_t*(CEF_CALLBACK* copy)(
      struct _cef_binary_value_t* self);

  ///
  // Returns the data size.
  ///
  size_t(CEF_CALLBACK* get_size)(struct _cef_binary_value_t* self);

  ///
  // Read up to |buffer_size| number of bytes into |buffer|. Reading begins at
  // the specified byte |data_offset|. Returns the number of bytes read.
  ///
  size_t(CEF_CALLBACK* get_data)(struct _cef_binary_value_t* self,
                                 void* buffer,
                                 size_t buffer_size,
                                 size_t data_offset);
} cef_binary_value_t;

///
// Creates a new object that is not owned by any other object. The specified
// |data| will be copied.
///
CEF_EXPORT cef_binary_value_t* cef_binary_value_create(const void* data,
                                                       size_t data_size);

///
// Structure representing a dictionary value. Can be used on any process and
// thread.
///
typedef struct _cef_dictionary_value_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this object is valid. This object may become invalid if
  // the underlying data is owned by another object (e.g. list or dictionary)
  // and that other object is then modified or destroyed. Do not call any other
  // functions if this function returns false (0).
  ///
  int(CEF_CALLBACK* is_valid)(struct _cef_dictionary_value_t* self);

  ///
  // Returns true (1) if this object is currently owned by another object.
  ///
  int(CEF_CALLBACK* is_owned)(struct _cef_dictionary_value_t* self);

  ///
  // Returns true (1) if the values of this object are read-only. Some APIs may
  // expose read-only objects.
  ///
  int(CEF_CALLBACK* is_read_only)(struct _cef_dictionary_value_t* self);

  ///
  // Returns true (1) if this object and |that| object have the same underlying
  // data. If true (1) modifications to this object will also affect |that|
  // object and vice-versa.
  ///
  int(CEF_CALLBACK* is_same)(struct _cef_dictionary_value_t* self,
                             struct _cef_dictionary_value_t* that);

  ///
  // Returns true (1) if this object and |that| object have an equivalent
  // underlying value but are not necessarily the same object.
  ///
  int(CEF_CALLBACK* is_equal)(struct _cef_dictionary_value_t* self,
                              struct _cef_dictionary_value_t* that);

  ///
  // Returns a writable copy of this object. If |exclude_NULL_children| is true
  // (1) any NULL dictionaries or lists will be excluded from the copy.
  ///
  struct _cef_dictionary_value_t*(CEF_CALLBACK* copy)(
      struct _cef_dictionary_value_t* self,
      int exclude_empty_children);

  ///
  // Returns the number of values.
  ///
  size_t(CEF_CALLBACK* get_size)(struct _cef_dictionary_value_t* self);

  ///
  // Removes all values. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* clear)(struct _cef_dictionary_value_t* self);

  ///
  // Returns true (1) if the current dictionary has a value for the given key.
  ///
  int(CEF_CALLBACK* has_key)(struct _cef_dictionary_value_t* self,
                             const cef_string_t* key);

  ///
  // Reads all keys for this dictionary into the specified vector.
  ///
  int(CEF_CALLBACK* get_keys)(struct _cef_dictionary_value_t* self,
                              cef_string_list_t keys);

  ///
  // Removes the value at the specified key. Returns true (1) is the value was
  // removed successfully.
  ///
  int(CEF_CALLBACK* remove)(struct _cef_dictionary_value_t* self,
                            const cef_string_t* key);

  ///
  // Returns the value type for the specified key.
  ///
  cef_value_type_t(CEF_CALLBACK* get_type)(struct _cef_dictionary_value_t* self,
                                           const cef_string_t* key);

  ///
  // Returns the value at the specified key. For simple types the returned value
  // will copy existing data and modifications to the value will not modify this
  // object. For complex types (binary, dictionary and list) the returned value
  // will reference existing data and modifications to the value will modify
  // this object.
  ///
  struct _cef_value_t*(CEF_CALLBACK* get_value)(
      struct _cef_dictionary_value_t* self,
      const cef_string_t* key);

  ///
  // Returns the value at the specified key as type bool.
  ///
  int(CEF_CALLBACK* get_bool)(struct _cef_dictionary_value_t* self,
                              const cef_string_t* key);

  ///
  // Returns the value at the specified key as type int.
  ///
  int(CEF_CALLBACK* get_int)(struct _cef_dictionary_value_t* self,
                             const cef_string_t* key);

  ///
  // Returns the value at the specified key as type double.
  ///
  double(CEF_CALLBACK* get_double)(struct _cef_dictionary_value_t* self,
                                   const cef_string_t* key);

  ///
  // Returns the value at the specified key as type string.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(CEF_CALLBACK* get_string)(
      struct _cef_dictionary_value_t* self,
      const cef_string_t* key);

  ///
  // Returns the value at the specified key as type binary. The returned value
  // will reference existing data.
  ///
  struct _cef_binary_value_t*(CEF_CALLBACK* get_binary)(
      struct _cef_dictionary_value_t* self,
      const cef_string_t* key);

  ///
  // Returns the value at the specified key as type dictionary. The returned
  // value will reference existing data and modifications to the value will
  // modify this object.
  ///
  struct _cef_dictionary_value_t*(CEF_CALLBACK* get_dictionary)(
      struct _cef_dictionary_value_t* self,
      const cef_string_t* key);

  ///
  // Returns the value at the specified key as type list. The returned value
  // will reference existing data and modifications to the value will modify
  // this object.
  ///
  struct _cef_list_value_t*(CEF_CALLBACK* get_list)(
      struct _cef_dictionary_value_t* self,
      const cef_string_t* key);

  ///
  // Sets the value at the specified key. Returns true (1) if the value was set
  // successfully. If |value| represents simple data then the underlying data
  // will be copied and modifications to |value| will not modify this object. If
  // |value| represents complex data (binary, dictionary or list) then the
  // underlying data will be referenced and modifications to |value| will modify
  // this object.
  ///
  int(CEF_CALLBACK* set_value)(struct _cef_dictionary_value_t* self,
                               const cef_string_t* key,
                               struct _cef_value_t* value);

  ///
  // Sets the value at the specified key as type null. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_null)(struct _cef_dictionary_value_t* self,
                              const cef_string_t* key);

  ///
  // Sets the value at the specified key as type bool. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_bool)(struct _cef_dictionary_value_t* self,
                              const cef_string_t* key,
                              int value);

  ///
  // Sets the value at the specified key as type int. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_int)(struct _cef_dictionary_value_t* self,
                             const cef_string_t* key,
                             int value);

  ///
  // Sets the value at the specified key as type double. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_double)(struct _cef_dictionary_value_t* self,
                                const cef_string_t* key,
                                double value);

  ///
  // Sets the value at the specified key as type string. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_string)(struct _cef_dictionary_value_t* self,
                                const cef_string_t* key,
                                const cef_string_t* value);

  ///
  // Sets the value at the specified key as type binary. Returns true (1) if the
  // value was set successfully. If |value| is currently owned by another object
  // then the value will be copied and the |value| reference will not change.
  // Otherwise, ownership will be transferred to this object and the |value|
  // reference will be invalidated.
  ///
  int(CEF_CALLBACK* set_binary)(struct _cef_dictionary_value_t* self,
                                const cef_string_t* key,
                                struct _cef_binary_value_t* value);

  ///
  // Sets the value at the specified key as type dict. Returns true (1) if the
  // value was set successfully. If |value| is currently owned by another object
  // then the value will be copied and the |value| reference will not change.
  // Otherwise, ownership will be transferred to this object and the |value|
  // reference will be invalidated.
  ///
  int(CEF_CALLBACK* set_dictionary)(struct _cef_dictionary_value_t* self,
                                    const cef_string_t* key,
                                    struct _cef_dictionary_value_t* value);

  ///
  // Sets the value at the specified key as type list. Returns true (1) if the
  // value was set successfully. If |value| is currently owned by another object
  // then the value will be copied and the |value| reference will not change.
  // Otherwise, ownership will be transferred to this object and the |value|
  // reference will be invalidated.
  ///
  int(CEF_CALLBACK* set_list)(struct _cef_dictionary_value_t* self,
                              const cef_string_t* key,
                              struct _cef_list_value_t* value);
} cef_dictionary_value_t;

///
// Creates a new object that is not owned by any other object.
///
CEF_EXPORT cef_dictionary_value_t* cef_dictionary_value_create();

///
// Structure representing a list value. Can be used on any process and thread.
///
typedef struct _cef_list_value_t {
  ///
  // Base structure.
  ///
  cef_base_ref_counted_t base;

  ///
  // Returns true (1) if this object is valid. This object may become invalid if
  // the underlying data is owned by another object (e.g. list or dictionary)
  // and that other object is then modified or destroyed. Do not call any other
  // functions if this function returns false (0).
  ///
  int(CEF_CALLBACK* is_valid)(struct _cef_list_value_t* self);

  ///
  // Returns true (1) if this object is currently owned by another object.
  ///
  int(CEF_CALLBACK* is_owned)(struct _cef_list_value_t* self);

  ///
  // Returns true (1) if the values of this object are read-only. Some APIs may
  // expose read-only objects.
  ///
  int(CEF_CALLBACK* is_read_only)(struct _cef_list_value_t* self);

  ///
  // Returns true (1) if this object and |that| object have the same underlying
  // data. If true (1) modifications to this object will also affect |that|
  // object and vice-versa.
  ///
  int(CEF_CALLBACK* is_same)(struct _cef_list_value_t* self,
                             struct _cef_list_value_t* that);

  ///
  // Returns true (1) if this object and |that| object have an equivalent
  // underlying value but are not necessarily the same object.
  ///
  int(CEF_CALLBACK* is_equal)(struct _cef_list_value_t* self,
                              struct _cef_list_value_t* that);

  ///
  // Returns a writable copy of this object.
  ///
  struct _cef_list_value_t*(CEF_CALLBACK* copy)(struct _cef_list_value_t* self);

  ///
  // Sets the number of values. If the number of values is expanded all new
  // value slots will default to type null. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* set_size)(struct _cef_list_value_t* self, size_t size);

  ///
  // Returns the number of values.
  ///
  size_t(CEF_CALLBACK* get_size)(struct _cef_list_value_t* self);

  ///
  // Removes all values. Returns true (1) on success.
  ///
  int(CEF_CALLBACK* clear)(struct _cef_list_value_t* self);

  ///
  // Removes the value at the specified index.
  ///
  int(CEF_CALLBACK* remove)(struct _cef_list_value_t* self, size_t index);

  ///
  // Returns the value type at the specified index.
  ///
  cef_value_type_t(CEF_CALLBACK* get_type)(struct _cef_list_value_t* self,
                                           size_t index);

  ///
  // Returns the value at the specified index. For simple types the returned
  // value will copy existing data and modifications to the value will not
  // modify this object. For complex types (binary, dictionary and list) the
  // returned value will reference existing data and modifications to the value
  // will modify this object.
  ///
  struct _cef_value_t*(CEF_CALLBACK* get_value)(struct _cef_list_value_t* self,
                                                size_t index);

  ///
  // Returns the value at the specified index as type bool.
  ///
  int(CEF_CALLBACK* get_bool)(struct _cef_list_value_t* self, size_t index);

  ///
  // Returns the value at the specified index as type int.
  ///
  int(CEF_CALLBACK* get_int)(struct _cef_list_value_t* self, size_t index);

  ///
  // Returns the value at the specified index as type double.
  ///
  double(CEF_CALLBACK* get_double)(struct _cef_list_value_t* self,
                                   size_t index);

  ///
  // Returns the value at the specified index as type string.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t(
      CEF_CALLBACK* get_string)(struct _cef_list_value_t* self, size_t index);

  ///
  // Returns the value at the specified index as type binary. The returned value
  // will reference existing data.
  ///
  struct _cef_binary_value_t*(
      CEF_CALLBACK* get_binary)(struct _cef_list_value_t* self, size_t index);

  ///
  // Returns the value at the specified index as type dictionary. The returned
  // value will reference existing data and modifications to the value will
  // modify this object.
  ///
  struct _cef_dictionary_value_t*(CEF_CALLBACK* get_dictionary)(
      struct _cef_list_value_t* self,
      size_t index);

  ///
  // Returns the value at the specified index as type list. The returned value
  // will reference existing data and modifications to the value will modify
  // this object.
  ///
  struct _cef_list_value_t*(
      CEF_CALLBACK* get_list)(struct _cef_list_value_t* self, size_t index);

  ///
  // Sets the value at the specified index. Returns true (1) if the value was
  // set successfully. If |value| represents simple data then the underlying
  // data will be copied and modifications to |value| will not modify this
  // object. If |value| represents complex data (binary, dictionary or list)
  // then the underlying data will be referenced and modifications to |value|
  // will modify this object.
  ///
  int(CEF_CALLBACK* set_value)(struct _cef_list_value_t* self,
                               size_t index,
                               struct _cef_value_t* value);

  ///
  // Sets the value at the specified index as type null. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_null)(struct _cef_list_value_t* self, size_t index);

  ///
  // Sets the value at the specified index as type bool. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_bool)(struct _cef_list_value_t* self,
                              size_t index,
                              int value);

  ///
  // Sets the value at the specified index as type int. Returns true (1) if the
  // value was set successfully.
  ///
  int(CEF_CALLBACK* set_int)(struct _cef_list_value_t* self,
                             size_t index,
                             int value);

  ///
  // Sets the value at the specified index as type double. Returns true (1) if
  // the value was set successfully.
  ///
  int(CEF_CALLBACK* set_double)(struct _cef_list_value_t* self,
                                size_t index,
                                double value);

  ///
  // Sets the value at the specified index as type string. Returns true (1) if
  // the value was set successfully.
  ///
  int(CEF_CALLBACK* set_string)(struct _cef_list_value_t* self,
                                size_t index,
                                const cef_string_t* value);

  ///
  // Sets the value at the specified index as type binary. Returns true (1) if
  // the value was set successfully. If |value| is currently owned by another
  // object then the value will be copied and the |value| reference will not
  // change. Otherwise, ownership will be transferred to this object and the
  // |value| reference will be invalidated.
  ///
  int(CEF_CALLBACK* set_binary)(struct _cef_list_value_t* self,
                                size_t index,
                                struct _cef_binary_value_t* value);

  ///
  // Sets the value at the specified index as type dict. Returns true (1) if the
  // value was set successfully. If |value| is currently owned by another object
  // then the value will be copied and the |value| reference will not change.
  // Otherwise, ownership will be transferred to this object and the |value|
  // reference will be invalidated.
  ///
  int(CEF_CALLBACK* set_dictionary)(struct _cef_list_value_t* self,
                                    size_t index,
                                    struct _cef_dictionary_value_t* value);

  ///
  // Sets the value at the specified index as type list. Returns true (1) if the
  // value was set successfully. If |value| is currently owned by another object
  // then the value will be copied and the |value| reference will not change.
  // Otherwise, ownership will be transferred to this object and the |value|
  // reference will be invalidated.
  ///
  int(CEF_CALLBACK* set_list)(struct _cef_list_value_t* self,
                              size_t index,
                              struct _cef_list_value_t* value);
} cef_list_value_t;

///
// Creates a new object that is not owned by any other object.
///
CEF_EXPORT cef_list_value_t* cef_list_value_create();

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_VALUES_CAPI_H_
