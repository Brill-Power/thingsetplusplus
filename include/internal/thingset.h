#pragma once

/* Function codes (binary mode) */
#define THINGSET_BIN_GET    0x01 /**< Function code for GET request in binary mode. */
#define THINGSET_BIN_EXEC   0x02 /**< Function code for EXEC request in binary mode. */
#define THINGSET_BIN_DELETE 0x04 /**< Function code for DELETE request in binary mode. */
#define THINGSET_BIN_FETCH  0x05 /**< Function code for FETCH request in binary mode. */
#define THINGSET_BIN_CREATE 0x06 /**< Function code for CREATE request in binary mode. */
#define THINGSET_BIN_UPDATE 0x07 /**< Function code for UPDATE request in binary mode. */
#define THINGSET_BIN_DESIRE 0x1D /**< Function code for DESIRE in binary mode. */
#define THINGSET_BIN_REPORT 0x1F /**< Function code for REPORT in binary mode. */

/* Function codes (text mode) */
#define THINGSET_TXT_GET_FETCH '?' /**< Function code for GET and FETCH requests in text mode. */
#define THINGSET_TXT_EXEC      '!' /**< Function code for EXEC request in text mode. */
#define THINGSET_TXT_DELETE    '-' /**< Function code for DELETE request in text mode. */
#define THINGSET_TXT_CREATE    '+' /**< Function code for CREATE request in text mode. */
#define THINGSET_TXT_UPDATE    '=' /**< Function code for UPDATE request in text mode. */
#define THINGSET_TXT_DESIRE    '@' /**< Function code for DESIRE in text mode. */
#define THINGSET_TXT_REPORT    '#' /**< Function code for REPORT in text mode. */

/* Status codes (success) */
#define THINGSET_STATUS_CREATED 0x81 /**< Response code for successful CREATE requests. */
#define THINGSET_STATUS_DELETED 0x82 /**< Response code for successful DELETE requests. */
#define THINGSET_STATUS_CHANGED 0x84 /**< Response code for successful EXEC/UPDATE requests. */
#define THINGSET_STATUS_CONTENT 0x85 /**< Response code for successful GET/FETCH requests. */

/* Status codes (client errors) */
#define THINGSET_ERR_BAD_REQUEST        0xA0 /**< Error code: Bad request. */
#define THINGSET_ERR_UNAUTHORIZED       0xA1 /**< Error code: Authentication needed. */
#define THINGSET_ERR_FORBIDDEN          0xA3 /**< Error code: Access forbidden. */
#define THINGSET_ERR_NOT_FOUND          0xA4 /**< Error code: Data object not found. */
#define THINGSET_ERR_METHOD_NOT_ALLOWED 0xA5 /**< Error code: Method not allowed. */
#define THINGSET_ERR_REQUEST_INCOMPLETE 0xA8 /**< Error code: Request incomplete. */
#define THINGSET_ERR_CONFLICT           0xA9 /**< Error code: Conflict. */
#define THINGSET_ERR_REQUEST_TOO_LARGE  0xAD /**< Error code: Request not fitting into buffer. */
#define THINGSET_ERR_UNSUPPORTED_FORMAT 0xAF /**< Error code: Format for an item not supported. */

/* Status codes (server errors) */
#define THINGSET_ERR_INTERNAL_SERVER_ERR 0xC0 /**< Error code: Generic catch-all response. */
#define THINGSET_ERR_NOT_IMPLEMENTED     0xC1 /**< Error code: Request method not implemented. */
#define THINGSET_ERR_GATEWAY_TIMEOUT     0xC4 /**< Error code: Node cannot be reached. */
#define THINGSET_ERR_NOT_A_GATEWAY       0xC5 /**< Error code: Node is not a gateway. */