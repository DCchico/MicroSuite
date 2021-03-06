// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: router.proto

#include "router.pb.h"
#include "router.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace router {

static const char* RouterService_method_names[] = {
  "/router.RouterService/Router",
};

std::unique_ptr< RouterService::Stub> RouterService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< RouterService::Stub> stub(new RouterService::Stub(channel));
  return stub;
}

RouterService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Router_(RouterService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status RouterService::Stub::Router(::grpc::ClientContext* context, const ::router::RouterRequest& request, ::router::LookupResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Router_, context, request, response);
}

void RouterService::Stub::experimental_async::Router(::grpc::ClientContext* context, const ::router::RouterRequest* request, ::router::LookupResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Router_, context, request, response, std::move(f));
}

void RouterService::Stub::experimental_async::Router(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::router::LookupResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Router_, context, request, response, std::move(f));
}

void RouterService::Stub::experimental_async::Router(::grpc::ClientContext* context, const ::router::RouterRequest* request, ::router::LookupResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Router_, context, request, response, reactor);
}

void RouterService::Stub::experimental_async::Router(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::router::LookupResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Router_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::router::LookupResponse>* RouterService::Stub::AsyncRouterRaw(::grpc::ClientContext* context, const ::router::RouterRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::router::LookupResponse>::Create(channel_.get(), cq, rpcmethod_Router_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::router::LookupResponse>* RouterService::Stub::PrepareAsyncRouterRaw(::grpc::ClientContext* context, const ::router::RouterRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::router::LookupResponse>::Create(channel_.get(), cq, rpcmethod_Router_, context, request, false);
}

RouterService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      RouterService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< RouterService::Service, ::router::RouterRequest, ::router::LookupResponse>(
          [](RouterService::Service* service,
             ::grpc_impl::ServerContext* ctx,
             const ::router::RouterRequest* req,
             ::router::LookupResponse* resp) {
               return service->Router(ctx, req, resp);
             }, this)));
}

RouterService::Service::~Service() {
}

::grpc::Status RouterService::Service::Router(::grpc::ServerContext* context, const ::router::RouterRequest* request, ::router::LookupResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace router

