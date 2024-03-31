# setup env variables for HSDS
export ROOT_DIR=${HOME}/hsds/data
export BUCKET_NAME=hsdstest
export HEAD_PORT=5100
export HEAD_RAM=512m
export SN_PORT=5101
export SN_RAM=3g
export SN_CORES=1
export DN_PORT=6101
export DN_RAM=3g
export DN_CORES=4
export LOG_LEVEL=DEBUG
export HSDS_ENDPOINT=http://localhost:${SN_PORT}

# environment variables for HSDS clients
export HS_USERNAME=${USER}
export HS_PASSWORD=${USER}