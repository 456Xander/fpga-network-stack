#include "hls_stream.h"
#include "rocev2_packing.hpp"
#include <algorithm>
#include <ios>

int main(int argc, char *argv[]) {

	// Qp Context
	ap_uint<160> packed("0x1feb33eb480429d2f855d88d048ebe8240000008");
	std::cout << "Packed value: " << std::hex << packed << "\n";

	hls::stream<ap_uint<160>> packed_stream;
	hls::stream<qpContext> unpacked_stream;

	packed_stream.write(packed);
	unpack_qp_context(packed_stream, unpacked_stream);

	auto unpacked = unpacked_stream.read();
	std::cout << "Unpacked: \n"
	          << "State: " << unpacked.newState << "\nQP: " << unpacked.qp_num
	          << "\nrKey: " << unpacked.r_key
	          << "\nlpsn: " << unpacked.local_psn
	          << "\nrpsn: " << unpacked.remote_psn << "\n";

	// Tx Meta

	ap_uint<240> packed_tx("0x200000000000000000000000000000000000800000400");
	hls::stream<ap_uint<240>> packed_stream_tx;
	hls::stream<txMeta> unpacked_stream_tx;

	packed_stream_tx.write(packed_tx);
	unpack_tx_meta(packed_stream_tx, unpacked_stream_tx);

	auto unpacked_tx = unpacked_stream_tx.read();
	std::cout << "Unpacked: \n"
	          << "OpCode: " << unpacked_tx.op_code
	          << "\nlen: " << unpacked_tx.len
	          << "\nraddr: " << unpacked_tx.raddr
	          << "\nladdr: " << unpacked_tx.laddr
	          << "\nqpn: " << unpacked_tx.qpn << "\nhost: " << unpacked_tx.host
	          << "\nlst: " << unpacked_tx.lst
	          << "\noffset: " << unpacked_tx.offs << "\n";

	// Qp Conn
	ap_uint<184> packed_conn(
	    "0x12b700000000000000000000ffffc0a82a010000040000");
	hls::stream<ap_uint<184>> packed_stream_conn;
	hls::stream<ifConnReq> unpacked_stream_conn;

	packed_stream_conn.write(packed_conn);
	unpack_if_conn_req(packed_stream_conn, unpacked_stream_conn);

	auto unpacked_conn = unpacked_stream_conn.read();
	std::cout << "Unpacked: \n"
	          << "QPN: " << unpacked_conn.qpn
	          << "\nremote QPN: " << unpacked_conn.remote_qpn
	          << "\nAddress: " << unpacked_conn.remote_ip_address
	          << "\nPort: " << unpacked_conn.remote_udp_port << "\n";

	return 0;
}
