#!/bin/bash
set -euo pipefail

PORT=5033
SRC_FILES=("src_1.dat" "src_2.dat" "src_3.dat" "src_4.dat" "src_5.dat" "src_6.dat")
DEST_FILES=("dest_1.dat" "dest_2.dat" "dest_3.dat" "dest_4.dat" "dest_5.dat" "dest_6.dat")
SERVER_PID=""

wait_for_server() {
	for _ in {1..30}; do
		if ! kill -0 "$SERVER_PID" 2>/dev/null; then
			return 1
		fi
		if ss -ltn | grep -q ":$PORT"; then
			return 0
		fi
		sleep 0.1
	done
	return 1
}

clear_port() {
	# Prevent stale listeners from previous runs from breaking this test.
	fuser -k "$PORT"/tcp 2>/dev/null || true
}

stop_server() {
	if [[ -n "${SERVER_PID:-}" ]] && kill -0 "$SERVER_PID" 2>/dev/null; then
		kill "$SERVER_PID" 2>/dev/null || true
		wait "$SERVER_PID" 2>/dev/null || true
	fi
	SERVER_PID=""
}

cleanup() {
	stop_server
}

trap cleanup EXIT

clear_port

echo "Compiling tcp_client and tcp_server_mt..."
gcc tcp_client.c -o tcp_client
gcc tcp_server_mt.c -o tcp_server_mt -pthread

echo "Creating source files..."
for src_file in "${SRC_FILES[@]}"; do
	head -c 100000 /dev/urandom > "$src_file"
done

echo "Running tcp_server_mt test (6 concurrent clients)..."
./tcp_server_mt &
SERVER_PID=$!

if ! wait_for_server; then
	echo "tcp_server_mt did not start on port $PORT"
	exit 1
fi

CLIENT_PIDS=()
for i in "${!SRC_FILES[@]}"; do
	./tcp_client "${SRC_FILES[$i]}" "${DEST_FILES[$i]}" &
	CLIENT_PIDS+=("$!")
done

for pid in "${CLIENT_PIDS[@]}"; do
	wait "$pid"
done

echo "Verifying transfers..."
for i in "${!SRC_FILES[@]}"; do
	cmp "${SRC_FILES[$i]}" "${DEST_FILES[$i]}"
done

echo "tcp_server_mt concurrent test passed for 6 clients."

rm -f tcp_server_mt tcp_client
