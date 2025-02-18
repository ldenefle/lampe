/**
 * @file ingestion
 * @author Lucas Denefle - ldenefle@gmail.com
 * @date 2023-12-15 14:27:59
 * @brief
 *
 */

#ifndef INGESTION_H_
#define INGESTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Includes                                                                   */
/******************************************************************************/
#include "zephyr/kernel.h"
#include "zephyr/sys/ring_buffer.h"

#include "command.pb.h"

/******************************************************************************/
/* Global Definitions/Macros                                                  */
/******************************************************************************/
#define INGESTION_PACKET_MAX_SIZE    1024
#define INGESTION_OBSERVERS_MAX_SIZE 3

/******************************************************************************/
/* External Typedefs                                                          */
/******************************************************************************/
enum ingestion_state {
	RCV_LENGTH_HIGH,
	RCV_LENGTH_LOW,
	RCV_DATA,
	PARSING,
};

struct ingestion_transport {
	/* Should return either an error either the size written */
	int (*write)(void *context, uint8_t *data, uint16_t len);
};

struct ingestion_rpc {
	int (*on_command)(ampoule_Command *command, ampoule_Response *response);
};

struct ingestion {
	/* Used by sys work q */
	struct k_work ingest_work;
	struct k_work_delayable timeout_work;

	struct ring_buf rb;

	enum ingestion_state state;

	uint8_t current[INGESTION_PACKET_MAX_SIZE];
	uint16_t expected_size;
	uint16_t bytes_read;

	struct ingestion_transport *transport;
	struct ingestion_rpc *rpc;

	void *transport_context;
};

/******************************************************************************/
/* External Variables                                                         */
/******************************************************************************/

/******************************************************************************/
/* Global Function Prototypes                                                 */
/******************************************************************************/
/**
 * @brief Initialise an ingestion object
 * @params [in] transport - pointer to the transport struct
 * @params [in] rpc - pointer to the rpc struct
 * @return [in] context - void pointer that will be passed to transport
 */
int ingestion_init(struct ingestion *ingestion, struct ingestion_transport *transport,
		   struct ingestion_rpc *rpc, void *context);

/**
 * @brief Feeds chunk to the ingestion layer
 * @params [in] data - pointer to the chunk
 * @return [in] len - length of the chunk
 */
int ingestion_feed(struct ingestion *ingestion, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* INGESTION */
