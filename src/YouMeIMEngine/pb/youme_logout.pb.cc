// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: youme_logout.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "youme_logout.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
// @@protoc_insertion_point(includes)

namespace YOUMEServiceProtocol {

void protobuf_ShutdownFile_youme_5flogout_2eproto() {
  delete LogoutReq::default_instance_;
  delete LogoutRsp::default_instance_;
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_youme_5flogout_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_youme_5flogout_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  LogoutReq::default_instance_ = new LogoutReq();
  LogoutRsp::default_instance_ = new LogoutRsp();
  LogoutReq::default_instance_->InitAsDefaultInstance();
  LogoutRsp::default_instance_->InitAsDefaultInstance();
  ::youmecommon::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_youme_5flogout_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_youme_5flogout_2eproto_once_);
void protobuf_AddDesc_youme_5flogout_2eproto() {
  ::youmecommon::protobuf::GoogleOnceInit(&protobuf_AddDesc_youme_5flogout_2eproto_once_,
                 &protobuf_AddDesc_youme_5flogout_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_youme_5flogout_2eproto {
  StaticDescriptorInitializer_youme_5flogout_2eproto() {
    protobuf_AddDesc_youme_5flogout_2eproto();
  }
} static_descriptor_initializer_youme_5flogout_2eproto_;
#endif

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

#ifndef _MSC_VER
const int LogoutReq::kVersionFieldNumber;
#endif  // !_MSC_VER

LogoutReq::LogoutReq()
  : ::youmecommon::protobuf::MessageLite(), _arena_ptr_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:YOUMEServiceProtocol.LogoutReq)
}

void LogoutReq::InitAsDefaultInstance() {
}

LogoutReq::LogoutReq(const LogoutReq& from)
  : ::youmecommon::protobuf::MessageLite(),
    _arena_ptr_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:YOUMEServiceProtocol.LogoutReq)
}

void LogoutReq::SharedCtor() {
  ::youmecommon::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  _unknown_fields_.UnsafeSetDefault(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  version_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

LogoutReq::~LogoutReq() {
  // @@protoc_insertion_point(destructor:YOUMEServiceProtocol.LogoutReq)
  SharedDtor();
}

void LogoutReq::SharedDtor() {
  _unknown_fields_.DestroyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void LogoutReq::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const LogoutReq& LogoutReq::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_youme_5flogout_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_youme_5flogout_2eproto();
#endif
  return *default_instance_;
}

LogoutReq* LogoutReq::default_instance_ = NULL;

LogoutReq* LogoutReq::New(::youmecommon::protobuf::Arena* arena) const {
  LogoutReq* n = new LogoutReq;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void LogoutReq::Clear() {
  version_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  _unknown_fields_.ClearToEmptyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool LogoutReq::MergePartialFromCodedStream(
    ::youmecommon::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::youmecommon::protobuf::uint32 tag;
  ::youmecommon::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::youmecommon::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:YOUMEServiceProtocol.LogoutReq)
  for (;;) {
    ::std::pair< ::youmecommon::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::youmecommon::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 version = 1;
      case 1: {
        if (tag == 8) {
          DO_((::youmecommon::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::youmecommon::protobuf::int32, ::youmecommon::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &version_)));
          set_has_version();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::youmecommon::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::youmecommon::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::youmecommon::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:YOUMEServiceProtocol.LogoutReq)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:YOUMEServiceProtocol.LogoutReq)
  return false;
#undef DO_
}

void LogoutReq::SerializeWithCachedSizes(
    ::youmecommon::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:YOUMEServiceProtocol.LogoutReq)
  // required int32 version = 1;
  if (has_version()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteInt32(1, this->version(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:YOUMEServiceProtocol.LogoutReq)
}

int LogoutReq::ByteSize() const {
  int total_size = 0;

  // required int32 version = 1;
  if (has_version()) {
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->version());
  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void LogoutReq::CheckTypeAndMergeFrom(
    const ::youmecommon::protobuf::MessageLite& from) {
  MergeFrom(*::youmecommon::protobuf::down_cast<const LogoutReq*>(&from));
}

void LogoutReq::MergeFrom(const LogoutReq& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_version()) {
      set_version(from.version());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void LogoutReq::CopyFrom(const LogoutReq& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool LogoutReq::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void LogoutReq::Swap(LogoutReq* other) {
  if (other == this) return;
  InternalSwap(other);
}
void LogoutReq::InternalSwap(LogoutReq* other) {
  std::swap(version_, other->version_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _unknown_fields_.Swap(&other->_unknown_fields_);
  std::swap(_cached_size_, other->_cached_size_);
}

::std::string LogoutReq::GetTypeName() const {
  return "YOUMEServiceProtocol.LogoutReq";
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// LogoutReq

// required int32 version = 1;
bool LogoutReq::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void LogoutReq::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
void LogoutReq::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
void LogoutReq::clear_version() {
  version_ = 0;
  clear_has_version();
}
 ::youmecommon::protobuf::int32 LogoutReq::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.LogoutReq.version)
  return version_;
}
 void LogoutReq::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.LogoutReq.version)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#ifndef _MSC_VER
const int LogoutRsp::kVersionFieldNumber;
const int LogoutRsp::kRetFieldNumber;
#endif  // !_MSC_VER

LogoutRsp::LogoutRsp()
  : ::youmecommon::protobuf::MessageLite(), _arena_ptr_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:YOUMEServiceProtocol.LogoutRsp)
}

void LogoutRsp::InitAsDefaultInstance() {
}

LogoutRsp::LogoutRsp(const LogoutRsp& from)
  : ::youmecommon::protobuf::MessageLite(),
    _arena_ptr_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:YOUMEServiceProtocol.LogoutRsp)
}

void LogoutRsp::SharedCtor() {
  ::youmecommon::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  _unknown_fields_.UnsafeSetDefault(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  version_ = 0;
  ret_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

LogoutRsp::~LogoutRsp() {
  // @@protoc_insertion_point(destructor:YOUMEServiceProtocol.LogoutRsp)
  SharedDtor();
}

void LogoutRsp::SharedDtor() {
  _unknown_fields_.DestroyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  if (this != &default_instance()) {
  #else
  if (this != default_instance_) {
  #endif
  }
}

void LogoutRsp::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const LogoutRsp& LogoutRsp::default_instance() {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  protobuf_AddDesc_youme_5flogout_2eproto();
#else
  if (default_instance_ == NULL) protobuf_AddDesc_youme_5flogout_2eproto();
#endif
  return *default_instance_;
}

LogoutRsp* LogoutRsp::default_instance_ = NULL;

LogoutRsp* LogoutRsp::New(::youmecommon::protobuf::Arena* arena) const {
  LogoutRsp* n = new LogoutRsp;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void LogoutRsp::Clear() {
#define ZR_HELPER_(f) reinterpret_cast<char*>(\
  &reinterpret_cast<LogoutRsp*>(16)->f)

#define ZR_(first, last) do {\
  ::memset(&first, 0,\
           ZR_HELPER_(last) - ZR_HELPER_(first) + sizeof(last));\
} while (0)

  ZR_(version_, ret_);

#undef ZR_HELPER_
#undef ZR_

  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  _unknown_fields_.ClearToEmptyNoArena(
      &::youmecommon::protobuf::internal::GetEmptyStringAlreadyInited());
}

bool LogoutRsp::MergePartialFromCodedStream(
    ::youmecommon::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::youmecommon::protobuf::uint32 tag;
  ::youmecommon::protobuf::io::StringOutputStream unknown_fields_string(
      mutable_unknown_fields());
  ::youmecommon::protobuf::io::CodedOutputStream unknown_fields_stream(
      &unknown_fields_string);
  // @@protoc_insertion_point(parse_start:YOUMEServiceProtocol.LogoutRsp)
  for (;;) {
    ::std::pair< ::youmecommon::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::youmecommon::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required int32 version = 1;
      case 1: {
        if (tag == 8) {
          DO_((::youmecommon::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::youmecommon::protobuf::int32, ::youmecommon::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &version_)));
          set_has_version();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_ret;
        break;
      }

      // required int32 ret = 2;
      case 2: {
        if (tag == 16) {
         parse_ret:
          DO_((::youmecommon::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::youmecommon::protobuf::int32, ::youmecommon::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &ret_)));
          set_has_ret();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::youmecommon::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::youmecommon::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::youmecommon::protobuf::internal::WireFormatLite::SkipField(
            input, tag, &unknown_fields_stream));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:YOUMEServiceProtocol.LogoutRsp)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:YOUMEServiceProtocol.LogoutRsp)
  return false;
#undef DO_
}

void LogoutRsp::SerializeWithCachedSizes(
    ::youmecommon::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:YOUMEServiceProtocol.LogoutRsp)
  // required int32 version = 1;
  if (has_version()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteInt32(1, this->version(), output);
  }

  // required int32 ret = 2;
  if (has_ret()) {
    ::youmecommon::protobuf::internal::WireFormatLite::WriteInt32(2, this->ret(), output);
  }

  output->WriteRaw(unknown_fields().data(),
                   unknown_fields().size());
  // @@protoc_insertion_point(serialize_end:YOUMEServiceProtocol.LogoutRsp)
}

int LogoutRsp::RequiredFieldsByteSizeFallback() const {
  int total_size = 0;

  if (has_version()) {
    // required int32 version = 1;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->version());
  }

  if (has_ret()) {
    // required int32 ret = 2;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->ret());
  }

  return total_size;
}
int LogoutRsp::ByteSize() const {
  int total_size = 0;

  if (((_has_bits_[0] & 0x00000003) ^ 0x00000003) == 0) {  // All required fields are present.
    // required int32 version = 1;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->version());

    // required int32 ret = 2;
    total_size += 1 +
      ::youmecommon::protobuf::internal::WireFormatLite::Int32Size(
        this->ret());

  } else {
    total_size += RequiredFieldsByteSizeFallback();
  }
  total_size += unknown_fields().size();

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void LogoutRsp::CheckTypeAndMergeFrom(
    const ::youmecommon::protobuf::MessageLite& from) {
  MergeFrom(*::youmecommon::protobuf::down_cast<const LogoutRsp*>(&from));
}

void LogoutRsp::MergeFrom(const LogoutRsp& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_version()) {
      set_version(from.version());
    }
    if (from.has_ret()) {
      set_ret(from.ret());
    }
  }
  mutable_unknown_fields()->append(from.unknown_fields());
}

void LogoutRsp::CopyFrom(const LogoutRsp& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool LogoutRsp::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void LogoutRsp::Swap(LogoutRsp* other) {
  if (other == this) return;
  InternalSwap(other);
}
void LogoutRsp::InternalSwap(LogoutRsp* other) {
  std::swap(version_, other->version_);
  std::swap(ret_, other->ret_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _unknown_fields_.Swap(&other->_unknown_fields_);
  std::swap(_cached_size_, other->_cached_size_);
}

::std::string LogoutRsp::GetTypeName() const {
  return "YOUMEServiceProtocol.LogoutRsp";
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// LogoutRsp

// required int32 version = 1;
bool LogoutRsp::has_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
void LogoutRsp::set_has_version() {
  _has_bits_[0] |= 0x00000001u;
}
void LogoutRsp::clear_has_version() {
  _has_bits_[0] &= ~0x00000001u;
}
void LogoutRsp::clear_version() {
  version_ = 0;
  clear_has_version();
}
 ::youmecommon::protobuf::int32 LogoutRsp::version() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.LogoutRsp.version)
  return version_;
}
 void LogoutRsp::set_version(::youmecommon::protobuf::int32 value) {
  set_has_version();
  version_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.LogoutRsp.version)
}

// required int32 ret = 2;
bool LogoutRsp::has_ret() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
void LogoutRsp::set_has_ret() {
  _has_bits_[0] |= 0x00000002u;
}
void LogoutRsp::clear_has_ret() {
  _has_bits_[0] &= ~0x00000002u;
}
void LogoutRsp::clear_ret() {
  ret_ = 0;
  clear_has_ret();
}
 ::youmecommon::protobuf::int32 LogoutRsp::ret() const {
  // @@protoc_insertion_point(field_get:YOUMEServiceProtocol.LogoutRsp.ret)
  return ret_;
}
 void LogoutRsp::set_ret(::youmecommon::protobuf::int32 value) {
  set_has_ret();
  ret_ = value;
  // @@protoc_insertion_point(field_set:YOUMEServiceProtocol.LogoutRsp.ret)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace YOUMEServiceProtocol

// @@protoc_insertion_point(global_scope)
