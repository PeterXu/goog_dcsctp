###> headers
INCLUDE += -I. -Ithird_party/abseil-cpp


###> sources

## net/dcsctp
NET_SRCS = \
	net/dcsctp/packet/chunk_validators.cc \
	net/dcsctp/packet/crc32c.cc \
	net/dcsctp/packet/sctp_packet.cc \
	net/dcsctp/packet/tlv_trait.cc \
	net/dcsctp/public/dcsctp_handover_state.cc \
	net/dcsctp/public/dcsctp_socket_factory.cc \
	net/dcsctp/public/text_pcap_packet_observer.cc \
	net/dcsctp/rx/data_tracker.cc \
	net/dcsctp/rx/interleaved_reassembly_streams.cc \
	net/dcsctp/rx/reassembly_queue.cc \
	net/dcsctp/rx/reassembly_streams.cc \
	net/dcsctp/rx/traditional_reassembly_streams.cc \
	net/dcsctp/socket/callback_deferrer.cc \
	net/dcsctp/socket/dcsctp_socket.cc \
	net/dcsctp/socket/heartbeat_handler.cc \
	net/dcsctp/socket/packet_sender.cc \
	net/dcsctp/socket/state_cookie.cc \
	net/dcsctp/socket/stream_reset_handler.cc \
	net/dcsctp/socket/transmission_control_block.cc \
	net/dcsctp/timer/task_queue_timeout.cc \
	net/dcsctp/timer/timer.cc \
	net/dcsctp/tx/outstanding_data.cc \
	net/dcsctp/tx/retransmission_error_counter.cc \
	net/dcsctp/tx/retransmission_queue.cc \
	net/dcsctp/tx/retransmission_timeout.cc \
	net/dcsctp/tx/rr_send_queue.cc \
	net/dcsctp/tx/stream_scheduler.cc



### all sources
SRCS = $(NET_SRCS)

TMPS = $(SRCS:.cc=.o)
OBJS = $(TMPS:.c=.o)

.cc.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<  -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $<  -o $@


###> targets

TARGET = dcsctp
STATIC_TARGET = lib$(TARGET).a
SHARED_TARGET = lib$(TARGET).$(DYEXT)

all: static

static: $(OBJS)
	@$(AR) $(STATIC_TARGET) $^
	@echo "generate static $(TARGET)"

shared: $(OBJS)
	@$(CXX) $(DFLAGS) -o $(SHARED_TARGET) $^ $(LDFLAGS)
	@echo "generate shared $(TARGET)"

clean:
	@$(RM) $(OBJS)

distclean: clean
	@$(RM) $(STATIC_TARGET)
	@$(RM) $(SHARED_TARGET)

