# Vbox

安装MonoSAT
sudo apt update
sudo apt install zlib1g-dev
sudo apt install libgmp-dev
cmake .
make
sudo make install


Install Cobra

git clone https://github.com/DBCobra/CobraVerifier.git

Install dbcop

git clone -b oopsla-2019 https://github.com/WeihuaSun/dbcop.git


下载dbcop源代码

git clone -b oopsla-2019 https://github.com/WeihuaSun/dbcop.git

安装cargo

curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

source $HOME/.cargo/env

编译

cd dbcop
apt-install clang
cargo install --path .

运行