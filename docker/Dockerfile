FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    qt5-default \
    qtbase5-dev \
    qt5-qmake \
    libsodium-dev \
    libqt5sql5-sqlite \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy all source files
COPY . /app

# Build the Encryption Desktop App
WORKDIR /app/encyption_desktop
RUN qmake encryption_desktop.pro && make clean && make

# Setup data persistence
RUN mkdir -p /app/encyption_desktop/db
VOLUME /app/encyption_desktop/db

# Expose port
EXPOSE 8080

# Run the app
CMD ["./Encryption_Desktop"]