// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: youme_forbid_record.proto

#ifndef PROTOBUF_youme_5fforbid_5frecord_2eproto__INCLUDED
#define PROTOBUF_youme_5fforbid_5frecord_2eproto__INCLUDED

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
void protobuf_AddDesc_youme_5fforbid_5frecord_2eproto();
void protobuf_AssignDesc_youme_5fforbid_5frecord_2eproto();
void protobuf_ShutdownFile_youme_5fforbid_5frecord_2eproto();

class ForbidRecordReq;
class ForbidRecordRsp;
class YoumeForbidRecord;

// ===================================================================

class ForbidRecordReq : public ::youmecommon::protobuf::MessageLite {
 public:
  ForbidRecordReq();
  virtual ~ForbidRecordReq();

  ForbidRecordReq(const ForbidRecordReq& from);

  inline ForbidRecordReq& operator=(const ForbidRecordReq& from) {
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

  static const ForbidRecordReq& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const ForbidRecordReq* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(ForbidRecordReq* other);

  // implements Message ----------------------------------------------

  inline ForbidRecordReq* New() const { return New(NULL); }

  ForbidRecordReq* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const ForbidRecordReq& from);
  void MergeFrom(const ForbidRecordReq& from);
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
  void InternalSwap(ForbidRecordReq* other);
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

  // optional bytes roomid = 2;
  bool has_roomid() const;
  void clear_roomid();
  static const int kRoomidFieldNumber = 2;
  const ::std::string& roomid() const;
  void set_roomid(const ::std::string& value);
  void set_roomid(const char* value);
  void set_roomid(const void* value, size_t size);
  ::std::string* mutable_roomid();
  ::std::string* release_roomid();
  void set_allocated_roomid(::std::string* roomid);

  // optional int32 user_right = 3;
  bool has_user_right() const;
  void clear_user_right();
  static const int kUserRightFieldNumber = 3;
  ::youmecommon::protobuf::int32 user_right() const;
  void set_user_right(::youmecommon::protobuf::int32 value);

  // optional uint32 room_seq = 4;
  bool has_room_seq() const;
  void clear_room_seq();
  static const int kRoomSeqFieldNumber = 4;
  ::youmecommon::protobuf::uint32 room_seq() const;
  void set_room_seq(::youmecommon::protobuf::uint32 value);

  // optional uint32 limit = 5;
  bool has_limit() const;
  void clear_limit();
  static const int kLimitFieldNumber = 5;
  ::youmecommon::protobuf::uint32 limit() const;
  void set_limit(::youmecommon::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.ForbidRecordReq)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_roomid();
  inline void clear_has_roomid();
  inline void set_has_user_right();
  inline void clear_has_user_right();
  inline void set_has_room_seq();
  inline void clear_has_room_seq();
  inline void set_has_limit();
  inline void clear_has_limit();

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::internal::ArenaStringPtr roomid_;
  ::youmecommon::protobuf::int32 version_;
  ::youmecommon::protobuf::int32 user_right_;
  ::youmecommon::protobuf::uint32 room_seq_;
  ::youmecommon::protobuf::uint32 limit_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5fforbid_5frecord_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5fforbid_5frecord_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5fforbid_5frecord_2eproto();
  friend void protobuf_ShutdownFile_youme_5fforbid_5frecord_2eproto();

  void InitAsDefaultInstance();
  static ForbidRecordReq* default_instance_;
};
// -------------------------------------------------------------------

class YoumeForbidRecord : public ::youmecommon::protobuf::MessageLite {
 public:
  YoumeForbidRecord();
  virtual ~YoumeForbidRecord();

  YoumeForbidRecord(const YoumeForbidRecord& from);

  inline YoumeForbidRecord& operator=(const YoumeForbidRecord& from) {
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

  static const YoumeForbidRecord& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const YoumeForbidRecord* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(YoumeForbidRecord* other);

  // implements Message ----------------------------------------------

  inline YoumeForbidRecord* New() const { return New(NULL); }

  YoumeForbidRecord* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const YoumeForbidRecord& from);
  void MergeFrom(const YoumeForbidRecord& from);
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
  void InternalSwap(YoumeForbidRecord* other);
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

  // required uint32 room_seq = 1;
  bool has_room_seq() const;
  void clear_room_seq();
  static const int kRoomSeqFieldNumber = 1;
  ::youmecommon::protobuf::uint32 room_seq() const;
  void set_room_seq(::youmecommon::protobuf::uint32 value);

  // required bytes roomid = 2;
  bool has_roomid() const;
  void clear_roomid();
  static const int kRoomidFieldNumber = 2;
  const ::std::string& roomid() const;
  void set_roomid(const ::std::string& value);
  void set_roomid(const char* value);
  void set_roomid(const void* value, size_t size);
  ::std::string* mutable_roomid();
  ::std::string* release_roomid();
  void set_allocated_roomid(::std::string* roomid);

  // required int32 user_right = 3;
  bool has_user_right() const;
  void clear_user_right();
  static const int kUserRightFieldNumber = 3;
  ::youmecommon::protobuf::int32 user_right() const;
  void set_user_right(::youmecommon::protobuf::int32 value);

  // required uint64 expire_time = 4;
  bool has_expire_time() const;
  void clear_expire_time();
  static const int kExpireTimeFieldNumber = 4;
  ::youmecommon::protobuf::uint64 expire_time() const;
  void set_expire_time(::youmecommon::protobuf::uint64 value);

  // required bool forbid_room = 5;
  bool has_forbid_room() const;
  void clear_forbid_room();
  static const int kForbidRoomFieldNumber = 5;
  bool forbid_room() const;
  void set_forbid_room(bool value);

  // required int32 reason_type = 6;
  bool has_reason_type() const;
  void clear_reason_type();
  static const int kReasonTypeFieldNumber = 6;
  ::youmecommon::protobuf::int32 reason_type() const;
  void set_reason_type(::youmecommon::protobuf::int32 value);

  // required string reason = 7;
  bool has_reason() const;
  void clear_reason();
  static const int kReasonFieldNumber = 7;
  const ::std::string& reason() const;
  void set_reason(const ::std::string& value);
  void set_reason(const char* value);
  void set_reason(const char* value, size_t size);
  ::std::string* mutable_reason();
  ::std::string* release_reason();
  void set_allocated_reason(::std::string* reason);

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.YoumeForbidRecord)
 private:
  inline void set_has_room_seq();
  inline void clear_has_room_seq();
  inline void set_has_roomid();
  inline void clear_has_roomid();
  inline void set_has_user_right();
  inline void clear_has_user_right();
  inline void set_has_expire_time();
  inline void clear_has_expire_time();
  inline void set_has_forbid_room();
  inline void clear_has_forbid_room();
  inline void set_has_reason_type();
  inline void clear_has_reason_type();
  inline void set_has_reason();
  inline void clear_has_reason();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::internal::ArenaStringPtr roomid_;
  ::youmecommon::protobuf::uint32 room_seq_;
  ::youmecommon::protobuf::int32 user_right_;
  ::youmecommon::protobuf::uint64 expire_time_;
  bool forbid_room_;
  ::youmecommon::protobuf::int32 reason_type_;
  ::youmecommon::protobuf::internal::ArenaStringPtr reason_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5fforbid_5frecord_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5fforbid_5frecord_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5fforbid_5frecord_2eproto();
  friend void protobuf_ShutdownFile_youme_5fforbid_5frecord_2eproto();

  void InitAsDefaultInstance();
  static YoumeForbidRecord* default_instance_;
};
// -------------------------------------------------------------------

class ForbidRecordRsp : public ::youmecommon::protobuf::MessageLite {
 public:
  ForbidRecordRsp();
  virtual ~ForbidRecordRsp();

  ForbidRecordRsp(const ForbidRecordRsp& from);

  inline ForbidRecordRsp& operator=(const ForbidRecordRsp& from) {
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

  static const ForbidRecordRsp& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const ForbidRecordRsp* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(ForbidRecordRsp* other);

  // implements Message ----------------------------------------------

  inline ForbidRecordRsp* New() const { return New(NULL); }

  ForbidRecordRsp* New(::youmecommon::protobuf::Arena* arena) const;
  void CheckTypeAndMergeFrom(const ::youmecommon::protobuf::MessageLite& from);
  void CopyFrom(const ForbidRecordRsp& from);
  void MergeFrom(const ForbidRecordRsp& from);
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
  void InternalSwap(ForbidRecordRsp* other);
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

  // required int32 record_cnt = 3;
  bool has_record_cnt() const;
  void clear_record_cnt();
  static const int kRecordCntFieldNumber = 3;
  ::youmecommon::protobuf::int32 record_cnt() const;
  void set_record_cnt(::youmecommon::protobuf::int32 value);

  // repeated .YOUMEServiceProtocol.YoumeForbidRecord record_list = 4;
  int record_list_size() const;
  void clear_record_list();
  static const int kRecordListFieldNumber = 4;
  const ::YOUMEServiceProtocol::YoumeForbidRecord& record_list(int index) const;
  ::YOUMEServiceProtocol::YoumeForbidRecord* mutable_record_list(int index);
  ::YOUMEServiceProtocol::YoumeForbidRecord* add_record_list();
  ::youmecommon::protobuf::RepeatedPtrField< ::YOUMEServiceProtocol::YoumeForbidRecord >*
      mutable_record_list();
  const ::youmecommon::protobuf::RepeatedPtrField< ::YOUMEServiceProtocol::YoumeForbidRecord >&
      record_list() const;

  // @@protoc_insertion_point(class_scope:YOUMEServiceProtocol.ForbidRecordRsp)
 private:
  inline void set_has_version();
  inline void clear_has_version();
  inline void set_has_ret();
  inline void clear_has_ret();
  inline void set_has_record_cnt();
  inline void clear_has_record_cnt();

  // helper for ByteSize()
  int RequiredFieldsByteSizeFallback() const;

  ::youmecommon::protobuf::internal::ArenaStringPtr _unknown_fields_;
  ::youmecommon::protobuf::Arena* _arena_ptr_;

  ::youmecommon::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::youmecommon::protobuf::int32 version_;
  ::youmecommon::protobuf::int32 ret_;
  ::youmecommon::protobuf::RepeatedPtrField< ::YOUMEServiceProtocol::YoumeForbidRecord > record_list_;
  ::youmecommon::protobuf::int32 record_cnt_;
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_youme_5fforbid_5frecord_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_youme_5fforbid_5frecord_2eproto();
  #endif
  friend void protobuf_AssignDesc_youme_5fforbid_5frecord_2eproto();
  friend void protobuf_ShutdownFile_youme_5fforbid_5frecord_2eproto();

  void InitAsDefaultInstance();
  static ForbidRecordRsp* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// ForbidRecordReq

// required int32 version = 1;
inline bool ForbidRecordReq::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ForbidRecordReq::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ForbidRecordReq::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ForbidRecordReq::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 ForbidRecordReq::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordReq.version)
  return version_;
}
inline void ForbidRecordReq::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordReq.version)
}

// optional bytes roomid = 2;
inline bool ForbidRecordReq::has_roomid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ForbidRecordReq::set_has_roomid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ForbidRecordReq::clear_has_roomid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ForbidRecordReq::clear_roomid() {
  roomid_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_roomid();
}
inline const ::std::string& ForbidRecordReq::roomid() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordReq.roomid)
  return roomid_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ForbidRecordReq::set_roomid(const ::std::string& value) {
  set_has_roomid();
  roomid_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordReq.roomid)
}
inline void ForbidRecordReq::set_roomid(const char* value) {
  set_has_roomid();
  roomid_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YOUMEServiceProtocol.ForbidRecordReq.roomid)
}
inline void ForbidRecordReq::set_roomid(const void* value, size_t size) {
  set_has_roomid();
  roomid_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YOUMEServiceProtocol.ForbidRecordReq.roomid)
}
inline ::std::string* ForbidRecordReq::mutable_roomid() {
  set_has_roomid();
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.ForbidRecordReq.roomid)
  return roomid_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* ForbidRecordReq::release_roomid() {
  clear_has_roomid();
  return roomid_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ForbidRecordReq::set_allocated_roomid(::std::string* roomid) {
  if (roomid != NULL) {
    set_has_roomid();
  } else {
    clear_has_roomid();
  }
  roomid_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), roomid);
  // @@protoc_insertion_point(field_set_allocated:YOUMEServiceProtocol.ForbidRecordReq.roomid)
}

// optional int32 user_right = 3;
inline bool ForbidRecordReq::has_user_right() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ForbidRecordReq::set_has_user_right() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ForbidRecordReq::clear_has_user_right() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ForbidRecordReq::clear_user_right() {
  user_right_ = 0;
  clear_has_user_right();
}
inline ::youmecommon::protobuf::int32 ForbidRecordReq::user_right() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordReq.user_right)
  return user_right_;
}
inline void ForbidRecordReq::set_user_right(::youmecommon::protobuf::int32 value) {
  set_has_user_right();
  user_right_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordReq.user_right)
}

// optional uint32 room_seq = 4;
inline bool ForbidRecordReq::has_room_seq() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ForbidRecordReq::set_has_room_seq() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ForbidRecordReq::clear_has_room_seq() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ForbidRecordReq::clear_room_seq() {
  room_seq_ = 0u;
  clear_has_room_seq();
}
inline ::youmecommon::protobuf::uint32 ForbidRecordReq::room_seq() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordReq.room_seq)
  return room_seq_;
}
inline void ForbidRecordReq::set_room_seq(::youmecommon::protobuf::uint32 value) {
  set_has_room_seq();
  room_seq_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordReq.room_seq)
}

// optional uint32 limit = 5;
inline bool ForbidRecordReq::has_limit() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ForbidRecordReq::set_has_limit() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ForbidRecordReq::clear_has_limit() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ForbidRecordReq::clear_limit() {
  limit_ = 0u;
  clear_has_limit();
}
inline ::youmecommon::protobuf::uint32 ForbidRecordReq::limit() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordReq.limit)
  return limit_;
}
inline void ForbidRecordReq::set_limit(::youmecommon::protobuf::uint32 value) {
  set_has_limit();
  limit_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordReq.limit)
}

// -------------------------------------------------------------------

// YoumeForbidRecord

// required uint32 room_seq = 1;
inline bool YoumeForbidRecord::has_room_seq() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void YoumeForbidRecord::set_has_room_seq() {
  _has_bits_[0] |= 0x00000001u;
}
inline void YoumeForbidRecord::clear_has_room_seq() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void YoumeForbidRecord::clear_room_seq() {
  room_seq_ = 0u;
  clear_has_room_seq();
}
inline ::youmecommon::protobuf::uint32 YoumeForbidRecord::room_seq() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.room_seq)
  return room_seq_;
}
inline void YoumeForbidRecord::set_room_seq(::youmecommon::protobuf::uint32 value) {
  set_has_room_seq();
  room_seq_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.room_seq)
}

// required bytes roomid = 2;
inline bool YoumeForbidRecord::has_roomid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void YoumeForbidRecord::set_has_roomid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void YoumeForbidRecord::clear_has_roomid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void YoumeForbidRecord::clear_roomid() {
  roomid_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_roomid();
}
inline const ::std::string& YoumeForbidRecord::roomid() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.roomid)
  return roomid_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void YoumeForbidRecord::set_roomid(const ::std::string& value) {
  set_has_roomid();
  roomid_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.roomid)
}
inline void YoumeForbidRecord::set_roomid(const char* value) {
  set_has_roomid();
  roomid_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YOUMEServiceProtocol.YoumeForbidRecord.roomid)
}
inline void YoumeForbidRecord::set_roomid(const void* value, size_t size) {
  set_has_roomid();
  roomid_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YOUMEServiceProtocol.YoumeForbidRecord.roomid)
}
inline ::std::string* YoumeForbidRecord::mutable_roomid() {
  set_has_roomid();
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.YoumeForbidRecord.roomid)
  return roomid_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* YoumeForbidRecord::release_roomid() {
  clear_has_roomid();
  return roomid_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void YoumeForbidRecord::set_allocated_roomid(::std::string* roomid) {
  if (roomid != NULL) {
    set_has_roomid();
  } else {
    clear_has_roomid();
  }
  roomid_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), roomid);
  // @@protoc_insertion_point(field_set_allocated:YOUMEServiceProtocol.YoumeForbidRecord.roomid)
}

// required int32 user_right = 3;
inline bool YoumeForbidRecord::has_user_right() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void YoumeForbidRecord::set_has_user_right() {
  _has_bits_[0] |= 0x00000004u;
}
inline void YoumeForbidRecord::clear_has_user_right() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void YoumeForbidRecord::clear_user_right() {
  user_right_ = 0;
  clear_has_user_right();
}
inline ::youmecommon::protobuf::int32 YoumeForbidRecord::user_right() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.user_right)
  return user_right_;
}
inline void YoumeForbidRecord::set_user_right(::youmecommon::protobuf::int32 value) {
  set_has_user_right();
  user_right_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.user_right)
}

// required uint64 expire_time = 4;
inline bool YoumeForbidRecord::has_expire_time() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void YoumeForbidRecord::set_has_expire_time() {
  _has_bits_[0] |= 0x00000008u;
}
inline void YoumeForbidRecord::clear_has_expire_time() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void YoumeForbidRecord::clear_expire_time() {
  expire_time_ = GOOGLE_ULONGLONG(0);
  clear_has_expire_time();
}
inline ::youmecommon::protobuf::uint64 YoumeForbidRecord::expire_time() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.expire_time)
  return expire_time_;
}
inline void YoumeForbidRecord::set_expire_time(::youmecommon::protobuf::uint64 value) {
  set_has_expire_time();
  expire_time_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.expire_time)
}

// required bool forbid_room = 5;
inline bool YoumeForbidRecord::has_forbid_room() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void YoumeForbidRecord::set_has_forbid_room() {
  _has_bits_[0] |= 0x00000010u;
}
inline void YoumeForbidRecord::clear_has_forbid_room() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void YoumeForbidRecord::clear_forbid_room() {
  forbid_room_ = false;
  clear_has_forbid_room();
}
inline bool YoumeForbidRecord::forbid_room() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.forbid_room)
  return forbid_room_;
}
inline void YoumeForbidRecord::set_forbid_room(bool value) {
  set_has_forbid_room();
  forbid_room_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.forbid_room)
}

// required int32 reason_type = 6;
inline bool YoumeForbidRecord::has_reason_type() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void YoumeForbidRecord::set_has_reason_type() {
  _has_bits_[0] |= 0x00000020u;
}
inline void YoumeForbidRecord::clear_has_reason_type() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void YoumeForbidRecord::clear_reason_type() {
  reason_type_ = 0;
  clear_has_reason_type();
}
inline ::youmecommon::protobuf::int32 YoumeForbidRecord::reason_type() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.reason_type)
  return reason_type_;
}
inline void YoumeForbidRecord::set_reason_type(::youmecommon::protobuf::int32 value) {
  set_has_reason_type();
  reason_type_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.reason_type)
}

// required string reason = 7;
inline bool YoumeForbidRecord::has_reason() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void YoumeForbidRecord::set_has_reason() {
  _has_bits_[0] |= 0x00000040u;
}
inline void YoumeForbidRecord::clear_has_reason() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void YoumeForbidRecord::clear_reason() {
  reason_.ClearToEmptyNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_reason();
}
inline const ::std::string& YoumeForbidRecord::reason() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.YoumeForbidRecord.reason)
  return reason_.GetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void YoumeForbidRecord::set_reason(const ::std::string& value) {
  set_has_reason();
  reason_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.YoumeForbidRecord.reason)
}
inline void YoumeForbidRecord::set_reason(const char* value) {
  set_has_reason();
  reason_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:YOUMEServiceProtocol.YoumeForbidRecord.reason)
}
inline void YoumeForbidRecord::set_reason(const char* value, size_t size) {
  set_has_reason();
  reason_.SetNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:YOUMEServiceProtocol.YoumeForbidRecord.reason)
}
inline ::std::string* YoumeForbidRecord::mutable_reason() {
  set_has_reason();
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.YoumeForbidRecord.reason)
  return reason_.MutableNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* YoumeForbidRecord::release_reason() {
  clear_has_reason();
  return reason_.ReleaseNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void YoumeForbidRecord::set_allocated_reason(::std::string* reason) {
  if (reason != NULL) {
    set_has_reason();
  } else {
    clear_has_reason();
  }
  reason_.SetAllocatedNoArena(&::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited(), reason);
  // @@protoc_insertion_point(field_set_allocated:YOUMEServiceProtocol.YoumeForbidRecord.reason)
}

// -------------------------------------------------------------------

// ForbidRecordRsp

// required int32 version = 1;
inline bool ForbidRecordRsp::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ForbidRecordRsp::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ForbidRecordRsp::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ForbidRecordRsp::clear_version() {
  version_ = 0;
  clear_has_version();
}
inline ::youmecommon::protobuf::int32 ForbidRecordRsp::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordRsp.version)
  return version_;
}
inline void ForbidRecordRsp::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordRsp.version)
}

// required int32 ret = 2;
inline bool ForbidRecordRsp::has_ret() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ForbidRecordRsp::set_has_ret() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ForbidRecordRsp::clear_has_ret() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ForbidRecordRsp::clear_ret() {
  ret_ = 0;
  clear_has_ret();
}
inline ::youmecommon::protobuf::int32 ForbidRecordRsp::ret() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordRsp.ret)
  return ret_;
}
inline void ForbidRecordRsp::set_ret(::youmecommon::protobuf::int32 value) {
  set_has_ret();
  ret_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordRsp.ret)
}

// required int32 record_cnt = 3;
inline bool ForbidRecordRsp::has_record_cnt() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ForbidRecordRsp::set_has_record_cnt() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ForbidRecordRsp::clear_has_record_cnt() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ForbidRecordRsp::clear_record_cnt() {
  record_cnt_ = 0;
  clear_has_record_cnt();
}
inline ::youmecommon::protobuf::int32 ForbidRecordRsp::record_cnt() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordRsp.record_cnt)
  return record_cnt_;
}
inline void ForbidRecordRsp::set_record_cnt(::youmecommon::protobuf::int32 value) {
  set_has_record_cnt();
  record_cnt_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.ForbidRecordRsp.record_cnt)
}

// repeated .YOUMEServiceProtocol.YoumeForbidRecord record_list = 4;
inline int ForbidRecordRsp::record_list_size() const {
  return record_list_.size();
}
inline void ForbidRecordRsp::clear_record_list() {
  record_list_.Clear();
}
inline const ::YOUMEServiceProtocol::YoumeForbidRecord& ForbidRecordRsp::record_list(int index) const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.ForbidRecordRsp.record_list)
  return record_list_.Get(index);
}
inline ::YOUMEServiceProtocol::YoumeForbidRecord* ForbidRecordRsp::mutable_record_list(int index) {
  // @@protoc_insertion_point(field_mutable:YOUMEServiceProtocol.ForbidRecordRsp.record_list)
  return record_list_.Mutable(index);
}
inline ::YOUMEServiceProtocol::YoumeForbidRecord* ForbidRecordRsp::add_record_list() {
  // @@protoc_insertion_point(field_add:YOUMEServiceProtocol.ForbidRecordRsp.record_list)
  return record_list_.Add();
}
inline ::youmecommon::protobuf::RepeatedPtrField< ::YOUMEServiceProtocol::YoumeForbidRecord >*
ForbidRecordRsp::mutable_record_list() {
  // @@protoc_insertion_point(field_mutable_list:YOUMEServiceProtocol.ForbidRecordRsp.record_list)
  return &record_list_;
}
inline const ::youmecommon::protobuf::RepeatedPtrField< ::YOUMEServiceProtocol::YoumeForbidRecord >&
ForbidRecordRsp::record_list() const {
  // @@protoc_insertion_point(field_list:YOUMEServiceProtocol.ForbidRecordRsp.record_list)
  return record_list_;
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace YOUMEServiceProtocol

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_youme_5fforbid_5frecord_2eproto__INCLUDED
