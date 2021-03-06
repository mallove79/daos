// Code generated by protoc-gen-go. DO NOT EDIT.
// source: control.proto

package ctl

import (
	context "context"
	fmt "fmt"
	proto "github.com/golang/protobuf/proto"
	grpc "google.golang.org/grpc"
	codes "google.golang.org/grpc/codes"
	status "google.golang.org/grpc/status"
	math "math"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

func init() {
	proto.RegisterFile("control.proto", fileDescriptor_0c5120591600887d)
}

var fileDescriptor_0c5120591600887d = []byte{
	// 297 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x74, 0xd2, 0xcd, 0x4a, 0xc4, 0x30,
	0x10, 0x07, 0x70, 0x41, 0x54, 0x88, 0xb6, 0x60, 0x56, 0x57, 0xec, 0xd1, 0x07, 0xe8, 0x41, 0x0f,
	0x82, 0x27, 0xa1, 0xb0, 0x27, 0x15, 0xdd, 0xe2, 0x03, 0xc4, 0x1a, 0x17, 0xb1, 0x6d, 0xe2, 0x64,
	0x64, 0xd9, 0x97, 0xf0, 0x99, 0x25, 0x5f, 0x76, 0xd2, 0xed, 0x1e, 0xf3, 0xdb, 0xf9, 0x67, 0x36,
	0xd3, 0x61, 0x59, 0xa3, 0x7a, 0x04, 0xd5, 0x96, 0x1a, 0x14, 0x2a, 0xbe, 0xdf, 0x60, 0x5b, 0x64,
	0x06, 0x15, 0x88, 0x95, 0xf4, 0x56, 0x64, 0xbd, 0xc4, 0xb5, 0x82, 0xaf, 0x70, 0x3c, 0x31, 0x1b,
	0x83, 0xb2, 0x0b, 0xa7, 0xfc, 0xe3, 0x13, 0xba, 0xb5, 0x80, 0x50, 0x7c, 0xfd, 0x7b, 0xc0, 0x8e,
	0x1e, 0x57, 0x1d, 0x56, 0xd8, 0xf2, 0x8a, 0xe5, 0xb5, 0xbf, 0xe9, 0x19, 0xa4, 0x16, 0x20, 0xf9,
	0xbc, 0x6c, 0xb0, 0x2d, 0x53, 0x5c, 0xca, 0xef, 0xe2, 0x62, 0xd2, 0x8d, 0xbe, 0xda, 0xe3, 0x77,
	0xec, 0x38, 0x78, 0xdd, 0x88, 0x9e, 0xcf, 0x68, 0xa5, 0x15, 0x1b, 0x3f, 0xdb, 0x46, 0x97, 0xbd,
	0x67, 0x59, 0xc0, 0x85, 0x82, 0x4e, 0x20, 0x3f, 0xa7, 0x85, 0xde, 0x6c, 0x7e, 0x3e, 0xc5, 0xff,
	0xdd, 0xdd, 0x73, 0x5f, 0x7e, 0x24, 0x6c, 0x62, 0xf7, 0x41, 0x48, 0x77, 0x8a, 0x2e, 0x7b, 0xcb,
	0x98, 0xc7, 0x1a, 0x95, 0xe6, 0x9c, 0x54, 0x59, 0xb0, 0xc9, 0xd9, 0x96, 0xb9, 0xe0, 0x03, 0x3b,
	0xf5, 0xb6, 0x94, 0x46, 0x62, 0xf8, 0xeb, 0x97, 0xa4, 0x96, 0xb8, 0xbd, 0xa6, 0xd8, 0xf5, 0x53,
	0xfa, 0x84, 0x1a, 0x05, 0x20, 0x4f, 0x7b, 0x0a, 0xc0, 0xf1, 0x13, 0x02, 0xc6, 0xec, 0x93, 0xff,
	0xf8, 0x64, 0xf8, 0x44, 0x86, 0x6c, 0x82, 0x71, 0xf8, 0x8b, 0xb0, 0x1b, 0x7e, 0x78, 0x7e, 0xf8,
	0x89, 0x0d, 0xc3, 0x1f, 0xb1, 0xbb, 0xa1, 0x62, 0x79, 0xe4, 0x57, 0xfd, 0x2e, 0x30, 0xee, 0x4f,
	0x8a, 0xc3, 0xfe, 0x8c, 0xdd, 0x5e, 0xf2, 0x76, 0xe8, 0xf6, 0xf2, 0xe6, 0x2f, 0x00, 0x00, 0xff,
	0xff, 0x35, 0x43, 0x60, 0xc7, 0xe9, 0x02, 0x00, 0x00,
}

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConnInterface

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion6

// MgmtCtlClient is the client API for MgmtCtl service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://godoc.org/google.golang.org/grpc#ClientConn.NewStream.
type MgmtCtlClient interface {
	// Prepare nonvolatile storage devices for use with DAOS
	StoragePrepare(ctx context.Context, in *StoragePrepareReq, opts ...grpc.CallOption) (*StoragePrepareResp, error)
	// Retrieve details of nonvolatile storage on server, including health info
	StorageScan(ctx context.Context, in *StorageScanReq, opts ...grpc.CallOption) (*StorageScanResp, error)
	// Format nonvolatile storage devices for use with DAOS
	StorageFormat(ctx context.Context, in *StorageFormatReq, opts ...grpc.CallOption) (*StorageFormatResp, error)
	// Query DAOS system status
	SystemQuery(ctx context.Context, in *SystemQueryReq, opts ...grpc.CallOption) (*SystemQueryResp, error)
	// Stop DAOS system (shutdown data-plane instances)
	SystemStop(ctx context.Context, in *SystemStopReq, opts ...grpc.CallOption) (*SystemStopResp, error)
	// ResetFormat DAOS system (restart data-plane instances)
	SystemResetFormat(ctx context.Context, in *SystemResetFormatReq, opts ...grpc.CallOption) (*SystemResetFormatResp, error)
	// Start DAOS system (restart data-plane instances)
	SystemStart(ctx context.Context, in *SystemStartReq, opts ...grpc.CallOption) (*SystemStartResp, error)
	// Perform a fabric scan to determine the available provider, device, NUMA node combinations
	NetworkScan(ctx context.Context, in *NetworkScanReq, opts ...grpc.CallOption) (*NetworkScanResp, error)
	// Retrieve firmware details from storage devices on server
	FirmwareQuery(ctx context.Context, in *FirmwareQueryReq, opts ...grpc.CallOption) (*FirmwareQueryResp, error)
	// Update firmware on storage devices on server
	FirmwareUpdate(ctx context.Context, in *FirmwareUpdateReq, opts ...grpc.CallOption) (*FirmwareUpdateResp, error)
}

type mgmtCtlClient struct {
	cc grpc.ClientConnInterface
}

func NewMgmtCtlClient(cc grpc.ClientConnInterface) MgmtCtlClient {
	return &mgmtCtlClient{cc}
}

func (c *mgmtCtlClient) StoragePrepare(ctx context.Context, in *StoragePrepareReq, opts ...grpc.CallOption) (*StoragePrepareResp, error) {
	out := new(StoragePrepareResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/StoragePrepare", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) StorageScan(ctx context.Context, in *StorageScanReq, opts ...grpc.CallOption) (*StorageScanResp, error) {
	out := new(StorageScanResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/StorageScan", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) StorageFormat(ctx context.Context, in *StorageFormatReq, opts ...grpc.CallOption) (*StorageFormatResp, error) {
	out := new(StorageFormatResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/StorageFormat", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) SystemQuery(ctx context.Context, in *SystemQueryReq, opts ...grpc.CallOption) (*SystemQueryResp, error) {
	out := new(SystemQueryResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/SystemQuery", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) SystemStop(ctx context.Context, in *SystemStopReq, opts ...grpc.CallOption) (*SystemStopResp, error) {
	out := new(SystemStopResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/SystemStop", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) SystemResetFormat(ctx context.Context, in *SystemResetFormatReq, opts ...grpc.CallOption) (*SystemResetFormatResp, error) {
	out := new(SystemResetFormatResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/SystemResetFormat", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) SystemStart(ctx context.Context, in *SystemStartReq, opts ...grpc.CallOption) (*SystemStartResp, error) {
	out := new(SystemStartResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/SystemStart", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) NetworkScan(ctx context.Context, in *NetworkScanReq, opts ...grpc.CallOption) (*NetworkScanResp, error) {
	out := new(NetworkScanResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/NetworkScan", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) FirmwareQuery(ctx context.Context, in *FirmwareQueryReq, opts ...grpc.CallOption) (*FirmwareQueryResp, error) {
	out := new(FirmwareQueryResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/FirmwareQuery", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *mgmtCtlClient) FirmwareUpdate(ctx context.Context, in *FirmwareUpdateReq, opts ...grpc.CallOption) (*FirmwareUpdateResp, error) {
	out := new(FirmwareUpdateResp)
	err := c.cc.Invoke(ctx, "/ctl.MgmtCtl/FirmwareUpdate", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// MgmtCtlServer is the server API for MgmtCtl service.
type MgmtCtlServer interface {
	// Prepare nonvolatile storage devices for use with DAOS
	StoragePrepare(context.Context, *StoragePrepareReq) (*StoragePrepareResp, error)
	// Retrieve details of nonvolatile storage on server, including health info
	StorageScan(context.Context, *StorageScanReq) (*StorageScanResp, error)
	// Format nonvolatile storage devices for use with DAOS
	StorageFormat(context.Context, *StorageFormatReq) (*StorageFormatResp, error)
	// Query DAOS system status
	SystemQuery(context.Context, *SystemQueryReq) (*SystemQueryResp, error)
	// Stop DAOS system (shutdown data-plane instances)
	SystemStop(context.Context, *SystemStopReq) (*SystemStopResp, error)
	// ResetFormat DAOS system (restart data-plane instances)
	SystemResetFormat(context.Context, *SystemResetFormatReq) (*SystemResetFormatResp, error)
	// Start DAOS system (restart data-plane instances)
	SystemStart(context.Context, *SystemStartReq) (*SystemStartResp, error)
	// Perform a fabric scan to determine the available provider, device, NUMA node combinations
	NetworkScan(context.Context, *NetworkScanReq) (*NetworkScanResp, error)
	// Retrieve firmware details from storage devices on server
	FirmwareQuery(context.Context, *FirmwareQueryReq) (*FirmwareQueryResp, error)
	// Update firmware on storage devices on server
	FirmwareUpdate(context.Context, *FirmwareUpdateReq) (*FirmwareUpdateResp, error)
}

// UnimplementedMgmtCtlServer can be embedded to have forward compatible implementations.
type UnimplementedMgmtCtlServer struct {
}

func (*UnimplementedMgmtCtlServer) StoragePrepare(ctx context.Context, req *StoragePrepareReq) (*StoragePrepareResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method StoragePrepare not implemented")
}
func (*UnimplementedMgmtCtlServer) StorageScan(ctx context.Context, req *StorageScanReq) (*StorageScanResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method StorageScan not implemented")
}
func (*UnimplementedMgmtCtlServer) StorageFormat(ctx context.Context, req *StorageFormatReq) (*StorageFormatResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method StorageFormat not implemented")
}
func (*UnimplementedMgmtCtlServer) SystemQuery(ctx context.Context, req *SystemQueryReq) (*SystemQueryResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method SystemQuery not implemented")
}
func (*UnimplementedMgmtCtlServer) SystemStop(ctx context.Context, req *SystemStopReq) (*SystemStopResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method SystemStop not implemented")
}
func (*UnimplementedMgmtCtlServer) SystemResetFormat(ctx context.Context, req *SystemResetFormatReq) (*SystemResetFormatResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method SystemResetFormat not implemented")
}
func (*UnimplementedMgmtCtlServer) SystemStart(ctx context.Context, req *SystemStartReq) (*SystemStartResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method SystemStart not implemented")
}
func (*UnimplementedMgmtCtlServer) NetworkScan(ctx context.Context, req *NetworkScanReq) (*NetworkScanResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method NetworkScan not implemented")
}
func (*UnimplementedMgmtCtlServer) FirmwareQuery(ctx context.Context, req *FirmwareQueryReq) (*FirmwareQueryResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method FirmwareQuery not implemented")
}
func (*UnimplementedMgmtCtlServer) FirmwareUpdate(ctx context.Context, req *FirmwareUpdateReq) (*FirmwareUpdateResp, error) {
	return nil, status.Errorf(codes.Unimplemented, "method FirmwareUpdate not implemented")
}

func RegisterMgmtCtlServer(s *grpc.Server, srv MgmtCtlServer) {
	s.RegisterService(&_MgmtCtl_serviceDesc, srv)
}

func _MgmtCtl_StoragePrepare_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(StoragePrepareReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).StoragePrepare(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/StoragePrepare",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).StoragePrepare(ctx, req.(*StoragePrepareReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_StorageScan_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(StorageScanReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).StorageScan(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/StorageScan",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).StorageScan(ctx, req.(*StorageScanReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_StorageFormat_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(StorageFormatReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).StorageFormat(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/StorageFormat",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).StorageFormat(ctx, req.(*StorageFormatReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_SystemQuery_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(SystemQueryReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).SystemQuery(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/SystemQuery",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).SystemQuery(ctx, req.(*SystemQueryReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_SystemStop_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(SystemStopReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).SystemStop(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/SystemStop",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).SystemStop(ctx, req.(*SystemStopReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_SystemResetFormat_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(SystemResetFormatReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).SystemResetFormat(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/SystemResetFormat",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).SystemResetFormat(ctx, req.(*SystemResetFormatReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_SystemStart_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(SystemStartReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).SystemStart(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/SystemStart",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).SystemStart(ctx, req.(*SystemStartReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_NetworkScan_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(NetworkScanReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).NetworkScan(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/NetworkScan",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).NetworkScan(ctx, req.(*NetworkScanReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_FirmwareQuery_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(FirmwareQueryReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).FirmwareQuery(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/FirmwareQuery",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).FirmwareQuery(ctx, req.(*FirmwareQueryReq))
	}
	return interceptor(ctx, in, info, handler)
}

func _MgmtCtl_FirmwareUpdate_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(FirmwareUpdateReq)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MgmtCtlServer).FirmwareUpdate(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/ctl.MgmtCtl/FirmwareUpdate",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MgmtCtlServer).FirmwareUpdate(ctx, req.(*FirmwareUpdateReq))
	}
	return interceptor(ctx, in, info, handler)
}

var _MgmtCtl_serviceDesc = grpc.ServiceDesc{
	ServiceName: "ctl.MgmtCtl",
	HandlerType: (*MgmtCtlServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "StoragePrepare",
			Handler:    _MgmtCtl_StoragePrepare_Handler,
		},
		{
			MethodName: "StorageScan",
			Handler:    _MgmtCtl_StorageScan_Handler,
		},
		{
			MethodName: "StorageFormat",
			Handler:    _MgmtCtl_StorageFormat_Handler,
		},
		{
			MethodName: "SystemQuery",
			Handler:    _MgmtCtl_SystemQuery_Handler,
		},
		{
			MethodName: "SystemStop",
			Handler:    _MgmtCtl_SystemStop_Handler,
		},
		{
			MethodName: "SystemResetFormat",
			Handler:    _MgmtCtl_SystemResetFormat_Handler,
		},
		{
			MethodName: "SystemStart",
			Handler:    _MgmtCtl_SystemStart_Handler,
		},
		{
			MethodName: "NetworkScan",
			Handler:    _MgmtCtl_NetworkScan_Handler,
		},
		{
			MethodName: "FirmwareQuery",
			Handler:    _MgmtCtl_FirmwareQuery_Handler,
		},
		{
			MethodName: "FirmwareUpdate",
			Handler:    _MgmtCtl_FirmwareUpdate_Handler,
		},
	},
	Streams:  []grpc.StreamDesc{},
	Metadata: "control.proto",
}
