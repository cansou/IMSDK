// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: youme_check_online.proto

#ifndef PROTOBUF_youme_5fcheck_5fonline_2eproto__INCLUDED
#define PROTOBUF_youme_5fcheck_5fonline_2eproto__INCLUDED

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
void protobuf_AddDesc_youme_5fcheck_5fonline_2eproto();
void protobuf_AssignDesc_youme_5fcheck_5fonline_2eproto();
void protobuf_ShutdownFile_youme_5fcheck_5fonline_2eproto();

class CheckOnlineReq;
class CheckOnlineRsp;

// ===================================================================

class CheckOnlineReq : public ::youmecommon::protobuf::MessageLite {
 public:
  CheckOnlineReq();
  virtual ~CheckOnlineReq();

  CheckOnlineReq(const CheckOnlineReq& from);

  inline CheckOnlineReq& operator=(const CheckOnlineReq& from) {
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

  static const CheckOnlineReq& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const CheckOnlineReq* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(CheckOnlineReq* other);

  // implements Message ----------------------------------------------

  inline CheckOnlineReq* New() const { return New(NULL); }

  CheckOnlineReq* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const CheckOnlineReq& from);
  void MergeFrom(const CheckOnlineReq& from);
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
  void InternalSwap(CheckOnlineReq* other);
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

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.CheckOnlineReq)
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
  friend void  protobuf_AddDesc_youme_5fcheck_5fonline_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5fcheck_5fonline_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5fcheck_5fonline_2eproto();
  friend void protobuf_ShutdownFile_youme_5fcheck_5fonline_2eproto();

  void InitAsDefaultInstance();
  static CheckOnlineReq* default_instance_;
};
// -------------------------------------------------------------------

class CheckOnlineRsp : public ::youmecommon::protobuf::MessageLite {
 public:
  CheckOnlineRsp();
  virtual ~CheckOnlineRsp();

  CheckOnlineRsp(const CheckOnlineRsp& from);

  inline CheckOnlineRsp& operator=(const CheckOnlineRsp& from) {
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

  static const CheckOnlineRsp& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const CheckOnlineRsp* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(CheckOnlineRsp* other);

  // implements Message ----------------------------------------------

  inline CheckOnlineRsp* New() const { return New(NULL); }

  CheckOnlineRsp* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const CheckOnlineRsp& from);
  void MergeFrom(const CheckOnlineRsp& from);
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
  void InternalSwap(CheckOnlineRsp* other);
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

  // required int32 is_online = 3;
  bool has_is_online() const;
  void clear_is_online();
  static const int kIsOnlineFieldNumber = 3;
  ::youmecommon::protobuf::int32 is_online() const;
  void set_is_online(::youmecommon::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.CheckOnlineRsp)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_ret();
  inline void clear_has_ret();
  inline void set_has_is_online();
  inline void clear_has_is_online();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::int32 version_;
  ::youmecommon::protobuf::int32 ret_;
  ::youmecommon::protobuf::int32 is_online_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5fcheck_5fonline_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5fcheck_5fonline_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5fcheck_5fonline_2eproto();
  friend void protobuf_ShutdownFile_youme_5fcheck_5fonline_2eproto();

  void InitAsDefaultInstance();
  static CheckOnlineRsp* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// CheckOnlineReq

// required int32 version = 1;
inline bool CheckOnlineReq::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void CheckOnlineReq::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void CheckOnlineReq::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void CheckOnlineReq::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 CheckOnlineReq::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.CheckOnlineReq.version)
  return version_;
}
inline void CheckOnlineReq::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.CheckOnlineReq.version)
}

// required bytes user_id = 2;
inline bool CheckOnlineReq::has_user_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void CheckOnlineReq::set_has_user_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void CheckOnlineReq::clear_has_user_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void CheckOnlineReq::clear_user_id() {
  user_id_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_user_id();
}
inline const ::std::string& CheckOnlineReq::user_id() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.CheckOnlineReq.user_id)
  return user_id_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void CheckOnlineReq::set_user_id(const ::std::string& value) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.CheckOnlineReq.user_id)
}
inline void CheckOnlineReq::set_user_id(const char* value) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YOUMEServiceProtocol.CheckOnlineReq.user_id)
}
inline void CheckOnlineReq::set_user_id(const void* value, size_t size) {
  set_has_user_id();
  user_id_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YOUMEServiceProtocol.CheckOnlineReq.user_id)
}
inline ::std::string* CheckOnlineReq::mutable_user_id() {
  set_has_user_id();
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.CheckOnlineReq.user_id)
  return user_id_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* CheckOnlineReq::release_user_id() {
  clear_has_user_id();
  return user_id_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void CheckOnlineReq::set_allocated_user_id(::std::string* user_id) {
  if (user_id != NULL) {
    set_has_user_id();
  } else {
    clear_has_user_id();
  }
  user_id_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), user_id);
  // @@protoc_insertion_point(field_set_allocated:YOUMEServiceProtocol.CheckOnlineReq.user_id)
}

// -------------------------------------------------------------------

// CheckOnlineRsp

// required int32 version = 1;
inline bool CheckOnlineRsp::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void CheckOnlineRsp::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void CheckOnlineRsp::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void CheckOnlineRsp::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 CheckOnlineRsp::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.CheckOnlineRsp.version)
  return version_;
}
inline void CheckOnlineRsp::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.CheckOnlineRsp.version)
}

// required int32 ret = 2;
inline bool CheckOnlineRsp::has_ret() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void CheckOnlineRsp::set_has_ret() {
  _has_bits_[0] |= 0x00000002u;
}
inline void CheckOnlineRsp::clear_has_ret() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void CheckOnlineRsp::clear_ret() {
  ret_ = 0;
  clear_has_ret();
}
inline ::youmecommon::protobuf::int32 CheckOnlineRsp::ret() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.CheckOnlineRsp.ret)
  return ret_;
}
inline void CheckOnlineRsp::set_ret(::youmecommon::protobuf::int32 value) {
  set_has_ret();
  ret_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.CheckOnlineRsp.ret)
}

// required int32 is_online = 3;
inline bool CheckOnlineRsp::has_is_online() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void CheckOnlineRsp::set_has_is_online() {
  _has_bits_[0] |= 0x00000004u;
}
inline void CheckOnlineRsp::clear_has_is_online() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void CheckOnlineRsp::clear_is_online() {
  is_online_ = 0;
  clear_has_is_online();
}
inline ::youmecommon::protobuf::int32 CheckOnlineRsp::is_online() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.CheckOnlineRsp.is_online)
  return is_online_;
}
inline void CheckOnlineRsp::set_is_online(::youmecommon::protobuf::int32 value) {
  set_has_is_online();
  is_online_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.CheckOnlineRsp.is_online)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace YOUMEServiceProtocol

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_youme_5fcheck_5fonline_2eproto__INCLUDED
