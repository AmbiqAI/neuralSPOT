// ****************************************************************************
//
//  errno.h
//! @file
//!
//! @brief Standared error code definition for CLANG
//!
//! @{
//
// ****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2025, Ambiq Micro, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// Third party software included in this distribution is subject to the
// additional license terms as defined in the /docs/licenses directory.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision release_sdk5_2_a_1_1-c2486c8ef of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef __METAL_CLANG_ERRNO__H__
#define __METAL_CLANG_ERRNO__H__

#ifdef __cplusplus
extern "C" {
#endif

#define EPERM 1         /**< Not owner */
#define ENOENT 2        /**< No such file or directory */
#define ESRCH 3         /**< No such context */
#define EINTR 4         /**< Interrupted system call */
#define EIO 5           /**< I/O error */
#define ENXIO 6         /**< No such device or address */
#define E2BIG 7         /**< Arg list too long */
#define ENOEXEC 8       /**< Exec format error */
#define EBADF 9         /**< Bad file number */
#define ECHILD 10       /**< No children */
#define EAGAIN 11       /**< No more contexts */
#define ENOMEM 12       /**< Not enough core */
#define EACCES 13       /**< Permission denied */
#define EFAULT 14       /**< Bad address */
#define ENOTBLK 15      /**< Block device required */
#define EBUSY 16        /**< Mount device busy */
#define EEXIST 17       /**< File exists */
#define EXDEV 18        /**< Cross-device link */
#define ENODEV 19       /**< No such device */
#define ENOTDIR 20      /**< Not a directory */
#define EISDIR 21       /**< Is a directory */
#define EINVAL 22       /**< Invalid argument */
#define ENFILE 23       /**< File table overflow */
#define EMFILE 24       /**< Too many open files */
#define ENOTTY 25       /**< Not a typewriter */
#define ETXTBSY 26      /**< Text file busy */
#define EFBIG 27        /**< File too large */
#define ENOSPC 28       /**< No space left on device */
#define ESPIPE 29       /**< Illegal seek */
#define EROFS 30        /**< Read-only file system */
#define EMLINK 31       /**< Too many links */
#define EPIPE 32        /**< Broken pipe */
#define EDOM 33         /**< Argument too large */
#define ERANGE 34       /**< Result too large */
#define ENOMSG 35       /**< Unexpected message type */
#define EDEADLK 45      /**< Resource deadlock avoided */
#define ENOLCK 46       /**< No locks available */
#define ENOSTR 60       /**< STREAMS device required */
#define ENODATA 61      /**< Missing expected message data */
#define ETIME 62        /**< STREAMS timeout occurred */
#define ENOSR 63        /**< Insufficient memory */
#define EPROTO 71       /**< Generic STREAMS error */
#define EBADMSG 77      /**< Invalid STREAMS message */
#define ENOSYS 88       /**< Function not implemented */
#define ENOTEMPTY 90    /**< Directory not empty */
#define ENAMETOOLONG 91 /**< File name too long */
#define ELOOP 92        /**< Too many levels of symbolic links */
#define EOPNOTSUPP 95   /**< Operation not supported on socket */
#define EPFNOSUPPORT 96 /**< Protocol family not supported */
#define ECONNRESET 104   /**< Connection reset by peer */
#define ENOBUFS 105      /**< No buffer space available */
#define EAFNOSUPPORT 106 /**< Addr family not supported */
#define EPROTOTYPE 107   /**< Protocol wrong type for socket */
#define ENOTSOCK 108     /**< Socket operation on non-socket */
#define ENOPROTOOPT 109  /**< Protocol not available */
#define ESHUTDOWN 110    /**< Can't send after socket shutdown */
#define ECONNREFUSED 111 /**< Connection refused */
#define EADDRINUSE 112   /**< Address already in use */
#define ECONNABORTED 113 /**< Software caused connection abort */
#define ENETUNREACH 114  /**< Network is unreachable */
#define ENETDOWN 115     /**< Network is down */
#define ETIMEDOUT 116    /**< Connection timed out */
#define EHOSTDOWN 117    /**< Host is down */
#define EHOSTUNREACH 118 /**< No route to host */
#define EINPROGRESS 119  /**< Operation now in progress */
#define EALREADY 120     /**< Operation already in progress */
#define EDESTADDRREQ 121 /**< Destination address required */
#define EMSGSIZE 122        /**< Message size */
#define EPROTONOSUPPORT 123 /**< Protocol not supported */
#define ESOCKTNOSUPPORT 124 /**< Socket type not supported */
#define EADDRNOTAVAIL 125   /**< Can't assign requested address */
#define ENETRESET 126       /**< Network dropped connection on reset */
#define EISCONN 127         /**< Socket is already connected */
#define ENOTCONN 128        /**< Socket is not connected */
#define ETOOMANYREFS 129    /**< Too many references: can't splice */
#define ENOTSUP 134         /**< Unsupported value */
#define EILSEQ 138          /**< Illegal byte sequence */
#define EOVERFLOW 139       /**< Value overflow */
#define ECANCELED 140       /**< Operation canceled */
#define EWOULDBLOCK EAGAIN /**< Operation would block */

#ifdef __cplusplus
}
#endif

#endif /* __METAL_CLANG_ERRNO__H__ */
