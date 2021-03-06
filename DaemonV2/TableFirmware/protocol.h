/*
 * protocol.h
 *
 * Created: 22.04.2017 15:08:48
 *  Author: Tomasz Jaworski
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_



#define RX_PAYLOAD_CAPACITY	32
#define TX_PAYLOAD_CAPACITY	128

enum class ADCBlockType : uint8_t {
	Invalid = 0,	// none
	_1Bits = 1,		// 8 measurements on 1 byte
	_2Bits = 2,		// 4 measurements on 1 byte
	_3Bits = 3,		// 8 measurements on 3 bytes
	_4Bits = 4,		// 2 measurements on 1 byte
	_5Bits = 5,		// 8 measurements on 5 bytes
	_6Bits = 6,		// 4 measurements on 3 bytes
	_7Bits = 7,		// 8 measurements on 7 bytes
	_8Bits = 8,		// 1 measurement on 1 byte - no additional type is needed
	_9Bits = 9,		// 8 measurements on 9 bytes
	_10Bits = 10,	// 4 measurements on 5 bytes
};


struct ADC_BLOCK10_N4_B5 // 4 measurements on 5 bytes
{
	uint16_t b1 : 10;
	uint16_t b2 : 10;
	uint16_t b3 : 10;
	uint16_t b4 : 10;
} __attribute__((packed));

struct ADC_BLOCK9_N8_B9 // 8 measurements on 9 bytes
{
	uint16_t b1 : 9;
	uint16_t b2 : 9;
	uint16_t b3 : 9;
	uint16_t b4 : 9;
	uint16_t b5 : 9;
	uint16_t b6 : 9;
	uint16_t b7 : 9;
	uint16_t b8 : 9;
} __attribute__((packed));

struct ADC_BLOCK8_N1_B1 // 1 measurements on 1 byte
{
	uint8_t b1;
} __attribute__((packed));

struct ADC_BLOCK7_N8_B7 // 8 measurements on 7 bytes
{
	uint16_t b1 : 7;
	uint16_t b2 : 7;
	uint16_t b3 : 7;
	uint16_t b4 : 7;
	uint16_t b5 : 7;
	uint16_t b6 : 7;
	uint16_t b7 : 7;
	uint16_t b8 : 7;
} __attribute__((packed));

struct ADC_BLOCK6_N4_B3 // 4 measurements on 3 bytes
{
	uint16_t v1 : 6;
	uint16_t v2 : 6;
	uint16_t v3 : 6;
	uint16_t v4 : 6;
} __attribute__((packed));

struct ADC_BLOCK5_N8_B5 // 8 measurements on 5 bytes
{
	uint16_t v1 : 5;
	uint16_t v2 : 5;
	uint16_t v3 : 5;
	uint16_t v4 : 5;
	uint16_t v5 : 5;
	uint16_t v6 : 5;
	uint16_t v7 : 5;
	uint16_t v8 : 5;
} __attribute__((packed));

struct ADC_BLOCK4_N2_B1 // 2 measurements on 1 byte
{
	uint8_t v1 : 4;
	uint8_t v2 : 4;
} __attribute__((packed));

struct ADC_BLOCK3_N8_B3 // 8 measurements on 3 bytes
{
	uint16_t v1 : 3;
	uint16_t v2 : 3;
	uint16_t v3 : 3;
	uint16_t v4 : 3;
	uint16_t v5 : 3;
	uint16_t v6 : 3;
	uint16_t v7 : 3;
	uint16_t v8 : 3;
} __attribute__((packed));

struct ADC_BLOCK2_N4_B1 // 4 measurements on 1 byte
{
	uint8_t v1 : 2;
	uint8_t v2 : 2;
	uint8_t v3 : 2;
	uint8_t v4 : 2;
} __attribute__((packed));

struct ADC_BLOCK1_N8_B1 // 8 measurements on 1 byte
{
	uint8_t v1 : 1;
	uint8_t v2 : 1;
	uint8_t v3 : 1;
	uint8_t v4 : 1;
	uint8_t v5 : 1;
	uint8_t v6 : 1;
	uint8_t v7 : 1;
	uint8_t v8 : 1;
} __attribute__((packed));

static_assert(sizeof(ADC_BLOCK10_N4_B5) == 5, "ADC_BLOCK10_N4_B5 has invalid_size");
static_assert(sizeof(ADC_BLOCK9_N8_B9) == 9, "ADC_BLOCK9_N8_B9 has invalid_size");
static_assert(sizeof(ADC_BLOCK7_N8_B7) == 7, "ADC_BLOCK7_N8_B7 has invalid_size");
static_assert(sizeof(ADC_BLOCK6_N4_B3) == 3, "ADC_BLOCK6_N4_B3 has invalid_size");
static_assert(sizeof(ADC_BLOCK5_N8_B5) == 5, "ADC_BLOCK5_N8_B5 has invalid_size");
static_assert(sizeof(ADC_BLOCK4_N2_B1) == 1, "ADC_BLOCK4_N2_B1 has invalid_size");
static_assert(sizeof(ADC_BLOCK3_N8_B3) == 3, "ADC_BLOCK3_N8_B3 has invalid_size");
static_assert(sizeof(ADC_BLOCK2_N4_B1) == 1, "ADC_BLOCK2_N4_B1 has invalid_size");
static_assert(sizeof(ADC_BLOCK1_N8_B1) == 1, "ADC_BLOCK1_N8_B1 has invalid_size");

//
//
//  ###############################################################################################
//
//


enum class MessageType : uint8_t
{
	// 
	// Message types implemented in photo module firmware and triggering firmware
	//
	Invalid = 0x00,
	None = 0x01,

	// Classical ping-pong messages to see if a device is alive and kickin'
	PingRequest = 0x02,
	PingResponse = 0x03,
	
	DeviceIdentifierRequest = 0x04,
	DeviceIdentifierResponse = 0x05,

	RebootRequest = 0x06,
	RebootResponse = 0x07,
	
	//
	// Message types implemented only in photo module firmware
	//
	
	SingleMeasurement8Request = 0x10,
	SingleMeasurement8Response = 0x11,
	SingleMeasurement10Request = 0x12,
	SingleMeasurement10Response = 0x13,
	
	TriggeredMeasurementEnterRequest = 0x21,
	TriggeredMeasurementEnterResponse = 0x23,

	TriggeredMeasurementLeaveRequest = 0x30,
	TriggeredMeasurementLeaveResponse = 0x31,
	
	//
	// Message types implemented only in triggering firmware
	//
	
	SetTriggerStateRequest = 0x41,
	SetTriggerStateResponse = 0x42,
	SetTriggerGeneratorRequest = 0x43,
	SetTriggerGeneratorResponse = 0x44,
	
};

//
//
//
//


typedef unsigned char device_identifier_t;


#define PROTOCOL_HEADER_VALUE   ((uint8_t)0xAB)
#define END_OF_TRIGGER_MARKER   ((char)'T')


//
// Incoming (module <- PC) header structure
struct RX_PROTO_HEADER {
	uint8_t magic;		
	MessageType type;				// type of the received message
	uint8_t payload_length;			//
	
	RX_PROTO_HEADER() : magic(PROTOCOL_HEADER_VALUE) {}
} __attribute__((packed));

//
// Outgoing (module->PC) header structure
struct TX_PROTO_HEADER {
	uint8_t magic;
	MessageType type;				// type of the received message
	uint8_t payload_length;			//
	
	device_identifier_t device_id;
	uint8_t sequence_counter;
	
	TX_PROTO_HEADER() : magic(PROTOCOL_HEADER_VALUE), sequence_counter(0x00) {}
} __attribute__((packed));



enum class TriggerStateSetMode: uint8_t {
	NoAction = 0x01,
	SetLow = 0x02,
	SetHigh = 0x03,
};

enum class TriggerGeneratorSetMode : uint8_t {
	NoAction = 0x01,
	SetAndRun = 0x02,
	TurnOff = 0x03,
};
	
struct TriggerGeneratorPayload {
	struct {
		TriggerGeneratorSetMode mode;
		bool is_single_shot;
		int16_t low_interval;
		int16_t high_interval;
		int16_t echo_delay;
		
	} __attribute__((packed)) trigger1, trigger2;
} __attribute__((packed));

struct TriggerStatePayload {
	TriggerStateSetMode trigger1;
	TriggerStateSetMode trigger2; // Filed .echo_delay for trigger 2 is not used.
} __attribute__((packed));


struct TriggeredMeasurementEnterPayload {
	int8_t data_size;	
} __attribute__((packed));


// data size asserts
static_assert(sizeof(enum MessageType) == 1, "MessageType has invalid size");
static_assert(sizeof(RX_PROTO_HEADER) == 3, "RX_PROTO_HEADER has invalid size");
static_assert(sizeof(TX_PROTO_HEADER) == 5, "RX_PROTO_HEADER has invalid size");

static_assert(sizeof(TriggerGeneratorSetMode) == 1, "TriggerGeneratorSetMode has invalid size");
static_assert(sizeof(TriggerStateSetMode) == 1, "TriggerStateSetMode has invalid size");
static_assert(sizeof(TriggerGeneratorPayload) == 16, "TriggerGeneratorPayload has invalid size");
static_assert(sizeof(TriggerStatePayload) == 2, "TriggerStatePayload has invalid size");

static_assert(sizeof(TriggeredMeasurementEnterPayload) == 1, "TriggeredMeasurementEnterPayload has invalid size");



#endif /* PROTOCOL_H_ */