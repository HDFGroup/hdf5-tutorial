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
  libhdf5-dev libhdf5-mpi-dev hdf5-tools libyajl-dev python3-pip
RUN conda install -n base h5py
RUN pip3 install h5pyd, s3fs
