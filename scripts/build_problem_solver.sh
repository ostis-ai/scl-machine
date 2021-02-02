echo -en '\E[47;31m'"\033[1mBuild sc-machine\033[0m\n"
tput sgr0

APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd .. && pwd)
PLATFORM_PATH=${APP_ROOT_PATH}/ostis-web-platform
WORKING_PATH=$(pwd)

cd "${PLATFORM_PATH}"/sc-machine
if [ ! -d "./build" ]
	then
		mkdir build
fi

cd build
cmake "${APP_ROOT_PATH}"
make

cd "${WORKING_PATH}"
