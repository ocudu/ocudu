# Amarisoft DIFI support

Amarisoft's UE can be interfaced with the `ocudu` gNB's DIFI radio using a custom TRX driver that carries baseband as
DIFI (IEEE-ISTO Std 4900) packets over UDP, without the need of any RF device.

The driver is self-contained: it needs only the Amarisoft TRX driver header, not ZeroMQ or any `ocudu` library. In order
to enable its compilation, the TRX driver header path must be provided:

```bash
cmake .. -DENABLE_TRX_DIFI_DRIVER=TRUE -DTRX_DRIVER_DIR=<path to trx_uhd-...>
make trx_ocudu_difi_test
ctest -R trx_ocudu_difi_test
```

Make sure CMake finds the file `trx_driver.h` in the specified folder. CMake must print a similar line:

```txt
-- Found trx_driver.h in TRX_DRIVER_DIR=/home/user/amarisoft/2026-09-11/trx_uhd-2026-09-11/trx_driver.h
```

The driver requires TRX API version 15, which the Amarisoft releases from 2025-09-04 to 2026-10-26 use. Against another
version it refuses to load and prints both version numbers.

A symbolic link must be done for the UE application to load the driver:

```bash
cd <path to lteue-linux-...>
ln -s <path to ocudu build directory>/utils/trx_ocudu_difi/libtrx_ocudu_difi.so trx_ocudu_difi.so
```

And finally, the UE's RF configuration shall be:

```cfg
  rf_driver: {
    // Select the ocudu DIFI TRX driver.
    name: "ocudu_difi",

    // Comma-separated key=value arguments, in the same form as the gNB's ru_sdr device_args.
    args: "rx_addr=127.0.0.1,rx_port=4991,tx_addr=127.0.0.1,tx_port=4992,srate=30.72e6",
  },
```

The gNB names its ports from its own point of view, so they cross over between the two ends. The matching gNB
configuration is:

```yaml
ru_sdr:
  device_driver: difi
  device_args: tx_addr=127.0.0.1,tx_port=4991,rx_addr=127.0.0.1,rx_port=4992
  srate: 30.72
```

Each antenna travels as a DIFI stream of its own, on its own UDP port. With several antennas, the ports are listed per
antenna as `rx_port0`, `rx_port1`, ... and `tx_port0`, `tx_port1`, ..., on both ends.

The supported arguments are:

| Key             | Default     | Description                                                                           |
| --------------- | ----------- | ------------------------------------------------------------------------------------- |
| `rx_addr`       | `0.0.0.0`   | Local address the downlink is received on.                                            |
| `rx_port<n>`    | `4991`      | UDP port antenna `n` is received on. `rx_port` is antenna 0.                          |
| `tx_addr`       | `127.0.0.1` | Address of the gNB the uplink is sent to.                                             |
| `tx_port<n>`    | `4992`      | UDP port antenna `n` is sent to. `tx_port` is antenna 0.                              |
| `srate`         | derived     | Sample rate in Hz. Must match the gNB's `ru_sdr.srate`.                               |
| `stream_id`     | `0`         | DIFI stream identifier of antenna 0. Antenna `n` uses `stream_id + n`.                |
| `bit_depth`     | `16`        | IQ component size in bits, 8 or 16.                                                   |
| `iq_byte_order` | `big`       | Byte order of the 16-bit IQ payload, `big` or `little`. Must match the gNB's.         |
| `pkt_samples`   | `1920`      | IQ samples per transmitted data packet.                                               |
| `ctx_interval`  | `100`       | Data packets between context packets.                                                 |
| `rcvbuf_mb`     | `16`        | Requested UDP receive buffer size in MB, capped by the kernel at `net.core.rmem_max`. |
| `rx_timeout_ms` | `1000`      | How long a read waits for the gNB's samples before returning none.                    |
| `tx_scale`      | `32767`     | Scale from the UE's floating point samples to 16-bit integers.                        |
| `rx_scale`      | `1/32768`   | Scale from 16-bit integers to the UE's floating point samples.                        |
| `timing_log`    | `0`         | When 1, logs read waits and transmit advance once a second.                           |

Unknown keys are rejected, so a mistyped key stops the UE rather than leaving the link half-configured.
