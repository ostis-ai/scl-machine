#!/usr/bin/env bash

APP_ROOT_PATH=$(cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && cd ../.. && pwd)

cd "$(dirname "$0")"

set -eo pipefail

python3 ${APP_ROOT_PATH}/scripts/clang/format-code.py ${APP_ROOT_PATH}/problem-solver/cxx/
