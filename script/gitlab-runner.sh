#!/bin/bash
target=${1:-build-debug-gcc}
exec gitlab-runner exec docker "$target" \
     --docker-helper-image "gitlab/gitlab-runner-helper:x86_64-081978aa"
