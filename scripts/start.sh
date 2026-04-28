#!/bin/bash
# KAAL OS Linux Launcher

DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$DIR"

echo -e "\033[1;36m[~] Building KAAL OS...\033[0m"
make -s

if [ $? -eq 0 ]; then
    echo -e "\033[1;32m[+] Build successful. Launching in new terminal...\033[0m"
    
    BINARY="$DIR/build/kaal_os"
    CMD="cd '$DIR/build' && '$BINARY'; exec bash"

    if command -v gnome-terminal &>/dev/null; then
        gnome-terminal -- bash -c "$CMD"
    elif command -v konsole &>/dev/null; then
        konsole -e bash -c "$CMD"
    elif command -v qterminal &>/dev/null; then
        qterminal -e bash -c "$CMD"
    elif command -v xterm &>/dev/null; then
        xterm -e "bash -c '$CMD'"
    else
        echo -e "\033[1;33m[!] No GUI terminal found. Running in place.\033[0m"
        cd "$DIR/build" && "$BINARY"
    fi
else
    echo -e "\033[1;31m[-] Build failed. Check compiler errors above.\033[0m"
fi
