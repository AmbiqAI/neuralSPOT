/*
 * Generated by erpcgen 1.9.1 on Fri Sep  9 09:53:34 2022.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "GenericDataOperations_PcToEvb_server.h"
#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
#include <new>
#endif
#include "erpc_port.h"
#include "erpc_manually_constructed.hpp"

#if 10901 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#endif

ERPC_MANUALLY_CONSTRUCTED_STATIC(pc_to_evb_service, s_pc_to_evb_service);


//! @brief Function to read struct dataBlock
static void read_dataBlock_struct(erpc::Codec * codec, dataBlock * data);

//! @brief Function to read struct binary_t
static void read_binary_t_struct(erpc::Codec * codec, binary_t * data);


// Read struct dataBlock function implementation
static void read_dataBlock_struct(erpc::Codec * codec, dataBlock * data)
{
    int32_t _tmp_local;

    if(NULL == data)
    {
        return;
    }

    codec->read(&data->length);

    codec->read(&_tmp_local);
    data->dType = static_cast<dataType>(_tmp_local);

    uint32_t description_len;
    char * description_local;
    codec->readString(&description_len, &description_local);
    data->description = (char*) erpc_malloc((description_len + 1) * sizeof(char));
    if ((data->description == NULL) || (description_local == NULL))
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }
    else
    {
        memcpy(data->description, description_local, description_len);
        (data->description)[description_len] = 0;
    }

    codec->read(&_tmp_local);
    data->cmd = static_cast<command>(_tmp_local);

    read_binary_t_struct(codec, &(data->buffer));
}

// Read struct binary_t function implementation
static void read_binary_t_struct(erpc::Codec * codec, binary_t * data)
{
    if(NULL == data)
    {
        return;
    }

    uint8_t * data_local;
    codec->readBinary(&data->dataLength, &data_local);
    data->data = (uint8_t *) erpc_malloc(data->dataLength * sizeof(uint8_t));
    if ((data->data == NULL) && (data->dataLength > 0))
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }
    else
    {
        memcpy(data->data, data_local, data->dataLength);
    }
}


//! @brief Function to write struct dataBlock
static void write_dataBlock_struct(erpc::Codec * codec, const dataBlock * data);

//! @brief Function to write struct binary_t
static void write_binary_t_struct(erpc::Codec * codec, const binary_t * data);


// Write struct dataBlock function implementation
static void write_dataBlock_struct(erpc::Codec * codec, const dataBlock * data)
{
    if(NULL == data)
    {
        return;
    }

    codec->write(data->length);

    codec->write(static_cast<int32_t>(data->dType));

    codec->writeString(strlen((const char*)data->description), (const char*)data->description);

    codec->write(static_cast<int32_t>(data->cmd));

    write_binary_t_struct(codec, &(data->buffer));
}

// Write struct binary_t function implementation
static void write_binary_t_struct(erpc::Codec * codec, const binary_t * data)
{
    if(NULL == data)
    {
        return;
    }

    codec->writeBinary(data->dataLength, data->data);
}


//! @brief Function to free space allocated inside struct dataBlock
static void free_dataBlock_struct(dataBlock * data);

//! @brief Function to free space allocated inside struct binary_t
static void free_binary_t_struct(binary_t * data);


// Free space allocated inside struct dataBlock function implementation
static void free_dataBlock_struct(dataBlock * data)
{
    erpc_free(data->description);

    free_binary_t_struct(&data->buffer);
}

// Free space allocated inside struct binary_t function implementation
static void free_binary_t_struct(binary_t * data)
{
    erpc_free(data->data);
}



// Call the correct server shim based on method unique ID.
erpc_status_t pc_to_evb_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    erpc_status_t erpcStatus;
    switch (methodId)
    {
        case kpc_to_evb_ns_rpc_data_sendBlockToEVB_id:
        {
            erpcStatus = ns_rpc_data_sendBlockToEVB_shim(codec, messageFactory, sequence);
            break;
        }

        case kpc_to_evb_ns_rpc_data_fetchBlockFromEVB_id:
        {
            erpcStatus = ns_rpc_data_fetchBlockFromEVB_shim(codec, messageFactory, sequence);
            break;
        }

        case kpc_to_evb_ns_rpc_data_computeOnEVB_id:
        {
            erpcStatus = ns_rpc_data_computeOnEVB_shim(codec, messageFactory, sequence);
            break;
        }

        default:
        {
            erpcStatus = kErpcStatus_InvalidArgument;
            break;
        }
    }

    return erpcStatus;
}

// Server shim for ns_rpc_data_sendBlockToEVB of pc_to_evb interface.
erpc_status_t pc_to_evb_service::ns_rpc_data_sendBlockToEVB_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    dataBlock *block = NULL;
    block = (dataBlock *) erpc_malloc(sizeof(dataBlock));
    if (block == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }
    status result;

    // startReadMessage() was already called before this shim was invoked.

    read_dataBlock_struct(codec, block);

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = ns_rpc_data_sendBlockToEVB(block);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kpc_to_evb_service_id, kpc_to_evb_ns_rpc_data_sendBlockToEVB_id, sequence);

        codec->write(static_cast<int32_t>(result));

        err = codec->getStatus();
    }

    if (block)
    {
        free_dataBlock_struct(block);
    }
    erpc_free(block);

    return err;
}

// Server shim for ns_rpc_data_fetchBlockFromEVB of pc_to_evb interface.
erpc_status_t pc_to_evb_service::ns_rpc_data_fetchBlockFromEVB_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    dataBlock *block = NULL;
    status result;

    // startReadMessage() was already called before this shim was invoked.

    block = (dataBlock *) erpc_malloc(sizeof(dataBlock));
    if (block == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = ns_rpc_data_fetchBlockFromEVB(block);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kpc_to_evb_service_id, kpc_to_evb_ns_rpc_data_fetchBlockFromEVB_id, sequence);

        write_dataBlock_struct(codec, block);

        codec->write(static_cast<int32_t>(result));

        err = codec->getStatus();
    }

    if (block)
    {
        free_dataBlock_struct(block);
    }
    erpc_free(block);

    return err;
}

// Server shim for ns_rpc_data_computeOnEVB of pc_to_evb interface.
erpc_status_t pc_to_evb_service::ns_rpc_data_computeOnEVB_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    dataBlock *in_block = NULL;
    in_block = (dataBlock *) erpc_malloc(sizeof(dataBlock));
    if (in_block == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }
    dataBlock *result_block = NULL;
    status result;

    // startReadMessage() was already called before this shim was invoked.

    read_dataBlock_struct(codec, in_block);

    result_block = (dataBlock *) erpc_malloc(sizeof(dataBlock));
    if (result_block == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        result = ns_rpc_data_computeOnEVB(in_block, result_block);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kpc_to_evb_service_id, kpc_to_evb_ns_rpc_data_computeOnEVB_id, sequence);

        write_dataBlock_struct(codec, result_block);

        codec->write(static_cast<int32_t>(result));

        err = codec->getStatus();
    }

    if (in_block)
    {
        free_dataBlock_struct(in_block);
    }
    erpc_free(in_block);

    if (result_block)
    {
        free_dataBlock_struct(result_block);
    }
    erpc_free(result_block);

    return err;
}

#if ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_DYNAMIC
erpc_service_t create_pc_to_evb_service()
{
    return new (nothrow) pc_to_evb_service();
}

void destroy_pc_to_evb_service(erpc_service_t service)
{
    if (service)
    {
        delete (pc_to_evb_service *)service;
    }
}
#elif ERPC_ALLOCATION_POLICY == ERPC_ALLOCATION_POLICY_STATIC
erpc_service_t create_pc_to_evb_service()
{
    s_pc_to_evb_service.construct();
    return s_pc_to_evb_service.get();
}

void destroy_pc_to_evb_service()
{
    s_pc_to_evb_service.destroy();
}
#else
#warning "Unknown eRPC allocation policy!"
#endif