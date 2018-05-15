#!/bin/bash
set -e

# This script assumes Debian GNU/Linux on WSL.
# Derived from https://github.com/JetBrains/clion-wsl/blob/master/ubuntu_setup_env.sh

SSHD_PORT=2222
SSHD_FILE=/etc/ssh/sshd_config
SUDOERS_FILE=/etc/sudoers

# 0. update package lists
sudo apt-get update
sudo apt-get install -y gnupg2 wget
echo 'deb http://deb.debian.org/debian unstable main' \
    | sudo tee -a /etc/apt/sources.list
echo 'deb http://apt.llvm.org/stretch/ llvm-toolchain-stretch-6.0 main' \
    | sudo tee -a /etc/apt/sources.list
wget --no-check-certificate -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
sudo apt-get update
sudo apt-get install -y clang-6.0 clang-format-6.0 clang-tools-6.0 cmake g++-8 gdb git \
     libboost-date-time1.63-dev libboost-test1.63-dev libsqlite3-dev sqlite3 valgrind vim

# 0.1. reinstall sshd (workaround for initial version of WSL)
sudo apt remove -y --purge openssh-server
sudo apt install -y openssh-server

# 1.2. configure sshd
sudo cp $SSHD_FILE ${SSHD_FILE}.`date '+%Y-%m-%d_%H-%M-%S'`.bak
sudo sed -i '/^Port/ d' $SSHD_FILE
sudo sed -i '/^UsePrivilegeSeparation/ d' $SSHD_FILE
sudo sed -i '/^PasswordAuthentication/ d' $SSHD_FILE
echo "# configured by CLion"      | sudo tee -a $SSHD_FILE
echo "Port ${SSHD_PORT}"          | sudo tee -a $SSHD_FILE
echo "UsePrivilegeSeparation no"  | sudo tee -a $SSHD_FILE
echo "PasswordAuthentication yes" | sudo tee -a $SSHD_FILE
# 1.3. apply new settings
sudo service ssh --full-restart

# 2. autostart: run sshd
sed -i '/^sudo service ssh --full-restart/ d' ~/.bashrc
echo "%sudo ALL=(ALL) NOPASSWD: /usr/sbin/service ssh --full-restart" | sudo tee -a $SUDOERS_FILE
cat << 'EOF' >> ~/.bashrc
sshd_status=$(service ssh status)
if [[ $sshd_status = *"is not running"* ]]; then
   sudo service ssh --full-restart
fi
EOF

# summary: SSHD config info
echo
echo "SSH server parameters ($SSHD_FILE):"
echo "Port ${SSHD_PORT}"
echo "UsePrivilegeSeparation no"
echo "PasswordAuthentication yes"
