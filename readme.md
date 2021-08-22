# Sourcemod Sentry Error Logger
Log sourcemod errors and sourcepawn exceptions to a sentry.io instance
Originaly made for use on Creators.TF servers.

## Setup
On your Sentry.io instance, set up a project and get the SDK DSN url. Use this as the value for ``ce_sentry_dsn_url``.

## Requirements
A TF2 server running an up to date version of sourcemod.

Linux servers require curl4 to be installed.

## Convars
``ce_sentry_dsn_url`` Sentry DSN url, required.

``ce_logreaderwaittime`` How many seconds between re checking for new log errors.

``ce_server_index`` Server number (not required)

``ce_environment`` Server environment (not required)

``ce_region`` Server region (not required)

``ce_type`` Server type (not required)


## Building
Ensure you have the required dependencies for [building Sourcemod](https://wiki.alliedmods.net/Building_sourcemod).

Currently, the sourcemod and sdk dependency locations are assumed to be where they exist for the sample extensions, so clone this repo into <sourcemod repo>/public folder. Metamod, Sourcemod and hl2sdk-tf2 are needed.
### Linix:
Use the provided make file via ``make``. Requires GCC 8 and above for full STD 17 support (w filesystem). 
 
Ambuild cannot be used due to linker errors.
### Windows:
Open and build using the solution file (``ctferrorloggerext.sln``). Ambuild can also be used for windows.
