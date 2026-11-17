# Open5GS Container

This is a all-in-one Docker container for Open5GS. At build, the container will use the specified version of the open5gs
repository (default v2.7.6 ). To run the latest tag of the open5gs repository
([https://github.com/open5gs/open5gs/tags](https://github.com/open5gs/open5gs/tags)), line 51 and 52 in .Dockerfile

```Dockerfile
# get latest open5gs tag (must be stored in a file, because docker does not allow to use the return value directly)
# RUN git ls-remote --tags https://github.com/open5gs/open5gs | sort -t '/' -k 3 -V | awk -F/ '{ print $3 }' | awk '!/\^\{\}/' | tail -n 1 > ./open5gsversion
```

must be uncommented.

## Container Parameters

In [open5gs.env](open5gs.env) the following parameters can be set:

- MONGODB_IP (default: 127.0.0.1): This is the IP of the mongodb to use. 127.0.0.1 is the mongodb that runs inside this
  container.
- SUBSCRIBER_DB (default:
  "001010123456780,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,8000,9,10.45.1.2"): This adds
  subscriber data for a single or multiple users to the Open5GS mongodb. It contains either:
  - Comma separated string with information to define a subscriber
  - `subscriber_db.csv`. This is a csv file that contains entries to add to open5gs mongodb. Each entry will represent a
    subscriber. It must be stored in `docker/open5gs/`
- OPEN5GS_IP: This must be set to the IP of the container (here: 10.53.1.2).
- UE_IP_BASE: Defines the IP base used for connected UEs (here: 10.45.0).
- DEBUG (default: false): This can be set to true to run Open5GS in debug mode.

```csv
# Kept in the following format: "Name,IMSI,Key,OP_Type,OP/OPc,AMF,QCI,IP_alloc[,SST,SD]"
#
# Name:     Human readable name to help distinguish UE's. Ignored by the HSS
# IMSI:     UE's IMSI value
# Key:      UE's key, where other keys are derived from. Stored in hexadecimal
# OP_Type:  Operator's code type, either OP or OPc
# OP/OPc:   Operator Code/Cyphered Operator Code, stored in hexadecimal
# AMF:      Authentication management field, stored in hexadecimal
# QCI:      QoS Class Identifier for the UE's default bearer.
# IP_alloc: IP allocation stratagy for the SPGW.
#           With 'dynamic' the SPGW will automatically allocate IPs
#           With a valid IPv4 (e.g. '10.45.0.2') the UE will have a statically assigned IP.
# SST:      Optional. Slice/Service Type the UE is subscribed to (0-255, default: 1).
# SD:       Optional. Slice Differentiator, 6 hexadecimal digits (e.g. '111111'). Without it the subscription has no SD.
#           The slice must be one of the `s_nssai` of the AMF in open5gs-5gc.yml for the UE to register.
#
# Note: Lines starting by '#' are ignored and will be overwritten
# List of UEs with IMSI, and key increasing by one for each new UE. Useful for testing with AmariUE simulator and ue_count option
ue01,001010123456789,00112233445566778899aabbccddeeff,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.1.2
ue02,001010123456790,00112233445566778899aabbccddef00,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.2.2
ue03,001010123456791,00112233445566778899aabbccddef01,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.3.2
# ue04 is subscribed to the slice with SST 1 and SD 111111 instead of SST 1 without SD:
ue04,001010123456792,00112233445566778899aabbccddef02,opc,63bfa50ee6523365ff14c1f45f88737d,9001,9,10.45.4.2,1,111111
```

The subscriber string of `SUBSCRIBER_DB` takes the same optional `SST,SD` at the end.

### Changing the slice of a running UE

[set_ue_slice.sh](set_ue_slice.sh) creates or updates a subscriber in the running container, without restarting it:

```bash
./set_ue_slice.sh 001010123456780 1 111111   # IMSI, SST and optional SD
```

The key, OPc, AMF, QCI and IP can be changed with the environment variables `KEY`, `OPC`, `AMF`, `QCI` and `IP_ALLOC`,
and the container with `CONTAINER` (default: `open5gs_5gc`). The whole subscriber is written, not only its slice: what
is not set goes back to its default, so a UE that has another key, OPc, AMF, QCI or IP needs the same values, and it
only keeps the sessions that `add_users.py` writes (`internet` and `ims`). Subscribers that are attached at the same
time need different `IMSI` and `IP_ALLOC`. The change applies to the next registration of the UE. The subscribers that
`SUBSCRIBER_DB` lists are written again at every start of the container, so they go back to the slice of
`SUBSCRIBER_DB`.

The subscriber is always written with an OPc, never with an OP. A UE that is listed in `SUBSCRIBER_DB` with OP type `op`
needs its OPc, derived from its OP and key, in `OPC`, or it fails to authenticate after the update.

## Open5GS Parameters

Open5GS can be setup using [open5gs-5gc.yml](open5gs-5gc.yml).

## Usage

Create a Docker network to assign a specified IP to the Open5GS conainer (here: 10.53.1.2):

`docker network create --subnet=10.53.1.0/16 open5gsnet`

Build the Docker container using:

`docker build --target open5gs -t open5gs-docker .`

You can overwrite open5gs version by adding `--build-arg OPEN5GS_VERSION=v2.6.6`

Then run the docker container with:

```bash
docker run --name open5gs_5gc --net open5gsnet --ip 10.53.1.2 --env-file open5gs.env --privileged \
    --publish 9999:9999 -v open5gs-db:/data/db open5gs-docker ./build/tests/app/5gc -c open5gs-5gc.yml
```

To use this container with ocudu, the `addrs` option under `amf` section in gnb configuration must be set to OPEN5GS_IP
(here: 10.53.1.2). It could also be required to modify `bind_addrs` option under `amf` section in gnb configuration to
the local ethernet/wifi IP address for the host or container where gnb is running, not a localhost IP.

To ping a connected UE setup the necessary route to the UE_IP_BASE + ".0/24" (here: 10.45.0) via the OPEN5GS_IP (here:
10.53.1.2) using:

`sudo ip ro add 10.45.0.0/16 via 10.53.1.2`

## Note

The Open5GS WebUI to manually add/change UEs to the mongodb can be accessed at [localhost:9999](localhost:9999).

The mongodb is stored in the `open5gs-db` docker volume, so the subscribers, including the ones added with the WebUI,
are kept when the container is recreated. At every start the subscribers of `SUBSCRIBER_DB` are added or updated, but
the ones removed from it are not deleted. To start from an empty database, remove the container and then the volume:
`docker compose down` followed by `docker volume rm docker_open5gs-db` (`docker_` is the name of the compose project),
or `docker rm -f open5gs_5gc` followed by `docker volume rm open5gs-db` if the container was started with `docker run`.
A volume cannot be removed while a container, even a stopped one, uses it. Do not use `docker compose down --volumes`
for this, as it also removes `gnb-storage`. This resets the local mongodb only, not a remote one set with `MONGODB_IP`.

The volume starts empty, so the subscribers that only exist in the database of a container created before it was added
are not kept the first time the container is recreated. Note their details from the WebUI before recreating it, then add
them again with the WebUI or in `SUBSCRIBER_DB`.

The image runs MongoDB 7.0, in preparation for MongoDB 8.0, which a future update of the Ubuntu image to a newer release
requires (Ubuntu 24.04 and later only have MongoDB 8.0). MongoDB only opens the data of the previous major version, so a
volume has to run MongoDB 7.0 before it can be opened by an image with MongoDB 8.0. For this, at every start the
entrypoint sets the feature compatibility version of the local database to the one of its mongod, so a volume created by
an image with MongoDB 6.0 is migrated at its first start. This cannot be undone: an image with MongoDB 6.0 does not open
the volume afterwards. Start this image at least once on a volume before updating to the one with MongoDB 8.0. A volume
that holds data of MongoDB 5.0 or older, which this image did not create, first has to be opened with MongoDB 6.0 and
have its feature compatibility version set to 6.0, as MongoDB 7.0 does not start on it and the entrypoint then waits for
it.
