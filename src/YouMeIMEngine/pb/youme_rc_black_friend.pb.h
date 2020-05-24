// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: youme_rc_black_friend.proto

#ifndef PROTOBUF_youme_5frc_5fblack_5ffriend_2eproto__INCLUDED
#define PROTOBUF_youme_5frc_5fblack_5ffriend_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
// @@protoc_insertion_point(includes)

namespace YOUMEServiceProtocol {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto();
void protobuf_AssignDesc_youme_5frc_5fblack_5ffriend_2eproto();
void protobuf_ShutdownFile_youme_5frc_5fblack_5ffriend_2eproto();

class BlackRcFriendReq;
class BlackRcFriendRsp;
class UnblackRcFriendReq;
class UnblackRcFriendRsp;

// ===================================================================

class BlackRcFriendReq : public ::youmecommon::protobuf::MessageLite {
 public:
  BlackRcFriendReq();
  virtual ~BlackRcFriendReq();

  BlackRcFriendReq(const BlackRcFriendReq& from);

  inline BlackRcFriendReq& operator=(const BlackRcFriendReq& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_.GetNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  inline ::std::string* mutable_unknown_fields() {
    return _unknown_fields_.MutableNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  static const BlackRcFriendReq& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const BlackRcFriendReq* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(BlackRcFriendReq* other);

  // implements Message ----------------------------------------------

  inline BlackRcFriendReq* New() const { return New(NULL); }

  BlackRcFriendReq* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const BlackRcFriendReq& from);
  void MergeFrom(const BlackRcFriendReq& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::youmecommon::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::youmecommon::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(BlackRcFriendReq* other);
  private:
  inline ::youmecommon::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::youmecommon::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 version = 1;
  bool has_version() const;
  void clear_version();
  static const int kVersionFieldNumber = 1;
  ::youmecommon::protobuf::int32 version() const;
  void set_version(::youmecommon::protobuf::int32 value);

  // required bytes user_id = 2;
  bool has_user_id() const;
  void clear_user_id();
  static const int kUserIdFieldNumber = 2;
  const ::std::string& user_id() const;
  void set_user_id(const ::std::string& value);
  void set_user_id(const char* value);
  void set_user_id(const void* value, size_t size);
  ::std::string* mutable_user_id();
  ::std::string* release_user_id();
  void set_allocated_user_id(::std::string* user_id);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.BlackRcFriendReq)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_user_id();
  inline void clear_has_user_id();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::internal::ArenaStringPtr user_id_;
  ::youmecommon::protobuf::int32 version_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5frc_5fblack_5ffriend_2eproto();
  friend void protobuf_ShutdownFile_youme_5frc_5fblack_5ffriend_2eproto();

  void InitAsDefaultInstance();
  static BlackRcFriendReq* default_instance_;
};
// -------------------------------------------------------------------

class BlackRcFriendRsp : public ::youmecommon::protobuf::MessageLite {
 public:
  BlackRcFriendRsp();
  virtual ~BlackRcFriendRsp();

  BlackRcFriendRsp(const BlackRcFriendRsp& from);

  inline BlackRcFriendRsp& operator=(const BlackRcFriendRsp& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_.GetNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  inline ::std::string* mutable_unknown_fields() {
    return _unknown_fields_.MutableNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  static const BlackRcFriendRsp& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const BlackRcFriendRsp* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(BlackRcFriendRsp* other);

  // implements Message ----------------------------------------------

  inline BlackRcFriendRsp* New() const { return New(NULL); }

  BlackRcFriendRsp* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const BlackRcFriendRsp& from);
  void MergeFrom(const BlackRcFriendRsp& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::youmecommon::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::youmecommon::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(BlackRcFriendRsp* other);
  private:
  inline ::youmecommon::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::youmecommon::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 version = 1;
  bool has_version() const;
  void clear_version();
  static const int kVersionFieldNumber = 1;
  ::youmecommon::protobuf::int32 version() const;
  void set_version(::youmecommon::protobuf::int32 value);

  // required int32 ret = 2;
  bool has_ret() const;
  void clear_ret();
  static const int kRetFieldNumber = 2;
  ::youmecommon::protobuf::int32 ret() const;
  void set_ret(::youmecommon::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.BlackRcFriendRsp)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_ret();
  inline void clear_has_ret();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::int32 version_;
  ::youmecommon::protobuf::int32 ret_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5frc_5fblack_5ffriend_2eproto();
  friend void protobuf_ShutdownFile_youme_5frc_5fblack_5ffriend_2eproto();

  void InitAsDefaultInstance();
  static BlackRcFriendRsp* default_instance_;
};
// -------------------------------------------------------------------

class UnblackRcFriendReq : public ::youmecommon::protobuf::MessageLite {
 public:
  UnblackRcFriendReq();
  virtual ~UnblackRcFriendReq();

  UnblackRcFriendReq(const UnblackRcFriendReq& from);

  inline UnblackRcFriendReq& operator=(const UnblackRcFriendReq& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_.GetNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  inline ::std::string* mutable_unknown_fields() {
    return _unknown_fields_.MutableNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  static const UnblackRcFriendReq& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const UnblackRcFriendReq* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(UnblackRcFriendReq* other);

  // implements Message ----------------------------------------------

  inline UnblackRcFriendReq* New() const { return New(NULL); }

  UnblackRcFriendReq* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const UnblackRcFriendReq& from);
  void MergeFrom(const UnblackRcFriendReq& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::youmecommon::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::youmecommon::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(UnblackRcFriendReq* other);
  private:
  inline ::youmecommon::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::youmecommon::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 version = 1;
  bool has_version() const;
  void clear_version();
  static const int kVersionFieldNumber = 1;
  ::youmecommon::protobuf::int32 version() const;
  void set_version(::youmecommon::protobuf::int32 value);

  // required bytes user_id = 2;
  bool has_user_id() const;
  void clear_user_id();
  static const int kUserIdFieldNumber = 2;
  const ::std::string& user_id() const;
  void set_user_id(const ::std::string& value);
  void set_user_id(const char* value);
  void set_user_id(const void* value, size_t size);
  ::std::string* mutable_user_id();
  ::std::string* release_user_id();
  void set_allocated_user_id(::std::string* user_id);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.UnblackRcFriendReq)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_user_id();
  inline void clear_has_user_id();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::internal::ArenaStringPtr user_id_;
  ::youmecommon::protobuf::int32 version_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5frc_5fblack_5ffriend_2eproto();
  friend void protobuf_ShutdownFile_youme_5frc_5fblack_5ffriend_2eproto();

  void InitAsDefaultInstance();
  static UnblackRcFriendReq* default_instance_;
};
// -------------------------------------------------------------------

class UnblackRcFriendRsp : public ::youmecommon::protobuf::MessageLite {
 public:
  UnblackRcFriendRsp();
  virtual ~UnblackRcFriendRsp();

  UnblackRcFriendRsp(const UnblackRcFriendRsp& from);

  inline UnblackRcFriendRsp& operator=(const UnblackRcFriendRsp& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::std::string& unknown_fields() const {
    return _unknown_fields_.GetNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  inline ::std::string* mutable_unknown_fields() {
    return _unknown_fields_.MutableNoArena(
        &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  }

  static const UnblackRcFriendRsp& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const UnblackRcFriendRsp* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(UnblackRcFriendRsp* other);

  // implements Message ----------------------------------------------

  inline UnblackRcFriendRsp* New() const { return New(NULL); }

  UnblackRcFriendRsp* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const UnblackRcFriendRsp& from);
  void MergeFrom(const UnblackRcFriendRsp& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::youmecommon::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::youmecommon::protobuf::io::CodedOutputStream* output) const;
  void DiscardUnknownFields();
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(UnblackRcFriendRsp* other);
  private:
  inline ::youmecommon::protobuf::Arena* GetArenaNoVirtual() const {
    return _arena_ptr_;
  }
  inline ::youmecommon::protobuf::Arena* MaybeArenaPtr() const {
    return _arena_ptr_;
  }
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required int32 version = 1;
  bool has_version() const;
  void clear_version();
  static const int kVersionFieldNumber = 1;
  ::youmecommon::protobuf::int32 version() const;
  void set_version(::youmecommon::protobuf::int32 value);

  // required int32 ret = 2;
  bool has_ret() const;
  void clear_ret();
  static const int kRetFieldNumber = 2;
  ::youmecommon::protobuf::int32 ret() const;
  void set_ret(::youmecommon::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.UnblackRcFriendRsp)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_ret();
  inline void clear_has_ret();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::int32 version_;
  ::youmecommon::protobuf::int32 ret_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5frc_5fblack_5ffriend_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5frc_5fblack_5ffriend_2eproto();
  friend void protobuf_ShutdownFile_youme_5frc_5fblack_5ffriend_2eproto();

  void InitAsDefaultInstance();
  static UnblackRcFriendRsp* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// BlackRcFriendReq

// required int32 version = 1;
inline bool BlackRcFriendReq::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void BlackRcFriendReq::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void BlackRcFriendReq::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void BlackRcFriendReq::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 BlackRcFriendReq::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.BlackRcFriendReq.version)
  return version_;
}
inline void BlackRcFriendReq::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.BlackRcFriendReq.version)
}

// required bytes user_id = 2;
inline bool BlackRcFriendReq::has_user_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void BlackRcFriendReq::set_has_user_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void BlackRcFriendReq::clear_has_user_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void BlackRcFriendReq::clear_user_id() {
  user_id_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_user_id();
}
inline const ::std::string& BlackRcFriendReq::user_id() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.BlackRcFriendReq.user_id)
  return user_id_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void BlackRcFriendReq::set_user_id(const ::std::string& value) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.BlackRcFriendReq.user_id)
}
inline void BlackRcFriendReq::set_user_id(const char* value) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YOUMEServiceProtocol.BlackRcFriendReq.user_id)
}
inline void BlackRcFriendReq::set_user_id(const void* value, size_t size) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YOUMEServiceProtocol.BlackRcFriendReq.user_id)
}
inline ::std::string* BlackRcFriendReq::mutable_user_id() {
  set_has_user_id();
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.BlackRcFriendReq.user_id)
  return user_id_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* BlackRcFriendReq::release_user_id() {
  clear_has_user_id();
  return user_id_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void BlackRcFriendReq::set_allocated_user_id(::std::string* user_id) {
  if (user_id != NULL) {
    set_has_user_id();
  } else {
    clear_has_user_id();
  }
  user_id_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), user_id);
  // @@protoc_insertion_point(field_set_allocated:YOUMEServiceProtocol.BlackRcFriendReq.user_id)
}

// -------------------------------------------------------------------

// BlackRcFriendRsp

// required int32 version = 1;
inline bool BlackRcFriendRsp::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void BlackRcFriendRsp::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void BlackRcFriendRsp::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void BlackRcFriendRsp::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 BlackRcFriendRsp::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.BlackRcFriendRsp.version)
  return version_;
}
inline void BlackRcFriendRsp::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.BlackRcFriendRsp.version)
}

// required int32 ret = 2;
inline bool BlackRcFriendRsp::has_ret() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void BlackRcFriendRsp::set_has_ret() {
  _has_bits_[0] |= 0x00000002u;
}
inline void BlackRcFriendRsp::clear_has_ret() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void BlackRcFriendRsp::clear_ret() {
  ret_ = 0;
  clear_has_ret();
}
inline ::youmecommon::protobuf::int32 BlackRcFriendRsp::ret() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.BlackRcFriendRsp.ret)
  return ret_;
}
inline void BlackRcFriendRsp::set_ret(::youmecommon::protobuf::int32 value) {
  set_has_ret();
  ret_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.BlackRcFriendRsp.ret)
}

// -------------------------------------------------------------------

// UnblackRcFriendReq

// required int32 version = 1;
inline bool UnblackRcFriendReq::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UnblackRcFriendReq::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UnblackRcFriendReq::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UnblackRcFriendReq::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 UnblackRcFriendReq::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.UnblackRcFriendReq.version)
  return version_;
}
inline void UnblackRcFriendReq::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.UnblackRcFriendReq.version)
}

// required bytes user_id = 2;
inline bool UnblackRcFriendReq::has_user_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UnblackRcFriendReq::set_has_user_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UnblackRcFriendReq::clear_has_user_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UnblackRcFriendReq::clear_user_id() {
  user_id_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_user_id();
}
inline const ::std::string& UnblackRcFriendReq::user_id() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.UnblackRcFriendReq.user_id)
  return user_id_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UnblackRcFriendReq::set_user_id(const ::std::string& value) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.UnblackRcFriendReq.user_id)
}
inline void UnblackRcFriendReq::set_user_id(const char* value) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YOUMEServiceProtocol.UnblackRcFriendReq.user_id)
}
inline void UnblackRcFriendReq::set_user_id(const void* value, size_t size) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YOUMEServiceProtocol.UnblackRcFriendReq.user_id)
}
inline ::std::string* UnblackRcFriendReq::mutable_user_id() {
  set_has_user_id();
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.UnblackRcFriendReq.user_id)
  return user_id_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* UnblackRcFriendReq::release_user_id() {
  clear_has_user_id();
  return user_id_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void UnblackRcFriendReq::set_allocated_user_id(::std::string* user_id) {
  if (user_id != NULL) {
    set_has_user_id();
  } else {
    clear_has_user_id();
  }
  user_id_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), user_id);
  // @@protoc_insertion_point(field_set_allocated:YOUMEServiceProtocol.UnblackRcFriendReq.user_id)
}

// -------------------------------------------------------------------

// UnblackRcFriendRsp

// required int32 version = 1;
inline bool UnblackRcFriendRsp::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UnblackRcFriendRsp::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UnblackRcFriendRsp::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UnblackRcFriendRsp::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 UnblackRcFriendRsp::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.UnblackRcFriendRsp.version)
  return version_;
}
inline void UnblackRcFriendRsp::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.UnblackRcFriendRsp.version)
}

// required int32 ret = 2;
inline bool UnblackRcFriendRsp::has_ret() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UnblackRcFriendRsp::set_has_ret() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UnblackRcFriendRsp::clear_has_ret() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UnblackRcFriendRsp::clear_ret() {
  ret_ = 0;
  clear_has_ret();
}
inline ::youmecommon::protobuf::int32 UnblackRcFriendRsp::ret() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.UnblackRcFriendRsp.ret)
  return ret_;
}
inline void UnblackRcFriendRsp::set_ret(::youmecommon::protobuf::int32 value) {
  set_has_ret();
  ret_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.UnblackRcFriendRsp.ret)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace YOUMEServiceProtocol

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_youme_5frc_5fblack_5ffriend_2eproto__INCLUDED