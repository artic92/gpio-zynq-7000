# Use the GHDL image as the base image
FROM ghdl/ext:latest

# Set metadata
LABEL maintainer=hi@ariccio.me
ENV REFRESHED_AT=2025-05-27

# Update the package list and install sudo and other necessary packages
RUN apt-get update && apt-get install -y sudo doxygen graphviz git

# Create a volume at /workspace
VOLUME [ "/workspace" ]

# Create a user named 'dev' with a home directory
RUN useradd -m -s /bin/bash dev

# Set the password for the 'dev' user (change 'password' to your desired password)
RUN echo 'dev:password' | chpasswd

# Add the 'dev' user to the 'sudo' group
RUN usermod -aG sudo dev

# Allow 'sudo' group members to execute any command without a password
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Set the working directory to /workspace
WORKDIR /workspace

# Switch to the 'dev' user
USER dev

# By default, the container will just open a shell as the 'dev' user in /workspace
CMD ["/bin/bash"]