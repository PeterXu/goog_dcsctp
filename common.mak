###> headers
INCLUDE += -I. -Ithird_party/abseil-cpp


###> sources

## api
API_SRCS = \
	api/task_queue/default_task_queue_factory_stdlib.cc  \
	api/task_queue/task_queue_base.cc \
	api/task_queue/pending_task_safety_flag.cc \
	api/units/data_rate.cc \
	api/units/data_size.cc \
	api/units/frequency.cc \
	api/units/time_delta.cc \
	api/units/timestamp.cc

## rtc_base
RTC_SRCS1 = \
	rtc_base/async_packet_socket.cc \
	rtc_base/async_resolver.cc \
	rtc_base/async_resolver_interface.cc \
	rtc_base/byte_buffer.cc \
	rtc_base/callback_list.cc \
	rtc_base/checks.cc \
	rtc_base/copy_on_write_buffer.cc \
	rtc_base/event.cc \
	rtc_base/event_tracer.cc \
	rtc_base/fake_clock.cc \
	rtc_base/gunit.cc \
	rtc_base/ip_address.cc \
	rtc_base/location.cc \
	rtc_base/logging.cc \
	rtc_base/message_handler.cc \
	rtc_base/net_helper.cc \
	rtc_base/net_helpers.cc \
	rtc_base/network_constants.cc \
	rtc_base/network_route.cc \
	rtc_base/null_socket_server.cc \
	rtc_base/platform_thread.cc \
	rtc_base/platform_thread_types.cc \
	rtc_base/random.cc \
	rtc_base/socket.cc \
	rtc_base/socket_address.cc \
	rtc_base/string_encode.cc \
	rtc_base/string_to_number.cc \
	rtc_base/string_utils.cc \
	rtc_base/system_time.cc \
	rtc_base/task_queue.cc \
	rtc_base/task_queue_stdlib.cc \
	rtc_base/thread.cc \
	rtc_base/time_utils.cc \
	rtc_base/zero_memory.cc

RTC_SRCS2 = \
	rtc_base/containers/flat_tree.cc \
	rtc_base/deprecated/recursive_critical_section.cc \
	rtc_base/internal/default_socket_server.cc \
	rtc_base/network/sent_packet.cc \
	rtc_base/strings/string_builder.cc \
	rtc_base/strings/string_format.cc \
	rtc_base/synchronization/sequence_checker_internal.cc \
	rtc_base/synchronization/yield.cc \
	rtc_base/synchronization/yield_policy.cc \
	rtc_base/third_party/sigslot/sigslot.cc

RTC_SRCS = $(RTC_SRCS1) $(RTC_SRCS2)

## system
SYS_SRCS = \
	system_wrappers/source/clock.cc

## net
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

## pc
PC_SRCS = \
	pc/sctp_utils.cc 



ALL_SRCS = $(API_SRCS) $(RTC_SRCS) $(SYS_SRCS) $(NET_SRCS) $(PC_SRCS)
TMPS = $(ALL_SRCS:.cc=.o)
OBJS = $(TMPS:.c=.o)
DEPS = $(OBJS:.o=.d)



###> targets

TARGET = dcsctp
STATIC_TARGET = lib$(TARGET).a
SHARED_TARGET = lib$(TARGET).$(DYEXT)

all: static


static: $(OBJS)
	@$(AR) $(STATIC_TARGET) $^
	@echo "generate static $(TARGET)"

shared: $(OBJS)
	@$(CXX) $(LDFLAGS) -o $(SHARED_TARGET) $^
	@echo "generate shared $(TARGET)"

clean:
	@$(RM) $(OBJS) $(DEPS)

distclean: clean
	@$(RM) $(STATIC_TARGET)
	@$(RM) $(SHARED_TARGET)



ifneq ($(filter clean,$(MAKECMDGOALS)),clean)
-include $(DEPS)
endif

%.o : %.cc
	$(CXX) $(CFLAGS) $(INCLUDE) $(CPPFLAGS) -c $<  -o $@

%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDE) $(CPPFLAGS) -c $<  -o $@

