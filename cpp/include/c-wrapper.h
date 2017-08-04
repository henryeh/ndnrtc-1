// 
// c-wrapper.h
//
// Copyright (c) 2017 Regents of the University of California
// For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev
//

#ifndef __c_wrapper_h__
#define __c_wrapper_h__

extern "C" {

	typedef void (*LibLog) (const char* message);

	// creates Face
	//		hostname
	//		portnum
	// creates KeyChain
	//		optional: local key storage
	// - runs everything on internal thread
	bool ndnrtc_init(const char* hostname, const char* storagePath, 
		LibLog libLog);

	// deinitializes library (removes connection and frees objects)
	// init can be called again after this
	void ndnrtc_deinit();

	// params
	//	base prefix
	//	settings
	//		sign
	//		faceIO
	//		keychain
	//		face
	//		params
	//			stream name
	//			producer params
	//				segment size
	//				freshness
	//		coder
	//			frame_rate
	//			gop
	//			start_bitrate
	//			max_bitrate
	//			encode_height
	//			encode_width
	//			drop_frames
	//	use FEC
	// void ndnrtc_CreateVideoStreamObject();
}

#endif