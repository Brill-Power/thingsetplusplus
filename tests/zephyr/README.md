# Zephyr Client/Server Unit Tests

Tests for Zephyr CAN client and server.

Note that the board overlays folder (`overlays`) is so named to hide it from
the Twister tests that run in CI, where SocketCAN is not available so the loopback
interface is used instead.

To run the tests using the board overlays, run:

```
west build -b native_sim/native/64 --extra-dtc-overlay overlays/native_sim_64.overlay -t run
```
