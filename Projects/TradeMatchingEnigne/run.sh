#!/bin/bash

BASHRC="$HOME/.bashrc"
FUNC_NAME="tme"
PY_SCRIPT="$HOME/tme.py"  # path to your python script

# Python script wrapper function
read -r -d '' FUNC_DEF <<'EOF'
tme() {
    cp tme.py $HOME
    python3 "$HOME/tme.py" "$@"
}
EOF

# Check if function is already in .bashrc
if grep -q "tme()" "$BASHRC"; then
  echo "Function 'tme' already exists in $BASHRC. Skipping append."
else
  echo "Adding function 'tme' to $BASHRC..."
  echo "" >> "$BASHRC"
  echo "# Trade Matching Engine Python wrapper function" >> "$BASHRC"
  echo "$FUNC_DEF" >> "$BASHRC"
fi

# Reload bashrc
echo "Reloading $BASHRC..."
source "$BASHRC"

