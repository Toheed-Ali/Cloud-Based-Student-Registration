# Build Stage - Use Debian Bookworm for consistent GLIBC
FROM debian:bookworm AS builder

WORKDIR /app

# Install build tools
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copy source code
COPY . .

# Build the project using CMake
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --config Release

# Runtime Stage - Use same Debian version for GLIBC compatibility
FROM debian:bookworm-slim

WORKDIR /app

# Install runtime dependencies (C++ standard library)
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Copy the compiled server executable from builder
COPY --from=builder /app/build/server ./server

# Expose port
EXPOSE 8080

# Run the server
CMD ["./server"]
