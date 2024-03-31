FROM mcr.microsoft.com/devcontainers/miniconda:0-3

RUN conda install -n base -c conda-forge mamba

# Copy environment.yml (if found) to a temp location so we update the environment. Also
# copy "noop.txt" so the COPY instruction does not fail if no environment.yml exists.
RUN conda install -n base -c conda-forge mamba
COPY environment.yml* .devcontainer/noop.txt /tmp/conda-tmp/
RUN if [ -f "/tmp/conda-tmp/environment.yml" ]; then umask 0002 && /opt/conda/bin/mamba env create -f /tmp/conda-tmp/environment.yml; fi \
    && rm -rf /tmp/conda-tmp

RUN apt-get update && apt-get -y install --no-install-recommends \
  build-essential cmake libtool-bin git less wget \
  libhdf5-dev libhdf5-mpi-dev hdf5-tools libyajl-dev

# setup env variables for HSDS
ENV ROOT_DIR=/home/codespace/hsds/data
ENV BUCKET_NAME=hsdstest
ENV HEAD_PORT=5100
ENV HEAD_RAM=512m
ENV SN_PORT=5101
ENV SN_RAM=3g
ENV SN_CORES=1
ENV DN_PORT=6101
ENV DN_RAM=3g
ENV DN_CORES=4
ENV LOG_LEVEL=DEBUG
ENV HSDS_ENDPOINT=http://localhost:${SN_PORT}
# environment variables for HSDS clients
ENV HS_USERNAME=${USER}
ENV HS_PASSWORD=${USER}

