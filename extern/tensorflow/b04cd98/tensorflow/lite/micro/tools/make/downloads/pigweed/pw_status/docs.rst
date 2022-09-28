.. _module-pw_status:

---------
pw_status
---------
``pw_status`` provides features for communicating the result of an operation.
The classes in ``pw_status`` are used extensively throughout Pigweed.

pw::Status
==========
The primary feature of ``pw_status`` is the ``pw::Status`` class.
``pw::Status`` (``pw_status/status.h``) is a simple, zero-overhead status
object that wraps a status code.

``pw::Status`` uses Google's standard status codes (see the `Google APIs
repository <https://github.com/googleapis/googleapis/blob/master/google/rpc/code.proto>`_).
These codes are used extensively in Google projects including `Abseil
<https://abseil.io>`_ (`status/status.h
<https://cs.opensource.google/abseil/abseil-cpp/+/master:absl/status/status.h>`_
) and `gRPC <https://grpc.io>`_ (`doc/statuscodes.md
<https://github.com/grpc/grpc/blob/master/doc/statuscodes.md>`_).

A ``Status`` is created with a ``static constexpr`` member function
corresponding to the code.

.. code-block:: cpp

  // Ok (gRPC code "OK") does not indicate an error; this value is returned on
  // success. It is typical to check for this value before proceeding on any
  // given call across an API or RPC boundary. To check this value, use the
  // `Status::ok()` member function rather than inspecting the raw code.
  Status::Ok()

  // Cancelled (gRPC code "CANCELLED") indicates the operation was cancelled,
  // typically by the caller.
  Status::Cancelled()

  // Unknown (gRPC code "UNKNOWN") indicates an unknown error occurred. In
  // general, more specific errors should be raised, if possible. Errors raised
  // by APIs that do not return enough error information may be converted to
  // this error.
  Status::Unknown()

  // InvalidArgument (gRPC code "INVALID_ARGUMENT") indicates the caller
  // specified an invalid argument, such a malformed filename. Note that such
  // errors should be narrowly limited to indicate to the invalid nature of the
  // arguments themselves. Errors with validly formed arguments that may cause
  // errors with the state of the receiving system should be denoted with
  // `FailedPrecondition` instead.
  Status::InvalidArgument()

  // DeadlineExceeded (gRPC code "DEADLINE_EXCEEDED") indicates a deadline
  // expired before the operation could complete. For operations that may change
  // state within a system, this error may be returned even if the operation has
  // completed successfully. For example, a successful response from a server
  // could have been delayed long enough for the deadline to expire.
  Status::DeadlineExceeded()

  // NotFound (gRPC code "NOT_FOUND") indicates some requested entity (such as
  // a file or directory) was not found.
  //
  // `NotFound` is useful if a request should be denied for an entire class of
  // users, such as during a gradual feature rollout or undocumented allow list.
  // If, instead, a request should be denied for specific sets of users, such as
  // through user-based access control, use `PermissionDenied` instead.
  Status::NotFound()

  // AlreadyExists (gRPC code "ALREADY_EXISTS") indicates the entity that a
  // caller attempted to create (such as file or directory) is already present.
  Status::AlreadyExists()

  // PermissionDenied (gRPC code "PERMISSION_DENIED") indicates that the caller
  // does not have permission to execute the specified operation. Note that this
  // error is different than an error due to an *un*authenticated user. This
  // error code does not imply the request is valid or the requested entity
  // exists or satisfies any other pre-conditions.
  //
  // `PermissionDenied` must not be used for rejections caused by exhausting
  // some resource. Instead, use `ResourceExhausted` for those errors.
  // `PermissionDenied` must not be used if the caller cannot be identified.
  // Instead, use `Unauthenticated` for those errors.
  Status::PermissionDenied()

  // ResourceExhausted (gRPC code "RESOURCE_EXHAUSTED") indicates some resource
  // has been exhausted, perhaps a per-user quota, or perhaps the entire file
  // system is out of space.
  Status::ResourceExhausted()

  // FailedPrecondition (gRPC code "FAILED_PRECONDITION") indicates that the
  // operation was rejected because the system is not in a state required for
  // the operation's execution. For example, a directory to be deleted may be
  // non-empty, an "rmdir" operation is applied to a non-directory, etc.
  //
  // Some guidelines that may help a service implementer in deciding between
  // `FailedPrecondition`, `Aborted`, and `Unavailable`:
  //
  //  (a) Use `Unavailable` if the client can retry just the failing call.
  //  (b) Use `Aborted` if the client should retry at a higher transaction
  //      level (such as when a client-specified test-and-set fails, indicating
  //      the client should restart a read-modify-write sequence).
  //  (c) Use `FailedPrecondition` if the client should not retry until
  //      the system state has been explicitly fixed. For example, if an "rmdir"
  //      fails because the directory is non-empty, `FailedPrecondition`
  //      should be returned since the client should not retry unless
  //      the files are deleted from the directory.
  Status::FailedPrecondition()

  // Aborted (gRPC code "ABORTED") indicates the operation was aborted,
  // typically due to a concurrency issue such as a sequencer check failure or a
  // failed transaction.
  //
  // See the guidelines above for deciding between `FailedPrecondition`,
  // `Aborted`, and `Unavailable`.
  Status::Aborted()

  // OutOfRange (gRPC code "OUT_OF_RANGE") indicates the operation was
  // attempted past the valid range, such as seeking or reading past an
  // end-of-file.
  //
  // Unlike `InvalidArgument`, this error indicates a problem that may
  // be fixed if the system state changes. For example, a 32-bit file
  // system will generate `InvalidArgument` if asked to read at an
  // offset that is not in the range [0,2^32-1], but it will generate
  // `OutOfRange` if asked to read from an offset past the current
  // file size.
  //
  // There is a fair bit of overlap between `FailedPrecondition` and
  // `OutOfRange`.  We recommend using `OutOfRange` (the more specific
  // error) when it applies so that callers who are iterating through
  // a space can easily look for an `OutOfRange` error to detect when
  // they are done.
  Status::OutOfRange()

  // Unimplemented (gRPC code "UNIMPLEMENTED") indicates the operation is not
  // implemented or supported in this service. In this case, the operation
  // should not be re-attempted.
  Status::Unimplemented()

  // Internal (gRPC code "INTERNAL") indicates an internal error has occurred
  // and some invariants expected by the underlying system have not been
  // satisfied. This error code is reserved for serious errors.
  Status::Internal()

  // Unavailable (gRPC code "UNAVAILABLE") indicates the service is currently
  // unavailable and that this is most likely a transient condition. An error
  // such as this can be corrected by retrying with a backoff scheme. Note that
  // it is not always safe to retry non-idempotent operations.
  //
  // See the guidelines above for deciding between `FailedPrecondition`,
  // `Aborted`, and `Unavailable`.
  Status::Unavailable()

  // DataLoss (gRPC code "DATA_LOSS") indicates that unrecoverable data loss or
  // corruption has occurred. As this error is serious, proper alerting should
  // be attached to errors such as this.
  Status::DataLoss()

  // Unauthenticated (gRPC code "UNAUTHENTICATED") indicates that the request
  // does not have valid authentication credentials for the operation. Correct
  // the authentication and try again.
  Status::Unauthenticated()

.. attention::

  Some code may use all-caps status values such as ``Status::UNKNOWN`` instead
  of ``Status::Unknown()``. These all-caps status codes are deprecated and will
  be removed in the future. Do not use them; use the functions above instead.

  The all-caps status aliases were deprecated because they do not comply with
  the style guide and potentially conflict with macro definitions. For example,
  projects might define an ``INTERNAL`` macro, which would prevent ``status.h``
  or code that uses ``Status::INTERNAL`` from compiling.

  The Python tool ``pw_status/update_style.py`` may be used to migrate code in a
  Git repo to the new status style.

C compatibility
---------------
``pw_status`` provides the C-compatible ``pw_Status`` enum for the status codes.
For ease of use, ``pw::Status`` implicitly converts to and from ``pw_Status``.
However, the ``pw_Status`` enum should never be used in C++; instead use the
``Status`` class.

The values of the ``pw_Status`` enum are all-caps and prefixed with
``PW_STATUS_``. For example, ``PW_STATUS_DATA_LOSS`` corresponds with the C++
``Status::DataLoss()``.

StatusWithSize
==============
``pw::StatusWithSize`` (``pw_status/status_with_size.h``) is a convenient,
efficient class for reporting a status along with an unsigned integer value.
It is similar to the ``pw::Result<T>`` class, but it stores both a size and a
status, regardless of the status value, and only supports a limited range (27
bits).

``pw::StatusWithSize`` values may be created with functions similar to
``pw::Status``. For example,

  .. code-block:: cpp

    // An OK StatusWithSize with a size of 123.
    StatusWithSize::Ok(123)

    // A NOT_FOUND StatusWithSize with a size of 0.
    StatusWithSize::NotFound()

    // A RESOURCE_EXHAUSTED StatusWithSize with a size of 10.
    StatusWithSize::ResourceExhausted(10)

PW_TRY
======
``PW_TRY`` (``pw_status/try.h``) is a convenient set of macros for working
with Status and StatusWithSize objects in functions that return Status or
StatusWithSize. The PW_TRY and PW_TRY_WITH_SIZE macros call a function and
do an early return if the function's return status is not ok.

Example:

.. code-block:: cpp

  Status PwTryExample() {
    PW_TRY(FunctionThatReturnsStatus());
    PW_TRY(FunctionThatReturnsStatusWithSize());

    // Do something, only executed if both functions above return OK.
  }

  StatusWithSize PwTryWithSizeExample() {
    PW_TRY_WITH_SIZE(FunctionThatReturnsStatus());
    PW_TRY_WITH_SIZE(FunctionThatReturnsStatusWithSize());

    // Do something, only executed if both functions above return OK.
  }

PW_TRY_ASSIGN is for working with StatusWithSize objects in in functions
that return Status. It is similar to PW_TRY with the addition of assigning
the size from the StatusWithSize on ok.

.. code-block:: cpp

  Status PwTryAssignExample() {
    size_t size_value
    PW_TRY_ASSIGN(size_value, FunctionThatReturnsStatusWithSize());

    // Do something that uses size_value. size_value is only assigned and this
    // following code executed if the PW_TRY_ASSIGN function above returns OK.
  }

Compatibility
=============
C++11
