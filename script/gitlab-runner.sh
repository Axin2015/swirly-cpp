#!/bin/bash
target=${1:-build-debug-gcc}
exec gitlab-runner exec docker "$target"
