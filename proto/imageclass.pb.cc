// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: imageclass.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "imageclass.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace youtu {

namespace {

const ::google::protobuf::Descriptor* ClassRequest_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ClassRequest_reflection_ = NULL;
const ::google::protobuf::Descriptor* ClassResult_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ClassResult_reflection_ = NULL;
const ::google::protobuf::Descriptor* ClassResponse_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ClassResponse_reflection_ = NULL;
const ::google::protobuf::ServiceDescriptor* ImageClass_descriptor_ = NULL;

}  // namespace


void protobuf_AssignDesc_imageclass_2eproto() {
  protobuf_AddDesc_imageclass_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "imageclass.proto");
  GOOGLE_CHECK(file != NULL);
  ClassRequest_descriptor_ = file->message_type(0);
  static const int ClassRequest_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassRequest, photoid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassRequest, data_),
  };
  ClassRequest_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ClassRequest_descriptor_,
      ClassRequest::default_instance_,
      ClassRequest_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassRequest, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassRequest, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ClassRequest));
  ClassResult_descriptor_ = file->message_type(1);
  static const int ClassResult_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResult, tags_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResult, sim_),
  };
  ClassResult_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ClassResult_descriptor_,
      ClassResult::default_instance_,
      ClassResult_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResult, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResult, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ClassResult));
  ClassResponse_descriptor_ = file->message_type(2);
  static const int ClassResponse_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResponse, resultarr_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResponse, similarurl_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResponse, computetime_),
  };
  ClassResponse_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ClassResponse_descriptor_,
      ClassResponse::default_instance_,
      ClassResponse_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResponse, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ClassResponse, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ClassResponse));
  ImageClass_descriptor_ = file->service(0);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_imageclass_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ClassRequest_descriptor_, &ClassRequest::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ClassResult_descriptor_, &ClassResult::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ClassResponse_descriptor_, &ClassResponse::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_imageclass_2eproto() {
  delete ClassRequest::default_instance_;
  delete ClassRequest_reflection_;
  delete ClassResult::default_instance_;
  delete ClassResult_reflection_;
  delete ClassResponse::default_instance_;
  delete ClassResponse_reflection_;
}

void protobuf_AddDesc_imageclass_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020imageclass.proto\022\005youtu\"-\n\014ClassReques"
    "t\022\017\n\007photoid\030\001 \002(\014\022\014\n\004data\030\002 \002(\014\"(\n\013Clas"
    "sResult\022\014\n\004tags\030\001 \002(\t\022\013\n\003sim\030\002 \002(\002\"_\n\rCl"
    "assResponse\022%\n\tresultArr\030\001 \003(\0132\022.youtu.C"
    "lassResult\022\022\n\nsimilarUrl\030\002 \003(\t\022\023\n\013comput"
    "eTime\030\003 \001(\0022C\n\nImageClass\0225\n\010classify\022\023."
    "youtu.ClassRequest\032\024.youtu.ClassResponse"
    "B\034\n\rorg.youtufaceB\010Classify\200\001\001", 310);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "imageclass.proto", &protobuf_RegisterTypes);
  ClassRequest::default_instance_ = new ClassRequest();
  ClassResult::default_instance_ = new ClassResult();
  ClassResponse::default_instance_ = new ClassResponse();
  ClassRequest::default_instance_->InitAsDefaultInstance();
  ClassResult::default_instance_->InitAsDefaultInstance();
  ClassResponse::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_imageclass_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_imageclass_2eproto {
  StaticDescriptorInitializer_imageclass_2eproto() {
    protobuf_AddDesc_imageclass_2eproto();
  }
} static_descriptor_initializer_imageclass_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int ClassRequest::kPhotoidFieldNumber;
const int ClassRequest::kDataFieldNumber;
#endif  // !_MSC_VER

ClassRequest::ClassRequest()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ClassRequest::InitAsDefaultInstance() {
}

ClassRequest::ClassRequest(const ClassRequest& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ClassRequest::SharedCtor() {
  _cached_size_ = 0;
  photoid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  data_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ClassRequest::~ClassRequest() {
  SharedDtor();
}

void ClassRequest::SharedDtor() {
  if (photoid_ != &::google::protobuf::internal::kEmptyString) {
    delete photoid_;
  }
  if (data_ != &::google::protobuf::internal::kEmptyString) {
    delete data_;
  }
  if (this != default_instance_) {
  }
}

void ClassRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ClassRequest::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ClassRequest_descriptor_;
}

const ClassRequest& ClassRequest::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_imageclass_2eproto();
  return *default_instance_;
}

ClassRequest* ClassRequest::default_instance_ = NULL;

ClassRequest* ClassRequest::New() const {
  return new ClassRequest;
}

void ClassRequest::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_photoid()) {
      if (photoid_ != &::google::protobuf::internal::kEmptyString) {
        photoid_->clear();
      }
    }
    if (has_data()) {
      if (data_ != &::google::protobuf::internal::kEmptyString) {
        data_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ClassRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required bytes photoid = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_photoid()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_data;
        break;
      }

      // required bytes data = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_data:
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_data()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ClassRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required bytes photoid = 1;
  if (has_photoid()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->photoid(), output);
  }

  // required bytes data = 2;
  if (has_data()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      2, this->data(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ClassRequest::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required bytes photoid = 1;
  if (has_photoid()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->photoid(), target);
  }

  // required bytes data = 2;
  if (has_data()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->data(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ClassRequest::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required bytes photoid = 1;
    if (has_photoid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->photoid());
    }

    // required bytes data = 2;
    if (has_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->data());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ClassRequest::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ClassRequest* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ClassRequest*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ClassRequest::MergeFrom(const ClassRequest& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_photoid()) {
      set_photoid(from.photoid());
    }
    if (from.has_data()) {
      set_data(from.data());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ClassRequest::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ClassRequest::CopyFrom(const ClassRequest& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ClassRequest::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void ClassRequest::Swap(ClassRequest* other) {
  if (other != this) {
    std::swap(photoid_, other->photoid_);
    std::swap(data_, other->data_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ClassRequest::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ClassRequest_descriptor_;
  metadata.reflection = ClassRequest_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ClassResult::kTagsFieldNumber;
const int ClassResult::kSimFieldNumber;
#endif  // !_MSC_VER

ClassResult::ClassResult()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ClassResult::InitAsDefaultInstance() {
}

ClassResult::ClassResult(const ClassResult& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ClassResult::SharedCtor() {
  _cached_size_ = 0;
  tags_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  sim_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ClassResult::~ClassResult() {
  SharedDtor();
}

void ClassResult::SharedDtor() {
  if (tags_ != &::google::protobuf::internal::kEmptyString) {
    delete tags_;
  }
  if (this != default_instance_) {
  }
}

void ClassResult::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ClassResult::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ClassResult_descriptor_;
}

const ClassResult& ClassResult::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_imageclass_2eproto();
  return *default_instance_;
}

ClassResult* ClassResult::default_instance_ = NULL;

ClassResult* ClassResult::New() const {
  return new ClassResult;
}

void ClassResult::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_tags()) {
      if (tags_ != &::google::protobuf::internal::kEmptyString) {
        tags_->clear();
      }
    }
    sim_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ClassResult::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string tags = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_tags()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->tags().data(), this->tags().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(21)) goto parse_sim;
        break;
      }

      // required float sim = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED32) {
         parse_sim:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &sim_)));
          set_has_sim();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ClassResult::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string tags = 1;
  if (has_tags()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->tags().data(), this->tags().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->tags(), output);
  }

  // required float sim = 2;
  if (has_sim()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(2, this->sim(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ClassResult::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string tags = 1;
  if (has_tags()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->tags().data(), this->tags().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->tags(), target);
  }

  // required float sim = 2;
  if (has_sim()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(2, this->sim(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ClassResult::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string tags = 1;
    if (has_tags()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->tags());
    }

    // required float sim = 2;
    if (has_sim()) {
      total_size += 1 + 4;
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ClassResult::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ClassResult* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ClassResult*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ClassResult::MergeFrom(const ClassResult& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_tags()) {
      set_tags(from.tags());
    }
    if (from.has_sim()) {
      set_sim(from.sim());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ClassResult::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ClassResult::CopyFrom(const ClassResult& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ClassResult::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000003) != 0x00000003) return false;

  return true;
}

void ClassResult::Swap(ClassResult* other) {
  if (other != this) {
    std::swap(tags_, other->tags_);
    std::swap(sim_, other->sim_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ClassResult::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ClassResult_descriptor_;
  metadata.reflection = ClassResult_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ClassResponse::kResultArrFieldNumber;
const int ClassResponse::kSimilarUrlFieldNumber;
const int ClassResponse::kComputeTimeFieldNumber;
#endif  // !_MSC_VER

ClassResponse::ClassResponse()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ClassResponse::InitAsDefaultInstance() {
}

ClassResponse::ClassResponse(const ClassResponse& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ClassResponse::SharedCtor() {
  _cached_size_ = 0;
  computetime_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ClassResponse::~ClassResponse() {
  SharedDtor();
}

void ClassResponse::SharedDtor() {
  if (this != default_instance_) {
  }
}

void ClassResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ClassResponse::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ClassResponse_descriptor_;
}

const ClassResponse& ClassResponse::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_imageclass_2eproto();
  return *default_instance_;
}

ClassResponse* ClassResponse::default_instance_ = NULL;

ClassResponse* ClassResponse::New() const {
  return new ClassResponse;
}

void ClassResponse::Clear() {
  if (_has_bits_[2 / 32] & (0xffu << (2 % 32))) {
    computetime_ = 0;
  }
  resultarr_.Clear();
  similarurl_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ClassResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .youtu.ClassResult resultArr = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_resultArr:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_resultarr()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_resultArr;
        if (input->ExpectTag(18)) goto parse_similarUrl;
        break;
      }

      // repeated string similarUrl = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_similarUrl:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_similarurl()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->similarurl(this->similarurl_size() - 1).data(),
            this->similarurl(this->similarurl_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_similarUrl;
        if (input->ExpectTag(29)) goto parse_computeTime;
        break;
      }

      // optional float computeTime = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED32) {
         parse_computeTime:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &computetime_)));
          set_has_computetime();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ClassResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .youtu.ClassResult resultArr = 1;
  for (int i = 0; i < this->resultarr_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->resultarr(i), output);
  }

  // repeated string similarUrl = 2;
  for (int i = 0; i < this->similarurl_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->similarurl(i).data(), this->similarurl(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->similarurl(i), output);
  }

  // optional float computeTime = 3;
  if (has_computetime()) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(3, this->computetime(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ClassResponse::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .youtu.ClassResult resultArr = 1;
  for (int i = 0; i < this->resultarr_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->resultarr(i), target);
  }

  // repeated string similarUrl = 2;
  for (int i = 0; i < this->similarurl_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->similarurl(i).data(), this->similarurl(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(2, this->similarurl(i), target);
  }

  // optional float computeTime = 3;
  if (has_computetime()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(3, this->computetime(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ClassResponse::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[2 / 32] & (0xffu << (2 % 32))) {
    // optional float computeTime = 3;
    if (has_computetime()) {
      total_size += 1 + 4;
    }

  }
  // repeated .youtu.ClassResult resultArr = 1;
  total_size += 1 * this->resultarr_size();
  for (int i = 0; i < this->resultarr_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->resultarr(i));
  }

  // repeated string similarUrl = 2;
  total_size += 1 * this->similarurl_size();
  for (int i = 0; i < this->similarurl_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->similarurl(i));
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ClassResponse::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ClassResponse* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ClassResponse*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ClassResponse::MergeFrom(const ClassResponse& from) {
  GOOGLE_CHECK_NE(&from, this);
  resultarr_.MergeFrom(from.resultarr_);
  similarurl_.MergeFrom(from.similarurl_);
  if (from._has_bits_[2 / 32] & (0xffu << (2 % 32))) {
    if (from.has_computetime()) {
      set_computetime(from.computetime());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ClassResponse::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ClassResponse::CopyFrom(const ClassResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ClassResponse::IsInitialized() const {

  for (int i = 0; i < resultarr_size(); i++) {
    if (!this->resultarr(i).IsInitialized()) return false;
  }
  return true;
}

void ClassResponse::Swap(ClassResponse* other) {
  if (other != this) {
    resultarr_.Swap(&other->resultarr_);
    similarurl_.Swap(&other->similarurl_);
    std::swap(computetime_, other->computetime_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ClassResponse::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ClassResponse_descriptor_;
  metadata.reflection = ClassResponse_reflection_;
  return metadata;
}


// ===================================================================

ImageClass::~ImageClass() {}

const ::google::protobuf::ServiceDescriptor* ImageClass::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ImageClass_descriptor_;
}

const ::google::protobuf::ServiceDescriptor* ImageClass::GetDescriptor() {
  protobuf_AssignDescriptorsOnce();
  return ImageClass_descriptor_;
}

void ImageClass::classify(::google::protobuf::RpcController* controller,
                         const ::youtu::ClassRequest*,
                         ::youtu::ClassResponse*,
                         ::google::protobuf::Closure* done) {
  controller->SetFailed("Method classify() not implemented.");
  done->Run();
}

void ImageClass::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                             ::google::protobuf::RpcController* controller,
                             const ::google::protobuf::Message* request,
                             ::google::protobuf::Message* response,
                             ::google::protobuf::Closure* done) {
  GOOGLE_DCHECK_EQ(method->service(), ImageClass_descriptor_);
  switch(method->index()) {
    case 0:
      classify(controller,
             ::google::protobuf::down_cast<const ::youtu::ClassRequest*>(request),
             ::google::protobuf::down_cast< ::youtu::ClassResponse*>(response),
             done);
      break;
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      break;
  }
}

const ::google::protobuf::Message& ImageClass::GetRequestPrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::youtu::ClassRequest::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);
  }
}

const ::google::protobuf::Message& ImageClass::GetResponsePrototype(
    const ::google::protobuf::MethodDescriptor* method) const {
  GOOGLE_DCHECK_EQ(method->service(), descriptor());
  switch(method->index()) {
    case 0:
      return ::youtu::ClassResponse::default_instance();
    default:
      GOOGLE_LOG(FATAL) << "Bad method index; this should never happen.";
      return *reinterpret_cast< ::google::protobuf::Message*>(NULL);
  }
}

ImageClass_Stub::ImageClass_Stub(::google::protobuf::RpcChannel* channel)
  : channel_(channel), owns_channel_(false) {}
ImageClass_Stub::ImageClass_Stub(
    ::google::protobuf::RpcChannel* channel,
    ::google::protobuf::Service::ChannelOwnership ownership)
  : channel_(channel),
    owns_channel_(ownership == ::google::protobuf::Service::STUB_OWNS_CHANNEL) {}
ImageClass_Stub::~ImageClass_Stub() {
  if (owns_channel_) delete channel_;
}

void ImageClass_Stub::classify(::google::protobuf::RpcController* controller,
                              const ::youtu::ClassRequest* request,
                              ::youtu::ClassResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}

// @@protoc_insertion_point(namespace_scope)

}  // namespace youtu

// @@protoc_insertion_point(global_scope)
