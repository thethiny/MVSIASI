# MVSI Loader
This will transform MVS game into MVS Infinite

### Current Features:
- Anti Pak Signature Patch. Allows you to run `.pak` files with invalid `.sig` signatures.
- Anti uToc Signature Patch. Allows you to use `uToc` files (and `.uCas`) with invalid Signature Headers
- Custom Server: Allows you to change the server endpoint

### How to use:
Download the Latest MVSI version, and select the mods you want in the provided ini file. When a game updates, you might need to change the patches in the [Patterns] section. The updated patterns will be provided by me below, and in the downloaded file.
The zip file will contain 3 files. MVSI.asi, MVSI.ini, and dsound.dll. You need to place the files into your game's folder, next to tbb.dll. `MVSFolder/Binaries/Win64/`.

### All Ini Settings
```ini
    [Debug]
    bEnableConsoleWindow = false
    bPauseOnStart = false
    bDebug = false

    [Settings]
    iLogSize = 100
    iLogLevel = 19

    [Patches.AntiCheat]
    bDisableSignatureCheck = true

    [Patterns]
    pSigCheck = 48 8D 0D ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 83 EC 28 E8 ? ? ? ? 48 89 05 ? ? ? ? 48 83 C4 28 C3 CC CC CC CC CC CC CC CC CC CC CC 48 8D 0D ? ? ? ? E9 ? ? ? ? CC CC CC CC 48 8D 0D ? ? ? ? E9 ? ? ? ? CC CC CC CC
    pEndpointLoader = 48 8b cb 48 8d 15 ? ? ? ? E8 ? ? ? ? 48 8b 4c 24 ? 48 85 c9 74 05 E8 ? ? ? ? 48 8b c3 48 8b 5c 24

    [Server]
    ; This proxies only connection to the server. It does not proxy other stuff or connections to WB Network servers.
    bEnableServerProxy = false
    szServerUrl = http://localhost:12181/
    ;szServerUrl = https://dokken-api.wbagora.com.

```
You can find the [sample ini file](sample.ini) in this repo

## Download
[Download Here](https://github.com/thethiny/MVSIASI/releases)

## Credits
- Ermaccer for the [dll loader](https://github.com/ermaccer/MK1Hook/releases/)
- doubleaa (multiversuskoth) as a colaborator
- The Generic SigCheck bypass for UE5 by unknown
