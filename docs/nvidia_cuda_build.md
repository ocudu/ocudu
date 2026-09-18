# Building with CUDA acceleration

CUDA acceleration is opt-in at build time. With `ENABLE_CUDA=OFF`, the default, the build, the sources and the tests are
exactly as they are without CUDA support.

This page covers the build prerequisites only. The accelerated components, their configuration and their benchmarks are
documented alongside the code that provides them.

## Prerequisites

- An NVIDIA CUDA Toolkit installation, providing `nvcc` and the CUDA runtime.
- A VkFFT checkout. VkFFT is a header-only library and is not distributed by the common package managers, so it is
  installed manually; see below.

## Installing VkFFT

VkFFT is header-only, so "installing" it means placing a checkout somewhere the build can find it:

```bash
git clone https://github.com/DTolm/VkFFT.git /opt/vkfft
```

The directory passed to the build must contain `vkFFT/vkFFT.h`. The build locates it through the `VKFFT_ROOT` CMake
variable or an environment variable of the same name, falling back to the standard system include directories:

```bash
export VKFFT_ROOT=/opt/vkfft
```

VkFFT is MIT licensed. It is not vendored into this repository.

## Selecting the CUDA architecture

`CMAKE_CUDA_ARCHITECTURES` is mandatory when `ENABLE_CUDA=ON`. It cannot be guessed: `nvcc` needs it to emit device code
for the target GPU, and an incorrect value is not detected until run time, where it surfaces as

```
CUDA Error: no kernel image is available for execution on the device
```

Query the compute capability of the target GPU and drop the period:

```bash
nvidia-smi --query-gpu=gpu_name,compute_cap --format=csv
```

```
name, compute_cap
NVIDIA RTX 5000 Ada Generation Laptop GPU, 8.9
```

A compute capability of 8.9 corresponds to `CMAKE_CUDA_ARCHITECTURES=89`.

## Configuring and building

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_CUDA=ON \
  -DCMAKE_CUDA_ARCHITECTURES=89 \
  -DVKFFT_ROOT=/opt/vkfft

cmake --build build -j $(nproc)
```

Check the resulting cache if a build behaves unexpectedly:

```bash
grep -nE '^ENABLE_CUDA|^CMAKE_CUDA_ARCHITECTURES|^VKFFT_ROOT' build/CMakeCache.txt
```

## Cross-compiling and non-default toolchains

The CUDA host compiler must match the compiler used for the rest of the project. When building for an Arm platform whose
exact core is not known to the compiler, select the closest supported target explicitly with `-DMCPU=<target>` rather
than relying on `-mcpu=native`, which can silently fall back to a generic baseline.
