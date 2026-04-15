#!/bin/bash
set -euo pipefail

PORT=5033
SRC_FILE="src_1.dat"
DEST_FILE="dest_1.dat"
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

echo "Compiling tcp_client and tcp_server..."
gcc tcp_client.c -o tcp_client
gcc tcp_server.c -o tcp_server

echo "Creating source file..."
head -c 100000 /dev/urandom > "$SRC_FILE"

echo "Running original tcp_server test (single client)..."
./tcp_server &
SERVER_PID=$!

if ! wait_for_server; then
	echo "tcp_server did not start on port $PORT"
	exit 1
fi

./tcp_client "$SRC_FILE" "$DEST_FILE"
cmp "$SRC_FILE" "$DEST_FILE"
echo "tcp_server test passed."

rm -f tcp_server tcp_client