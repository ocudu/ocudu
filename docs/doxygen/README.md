# Doxygen API documentation

Doxygen project for the OCUDU API documentation, plus a Docker Compose setup that builds it without installing Doxygen
locally.

## Structure

```txt
docs/doxygen/
├── .env                     # env file for docker-compose
├── docker-compose.yml       # Docker service that builds the documentation
├── CMakeLists.txt           # doxygen and doxygen-<module> build targets
├── main.dox, acronyms.dox   # hand-written pages
├── header.html, *.css       # theme
└── README.md                # This file
```

## Building with CMake

The Doxygen targets are part of the main build:

```bash
make doxygen
```

## Building with Docker

```bash
docker compose -f docs/doxygen/docker-compose.yml up
```

Select another Doxygen target with the `DOXYGEN_TARGET` environment variable:

```bash
DOXYGEN_TARGET=doxygen-support docker compose -f docs/doxygen/docker-compose.yml up
```

The generated HTML is written to `build_doxygen/docs/doxygen/html`.

### Environment variables

Adjust the variables in `.env` if the defaults do not suit the host:

- `UID`/`GID`: user/group IDs used inside the container, so generated files are owned by the invoking user.
- `DOXYGEN_VERSION`/`PLANTUML_VERSION`: image tag components. They must match `.gitlab/ci/doxygen/version.yml`.
- `CI_REGISTRY_IMAGE`: registry holding the pre-built Doxygen image.
