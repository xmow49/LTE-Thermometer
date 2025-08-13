ARG DOCKER_TAG=latest
FROM espressif/idf:release-v5.5

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update -y && apt-get install udev sudo -y


ARG CONTAINER_USER=esp
ARG USER_UID=1010
ARG USER_GID=$USER_UID

# Vérifier et gérer l'utilisateur existant avec ce UID
RUN if id -u $USER_UID >/dev/null 2>&1; then \
    EXISTING_USER=$(id -nu $USER_UID); \
    echo "Suppression de l'utilisateur existant: $EXISTING_USER"; \
    userdel $EXISTING_USER; \
    fi

# Vérifier et supprimer le groupe existant si nécessaire  
RUN if getent group $USER_GID >/dev/null 2>&1; then \
    EXISTING_GROUP=$(getent group $USER_GID | cut -d: -f1); \
    echo "Suppression du groupe existant: $EXISTING_GROUP"; \
    groupdel $EXISTING_GROUP; \
    fi

# Créer le nouveau groupe et utilisateur
RUN groupadd --gid $USER_GID $CONTAINER_USER \
    && adduser --uid $USER_UID --gid $USER_GID --disabled-password --gecos "" ${CONTAINER_USER} \
    && usermod -a -G root $CONTAINER_USER \
    && usermod -a -G dialout $CONTAINER_USER

RUN echo "" >> /etc/sudoers && echo "$CONTAINER_USER ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

USER ${CONTAINER_USER}
ENV USER=${CONTAINER_USER}
WORKDIR /home/${CONTAINER_USER}

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]

CMD ["/bin/bash", "-c"]
