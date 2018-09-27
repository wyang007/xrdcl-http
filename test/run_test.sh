#!/bin/sh

set -e

# Integration script for the XrdClHttp plugin

CADDY_URL=http://ecsft.cern.ch:80/dist/cvmfs/builddeps/caddy-linux-amd64-webdav
CADDY_SHA1=3b97ad2b1ab7c575768016d9c4f326cf97a42c1f


SCRIPT_LOCATION=$(cd "$(dirname "$0")"; pwd)
. ${SCRIPT_LOCATION}/common.sh

TEST_CASE_PATTERN="*"
if [ x"$1" != x"" ]; then
    TEST_CASE_PATTERN=$1
fi

# Additional commands
check_executable wget

# Preconditions (XRootD and XrdClHttp in $XROOTD_PREFIX)
check_prefix $XROOTD_PREFIX
PATH=$XROOTD_PREFIX/bin:$PATH
LD_LIBRARY_PATH=$XROOTD_PREFIX/lib:$LD_LIBRARY_PATH

# Set up the workspace for the test run
WORKSPACE=$(create_workspace)
echo "Using test workspace: $WORKSPACE"

cd $WORKSPACE

# Copy all test config files into the workspace
cp -r $SCRIPT_LOCATION/config $WORKSPACE/

# Set up the XRootD client HTTP plugin configuration
sed -i -e "s:<<XROOTD_PREFIX>>:$XROOTD_PREFIX:g" $WORKSPACE/config/client/http.conf
XRD_PLUGINCONFDIR=$WORKSPACE/config/client

# Download the caddy HTTP server, if needed
CADDY_EXEC=/tmp/caddy
CADDY_PID_FILE=$WORKSPACE/caddy_pid
CADDY_LOG=$WORKSPACE/caddy.log

if [ ! -f $CADDY_EXEC ] || [ x"$(file_sha1 $CADDY_EXEC)" != x"$CADDY_SHA1" ]; then
    echo "Downloading: $CADDY_URL"
    wget -q -O $CADDY_EXEC $CADDY_URL
    chmod +x $CADDY_EXEC
fi

# Run all the test cases
set +e
TEST_CASES=$(ls -d $SCRIPT_LOCATION/cases/$TEST_CASE_PATTERN)
for c in $TEST_CASES ; do
    run_test_case $c &
    wait $!
done

echo "Finished."
