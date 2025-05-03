#pragma once
#include "ap_int.h"
#include "hls_stream.h"
#include "../axi_utils.hpp"
#include "../ipv4/ipv4.hpp"
#include "../ipv6/ipv6.hpp"

#include "../ib_transport_protocol/ib_transport_protocol.hpp"

template<int width>
size_t count_trailing_zeros(ap_uint<width> value) {
    for (int i = 0; i < width; i++) {
        if (value.test(i)) {
            return i;
        }
    }
    return width;
}

inline void unpack_qp_context(
    hls::stream<ap_uint<160>>& in_stream,
    hls::stream<qpContext>& out_stream
) {
    #pragma HLS INLINE

    ap_uint<160> data = in_stream.read();
    qpContext unpacked;
    ap_uint<6> state_onehot = data(5, 0);
    unpacked.newState = static_cast<qpState>(count_trailing_zeros(state_onehot));
    unpacked.qp_num = data(29, 6);
    unpacked.remote_psn = data(53, 30);
    unpacked.local_psn = data(77, 54);
    unpacked.r_key = data(109, 78);
    unpacked.virtual_address = data(157, 110);

    out_stream.write(unpacked);
}

inline void unpack_if_conn_req(
    hls::stream<ap_uint<184>>& in_stream,
    hls::stream<ifConnReq>& out_stream
) {
    #pragma HLS INLINE

    ap_uint<184> data = in_stream.read();
    ifConnReq unpacked;
    unpacked.qpn = data(15, 0);
    unpacked.remote_qpn = data(39, 16);
    unpacked.remote_ip_address = data(167, 40);
    unpacked.remote_udp_port = data(183, 168);

    out_stream.write(unpacked);
}

inline void unpack_tx_meta(
    hls::stream<ap_uint<240>>& in_stream,
    hls::stream<txMeta>& out_stream
) {
    #pragma HLS INLINE

    ap_uint<240> data = in_stream.read();
    txMeta unpacked;

    ap_uint<18> opcode_oneshot = data(17, 0);
    unpacked.op_code = static_cast<ibOpCode>(count_trailing_zeros(opcode_oneshot));
    unpacked.qpn = data(33, 18);
    unpacked.host = data(34, 34);
    unpacked.lst = data(35, 35);
    unpacked.offs = data(41, 36);
    unpacked.raddr = data(105, 42);
    unpacked.laddr = data(169, 106);
    unpacked.len = data(201, 170);
    unpacked.imm = data(233, 202);

    out_stream.write(unpacked);
}

// This function converts a stream of memCmd to a stream of ap_uint<128>
// It assumes that 'num_cmds' commands will be processed.
inline void convert_memCmd_stream(
    hls::stream<memCmd>&       in_stream,
    hls::stream<ap_uint<128> >& out_stream)
{
#pragma HLS INLINE

        // Read one memCmd from the input stream (blocking read)
        memCmd cmd = in_stream.read();

        // Convert op_code to one-hot encoding (18 bits)
        // (Assumes that op_code values are in the range 0..17)
        ap_uint<18> op_hot = ((ap_uint<18>)1) << cmd.op_code;

        // Create a 128-bit word to hold all packed fields.
        ap_uint<128> packed = 0;
        int bitPos = 0;

        // Pack the one-hot op_code (18 bits)
        packed.range(bitPos + 18 - 1, bitPos) = op_hot;
        bitPos += 18;

        // Pack qpn (16 bits)
        packed.range(bitPos + 16 - 1, bitPos) = cmd.qpn;
        bitPos += 16;

        // Pack lst (1 bit)
        packed.range(bitPos, bitPos) = cmd.lst;
        bitPos += 1;

        // Pack addr (48 bits)
        packed.range(bitPos + 48 - 1, bitPos) = cmd.addr;
        bitPos += 48;

        // Pack dst (4 bits)
        packed.range(bitPos + 4 - 1, bitPos) = cmd.dst;
        bitPos += 4;

        // Pack strm (2 bits)
        packed.range(bitPos + 2 - 1, bitPos) = cmd.strm;
        bitPos += 2;

        // Pack len (28 bits)
        packed.range(bitPos + 28 - 1, bitPos) = cmd.len;
        bitPos += 28;

        // Pack actv (1 bit)
        packed.range(bitPos, bitPos) = cmd.actv;
        bitPos += 1;

        // Pack host (1 bit)
        packed.range(bitPos, bitPos) = cmd.host;
        bitPos += 1;

        // Pack offs (6 bits)
        packed.range(bitPos + 6 - 1, bitPos) = cmd.offs;
        bitPos += 6;

        // At this point, bitPos is 125. The remaining bits [127:125] are left as 0.
        // Write the 128-bit packed word to the output stream.
        out_stream.write(packed);
}
