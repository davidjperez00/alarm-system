idf_component_register(
    SRCS main.c
    INCLUDE_DIRS .
    REQUIRES i2s     # optional if you want to enforce subsystem dependency
)
