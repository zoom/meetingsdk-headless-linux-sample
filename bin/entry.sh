#!/usr/bin/env bash

# directory for CMake output
BUILD=build

# directory for application output
mkdir -p out

setup-pulseaudio() {
  # Enable dbus
  if [[  ! -d /var/run/dbus ]]; then
    mkdir -p /var/run/dbus
    dbus-uuidgen > /var/lib/dbus/machine-id
    dbus-daemon --config-file=/usr/share/dbus-1/system.conf --print-address
  fi

  usermod -G pulse-access,audio root

  # Cleanup to be "stateless" on startup, otherwise pulseaudio daemon can't start
  rm -rf /var/run/pulse /var/lib/pulse /root/.config/pulse/
  mkdir -p ~/.config/pulse/ && cp -r /etc/pulse/* "$_"

  pulseaudio -D --exit-idle-time=-1 --system --disallow-exit

  # Create a virtual speaker output

  pactl load-module module-null-sink sink_name=SpeakerOutput
  pactl set-default-sink SpeakerOutput
  pactl set-default-source SpeakerOutput.monitor

  # Make config file
  echo -e "[General]\nsystem.audio.type=default" > ~/.config/zoomus.conf
}

build() {
  # Configure CMake if this is the first run
  [[ ! -d "$BUILD" ]] && {
    cmake -B "$BUILD" -S . --preset debug || exit;
    npm --prefix=client install
  }

  # Rename the shared library
  LIB="lib/zoomsdk/libmeetingsdk.so"
  [[ ! -f "${LIB}.1" ]] && cp "$LIB"{,.1}

  # Set up and start pulseaudio
  setup-pulseaudio &> /dev/null || exit;

  # Build the Source Code
  cmake --build "$BUILD"
}

run() {
    exec ./"$BUILD"/zoomsdk
}

build && run;

exit $?

